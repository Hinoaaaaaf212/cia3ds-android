// cia3ds_jni.cpp - JNI surface for the on-device 3DS .cia/.3ds decryptor.
//
// Calls renamed ctrtool_main() / makerom_main() in-process to perform the
// same multi-step pipeline that the original Windows .bat script ran:
//
//   1. ctrtool --info               -> read TitleId, version, content layout
//   2. ctrtool --contents=<tmp>/c   -> extract decrypted NCCH partitions
//   3. ncch_flags_mark_decrypted()  -> mark each partition as plaintext
//   4. makerom -f cia ...           -> rebuild the CIA from the partitions
//
// The Kotlin layer hands us int file descriptors from
// ContentResolver.openFileDescriptor; we materialize the input/output to a
// real file path under cacheDir so the C/C++ tools (which use FILE*) can
// fopen() them. Output is then copied back to the caller's Uri-backed fd.
//
// SPDX-License-Identifier: MIT

#include <jni.h>
#include <android/log.h>

#include <cerrno>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

extern "C" {
#include "ncch_flags.h"
// makerom is pure C, so its renamed entrypoint has C linkage.
int makerom_main(int argc, char *argv[]);
}

// ctrtool is C++; its main() takes (argc, argv, envp) and we keep that
// signature so the mangled symbol matches what's in libctrtool_static.a.
int ctrtool_main(int argc, char *argv[], char *envp[]);

#define LOG_TAG "cia3ds-jni"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace {

// Identifies the kind of CIA we're rebuilding so the makerom flags match
// what the original .bat script produced. Values mirror the result enum on
// the Kotlin side.
enum class CiaKind {
    Game = 0,
    Demo = 1,
    System = 2,
    DLC = 3,
    Patch = 4,
    TWL = 5,
    Unknown = 6,
};

struct ProgressReporter {
    JNIEnv *env;
    jobject callback;
    jmethodID onProgress;

    void post(int percent, const std::string &msg) const {
        if (!callback || !onProgress) return;
        jstring jmsg = env->NewStringUTF(msg.c_str());
        env->CallVoidMethod(callback, onProgress, (jint)percent, jmsg);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        env->DeleteLocalRef(jmsg);
    }
};

struct LogSink {
    JNIEnv *env;
    jobject callback;
    jmethodID onLine;

    void emit(const std::string &line) const {
        // Always echo to logcat so users (and bug reports) see it.
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s", line.c_str());
        if (!callback || !onLine) return;
        jstring jline = env->NewStringUTF(line.c_str());
        env->CallVoidMethod(callback, onLine, jline);
        if (env->ExceptionCheck()) env->ExceptionClear();
        env->DeleteLocalRef(jline);
    }

    void emitf(const char *fmt, ...) const __attribute__((format(printf, 2, 3))) {
        char buf[1024];
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);
        emit(buf);
    }

    void emitBlock(const std::string &text) const {
        // Split a multi-line capture into individual lines so the UI can show
        // each as a separate log row. Wrap in a local frame because emit()
        // creates a jstring per line and ctrtool/makerom output can be tens
        // of thousands of lines, blowing the 512-entry local-ref table.
        if (env) env->PushLocalFrame(16);
        size_t start = 0;
        size_t emitted = 0;
        while (start < text.size()) {
            size_t nl = text.find('\n', start);
            std::string line = (nl == std::string::npos)
                               ? text.substr(start)
                               : text.substr(start, nl - start);
            while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) {
                line.pop_back();
            }
            if (!line.empty()) {
                emit(line);
                if (env && (++emitted % 64) == 0) {
                    env->PopLocalFrame(nullptr);
                    env->PushLocalFrame(16);
                }
            }
            if (nl == std::string::npos) break;
            start = nl + 1;
        }
        if (env) env->PopLocalFrame(nullptr);
    }
};

// Helper: construct a vector<char*> argv from a string list, plus a sentinel.
class Argv {
public:
    explicit Argv(std::initializer_list<std::string> list) : owned_(list) {
        rebuild();
    }

    void push(const std::string &s) {
        owned_.push_back(s);
        rebuild();
    }

    int size() const { return (int)owned_.size(); }
    char **data() { return ptrs_.data(); }

private:
    void rebuild() {
        ptrs_.clear();
        ptrs_.reserve(owned_.size() + 1);
        for (auto &s : owned_) {
            ptrs_.push_back(const_cast<char *>(s.c_str()));
        }
        ptrs_.push_back(nullptr);
    }

    std::vector<std::string> owned_;
    std::vector<char *> ptrs_;
};

bool make_dir_p(const std::string &path) {
    if (path.empty()) return true;
    if (mkdir(path.c_str(), 0700) == 0) return true;
    if (errno == EEXIST) return true;
    LOGE("mkdir %s failed: %s", path.c_str(), strerror(errno));
    return false;
}

bool rmtree(const std::string &path) {
    DIR *d = opendir(path.c_str());
    if (!d) return errno == ENOENT;
    bool ok = true;
    struct dirent *e;
    while ((e = readdir(d)) != nullptr) {
        std::string name = e->d_name;
        if (name == "." || name == "..") continue;
        std::string sub = path + "/" + name;
        struct stat st;
        if (lstat(sub.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            ok = rmtree(sub) && ok;
        } else {
            if (unlink(sub.c_str()) != 0) ok = false;
        }
    }
    closedir(d);
    if (rmdir(path.c_str()) != 0) ok = false;
    return ok;
}

bool copy_fd_to_path(int fd, const std::string &path) {
    if (fd < 0) return false;
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1 && errno != ESPIPE) {
        // Some pipe-style fds aren't seekable; we'll just read forward.
    }
    int out = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (out < 0) {
        LOGE("open %s for write: %s", path.c_str(), strerror(errno));
        return false;
    }
    constexpr size_t kBuf = 256 * 1024;
    std::unique_ptr<char[]> buf(new char[kBuf]);
    ssize_t n;
    while ((n = read(fd, buf.get(), kBuf)) > 0) {
        ssize_t w = 0;
        while (w < n) {
            ssize_t k = write(out, buf.get() + w, (size_t)(n - w));
            if (k <= 0) {
                close(out);
                return false;
            }
            w += k;
        }
    }
    close(out);
    return n == 0;
}

bool copy_path_to_fd(const std::string &path, int fd) {
    int in = open(path.c_str(), O_RDONLY);
    if (in < 0) {
        LOGE("open %s for read: %s", path.c_str(), strerror(errno));
        return false;
    }
    constexpr size_t kBuf = 256 * 1024;
    std::unique_ptr<char[]> buf(new char[kBuf]);
    ssize_t n;
    bool ok = true;
    while ((n = read(in, buf.get(), kBuf)) > 0) {
        ssize_t w = 0;
        while (w < n) {
            ssize_t k = write(fd, buf.get() + w, (size_t)(n - w));
            if (k <= 0) { ok = false; break; }
            w += k;
        }
        if (!ok) break;
    }
    if (n < 0) ok = false;
    close(in);
    return ok;
}

// Capture stdout/stderr produced by ctrtool/makerom into a temp file and
// return its contents. Used both for parsing ctrtool --info output and for
// surfacing native errors back to the Kotlin layer when tools fail.
class StdoutCapture {
public:
    StdoutCapture(const std::string &path) : path_(path), saved_stdout_(-1), saved_stderr_(-1) {
        fflush(stdout);
        fflush(stderr);
        saved_stdout_ = dup(STDOUT_FILENO);
        saved_stderr_ = dup(STDERR_FILENO);
        int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (fd >= 0) {
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
    }

    ~StdoutCapture() { restore(); }

    void restore() {
        if (saved_stdout_ < 0 && saved_stderr_ < 0) return;
        fflush(stdout);
        fflush(stderr);
        if (saved_stdout_ >= 0) {
            dup2(saved_stdout_, STDOUT_FILENO);
            close(saved_stdout_);
            saved_stdout_ = -1;
        }
        if (saved_stderr_ >= 0) {
            dup2(saved_stderr_, STDERR_FILENO);
            close(saved_stderr_);
            saved_stderr_ = -1;
        }
    }

    std::string read() {
        restore();
        std::ifstream f(path_);
        std::stringstream ss;
        ss << f.rdbuf();
        return ss.str();
    }

private:
    std::string path_;
    int saved_stdout_;
    int saved_stderr_;
};

struct CiaInfo {
    std::string title_id;
    std::string title_version;
    bool already_decrypted = false;
    bool is_3ds = false;
    CiaKind kind = CiaKind::Unknown;
};

CiaKind classify_kind(const std::string &title_id_hex) {
    // Title-id high-32 nibbles, as used by the original .bat:
    //   00040000  -> Game (eShop / cartridge)
    //   00040002  -> Demo
    //   00040010  -> System app
    //   0004000e  -> Patch / update
    //   0004008c  -> DLC
    //   00048005, 0004800f, 00048004 -> TWL (DSiWare)
    if (title_id_hex.size() < 8) return CiaKind::Unknown;
    std::string hi = title_id_hex.substr(0, 8);
    for (auto &c : hi) c = (char)tolower(c);
    if (hi == "00040000") return CiaKind::Game;
    if (hi == "00040002") return CiaKind::Demo;
    if (hi == "0004000e") return CiaKind::Patch;
    if (hi == "0004008c") return CiaKind::DLC;
    if (hi == "00040010" || hi == "0004001b" || hi == "00040030"
        || hi == "0004009b" || hi == "000400db" || hi == "00040130"
        || hi == "00040138") return CiaKind::System;
    if (hi == "00048005" || hi == "0004800f" || hi == "00048004")
        return CiaKind::TWL;
    return CiaKind::Unknown;
}

CiaInfo run_ctrtool_info(const std::string &input_path,
                         const std::string &seeddb_path,
                         const std::string &capture_path,
                         const LogSink &sink) {
    CiaInfo out;
    sink.emitf("$ ctrtool --info --seeddb=%s %s",
               seeddb_path.c_str(), input_path.c_str());
    StdoutCapture cap(capture_path);
    Argv argv({"ctrtool", "--info", "--seeddb=" + seeddb_path, input_path});
    char *empty_envp[] = {nullptr};
    int rc = ctrtool_main(argv.size(), argv.data(), empty_envp);
    std::string text = cap.read();
    sink.emitBlock(text);
    sink.emitf("[ctrtool --info exit=%d]", rc);

    // Parse "Title id:" or "Title ID:" (case varies between CIA and CCI).
    // We accept the first 16 hex chars after the colon.
    std::regex tid_re(R"(Title\s+[iI][dD]\s*:\s*([0-9a-fA-F]{16}))");
    std::smatch m;
    if (std::regex_search(text, m, tid_re)) {
        out.title_id = m[1].str();
        out.kind = classify_kind(out.title_id);
    }
    // "Title version:" appears in TMD section; falls back to 0.
    std::regex ver_re(R"(Title\s+[vV]ersion\s*:\s*(\d+))");
    if (std::regex_search(text, m, ver_re)) {
        out.title_version = m[1].str();
    } else {
        out.title_version = "0";
    }
    // "Crypto Type:  None" / "Crypto Type:  Decrypted" -> already plaintext.
    std::regex ct_re(R"(Crypto\s+Type\s*:\s*(None|Decrypted))",
                     std::regex::icase);
    if (std::regex_search(text, m, ct_re)) {
        out.already_decrypted = true;
    }
    // 3DS NCSD inputs don't have a TitleMetaData header but ctrtool prints
    // "NCSD:" early. Used to switch to CCI rebuild.
    if (text.find("NCSD:") != std::string::npos
        && text.find("CIA:") == std::string::npos) {
        out.is_3ds = true;
    }
    return out;
}

bool list_extracted_partitions(const std::string &dir_path,
                               std::vector<std::string> &out) {
    DIR *d = opendir(dir_path.c_str());
    if (!d) return false;
    struct dirent *e;
    while ((e = readdir(d)) != nullptr) {
        std::string name = e->d_name;
        if (name == "." || name == "..") continue;
        // ctrtool writes content files like c.0000.00000000 and
        // c.0001.00000001 etc. inside the chosen --contents directory.
        if (name.find("c.") == 0) {
            out.push_back(dir_path + "/" + name);
        }
    }
    closedir(d);
    std::sort(out.begin(), out.end());
    return !out.empty();
}

const char *kind_to_suffix(CiaKind k) {
    switch (k) {
        case CiaKind::Game: return "Game";
        case CiaKind::Demo: return "Demo";
        case CiaKind::System: return "System";
        case CiaKind::DLC: return "DLC";
        case CiaKind::Patch: return "Patch";
        case CiaKind::TWL: return "TWL";
        default: return "Decrypted";
    }
}

} // namespace

extern "C" JNIEXPORT jint JNICALL
Java_io_github_cia3ds_jni_Cia3ds_nativeDecryptCia(
    JNIEnv *env, jobject /*thiz*/,
    jint inFd, jint outFd,
    jstring jSeedDb, jstring jTmpDir,
    jstring jOriginalName, jboolean wantCci,
    jobject progressCallback,
    jobject logCallback) {

    const char *seeddb_c = env->GetStringUTFChars(jSeedDb, nullptr);
    const char *tmp_c    = env->GetStringUTFChars(jTmpDir, nullptr);
    const char *orig_c   = env->GetStringUTFChars(jOriginalName, nullptr);
    std::string seeddb_path = seeddb_c;
    std::string tmp_dir = tmp_c;
    std::string orig_name = orig_c;
    env->ReleaseStringUTFChars(jSeedDb, seeddb_c);
    env->ReleaseStringUTFChars(jTmpDir, tmp_c);
    env->ReleaseStringUTFChars(jOriginalName, orig_c);

    ProgressReporter progress{env, progressCallback, nullptr};
    if (progressCallback) {
        jclass cls = env->GetObjectClass(progressCallback);
        progress.onProgress = env->GetMethodID(cls, "onProgress", "(ILjava/lang/String;)V");
        env->DeleteLocalRef(cls);
    }
    LogSink sink{env, logCallback, nullptr};
    if (logCallback) {
        jclass cls = env->GetObjectClass(logCallback);
        sink.onLine = env->GetMethodID(cls, "onLine", "(Ljava/lang/String;)V");
        env->DeleteLocalRef(cls);
    }

    // Outer local frame so any leaked jstring during error paths is freed
    // when the JNI call returns. RAII guard so every return path pops it.
    struct LocalFrameGuard {
        JNIEnv *e;
        LocalFrameGuard(JNIEnv *e, jint cap) : e(e) { e->PushLocalFrame(cap); }
        ~LocalFrameGuard() { e->PopLocalFrame(nullptr); }
    } _frame{env, 64};

    sink.emitf("== cia3ds: decrypt %s (wantCci=%d) ==",
               orig_name.c_str(), (int)wantCci);

    if (!make_dir_p(tmp_dir)) {
        sink.emitf("ERR: cannot create tmp dir %s", tmp_dir.c_str());
        return 1;
    }
    std::string work = tmp_dir + "/work";
    rmtree(work);
    if (!make_dir_p(work)) {
        sink.emitf("ERR: cannot create work dir %s", work.c_str());
        return 1;
    }

    std::string input_path  = work + "/input.bin";
    std::string output_path = work + "/output.bin";
    std::string contents_dir = work + "/contents";
    std::string log_path = work + "/tool.log";
    if (!make_dir_p(contents_dir)) {
        sink.emitf("ERR: cannot create contents dir %s", contents_dir.c_str());
        return 1;
    }

    progress.post(2, "Staging input");
    if (!copy_fd_to_path(inFd, input_path)) {
        sink.emitf("ERR: failed to stage input fd to %s", input_path.c_str());
        return 2;
    }
    {
        struct stat st;
        if (stat(input_path.c_str(), &st) == 0) {
            sink.emitf("staged input: %lld bytes",
                       (long long)st.st_size);
        }
    }

    progress.post(10, "Reading metadata");
    CiaInfo info = run_ctrtool_info(input_path, seeddb_path, log_path, sink);
    sink.emitf("metadata: title=%s version=%s kind=%d already=%d is_3ds=%d",
               info.title_id.empty() ? "(unknown)" : info.title_id.c_str(),
               info.title_version.c_str(),
               (int)info.kind, (int)info.already_decrypted, (int)info.is_3ds);

    if (info.already_decrypted) {
        progress.post(100, "Already decrypted; nothing to do.");
        sink.emit("input is already decrypted; copying input to output");
        if (!copy_path_to_fd(input_path, outFd)) {
            sink.emit("ERR: copy of input to output fd failed");
            return 4;
        }
        rmtree(work);
        return 10; // sentinel: "already decrypted"
    }

    progress.post(20, "Extracting partitions");
    {
        std::string contents_prefix = contents_dir + "/c";
        sink.emitf("$ ctrtool --seeddb=%s --contents=%s %s",
                   seeddb_path.c_str(), contents_prefix.c_str(),
                   input_path.c_str());
        StdoutCapture cap(log_path);
        Argv argv({
            "ctrtool",
            "--seeddb=" + seeddb_path,
            "--contents=" + contents_prefix,
            input_path,
        });
        char *empty_envp[] = {nullptr};
    int rc = ctrtool_main(argv.size(), argv.data(), empty_envp);
        std::string out = cap.read();
        sink.emitBlock(out);
        sink.emitf("[ctrtool extract exit=%d]", rc);
        if (rc != 0) {
            return 5;
        }
    }

    std::vector<std::string> partitions;
    if (!list_extracted_partitions(contents_dir, partitions)) {
        sink.emitf("ERR: no partitions extracted in %s", contents_dir.c_str());
        // Dump what's actually there to help diagnose.
        DIR *d = opendir(contents_dir.c_str());
        if (d) {
            struct dirent *e;
            while ((e = readdir(d)) != nullptr) {
                std::string n = e->d_name;
                if (n != "." && n != "..") sink.emitf("  contents/%s", n.c_str());
            }
            closedir(d);
        }
        return 6;
    }
    sink.emitf("extracted %zu partition(s)", partitions.size());
    for (auto &p : partitions) sink.emitf("  %s", p.c_str());

    progress.post(60, "Marking partitions decrypted");
    for (auto &p : partitions) {
        int rc = ncch_flags_mark_decrypted(p.c_str());
        if (rc != 0) {
            sink.emitf("ncch_flags_mark_decrypted(%s) -> %d (%s) [non-fatal]",
                       p.c_str(), rc, strerror(rc));
        }
    }

    progress.post(75, "Rebuilding CIA");
    {
        Argv argv({
            "makerom",
            "-f", "cia",
            "-ignoresign",
            "-target", "p",
            "-o", output_path,
        });
        if (info.kind == CiaKind::DLC) {
            argv.push("-dlc");
        }
        for (size_t i = 0; i < partitions.size(); ++i) {
            std::string spec = partitions[i] + ":" + std::to_string(i)
                             + ":" + std::to_string(i);
            argv.push("-i");
            argv.push(spec);
        }
        if (!info.title_version.empty()) {
            argv.push("-ver");
            argv.push(info.title_version);
        }
        // Echo full command line for transparency.
        std::string cmd = "$";
        for (int j = 0; j < argv.size(); ++j) cmd += " " + std::string(argv.data()[j]);
        sink.emit(cmd);
        StdoutCapture cap(log_path);
        int rc = makerom_main(argv.size(), argv.data());
        std::string out = cap.read();
        sink.emitBlock(out);
        sink.emitf("[makerom rebuild exit=%d]", rc);
        if (rc != 0) {
            return 7;
        }
    }

    progress.post(90, "Saving output");

    {
        struct stat st;
        if (stat(output_path.c_str(), &st) == 0) {
            sink.emitf("rebuilt CIA: %lld bytes", (long long)st.st_size);
        }
    }

    std::string final_input = output_path;
    if (wantCci && info.kind == CiaKind::Game) {
        std::string cci_path = work + "/output.cci";
        sink.emitf("$ makerom -ciatocci %s -o %s",
                   output_path.c_str(), cci_path.c_str());
        StdoutCapture cap(log_path);
        Argv argv({
            "makerom",
            "-ciatocci", output_path,
            "-o", cci_path,
        });
        int rc = makerom_main(argv.size(), argv.data());
        std::string out = cap.read();
        sink.emitBlock(out);
        sink.emitf("[makerom -ciatocci exit=%d]", rc);
        if (rc != 0) {
            sink.emit("WARN: ciatocci failed; keeping CIA");
        } else {
            final_input = cci_path;
        }
    }

    progress.post(90, "Saving output");
    if (!copy_path_to_fd(final_input, outFd)) {
        sink.emit("ERR: copy of result to caller fd failed");
        rmtree(work);
        return 8;
    }
    {
        struct stat st;
        if (stat(final_input.c_str(), &st) == 0) {
            sink.emitf("wrote %lld bytes to output", (long long)st.st_size);
        }
    }

    rmtree(work);
    progress.post(100, "Done");
    return 0;
}

extern "C" JNIEXPORT jstring JNICALL
Java_io_github_cia3ds_jni_Cia3ds_nativeVersion(JNIEnv *env, jobject /*thiz*/) {
    return env->NewStringUTF("cia3ds 1.0.0 (ctrtool+makerom)");
}
