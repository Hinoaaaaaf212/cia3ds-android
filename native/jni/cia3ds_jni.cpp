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
int makerom_main(int argc, char *argv[]);
}

int ctrtool_main(int argc, char *argv[], char *envp[]);

#define LOG_TAG "cia3ds-jni"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace {

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

struct SeedFetcher {
    JNIEnv *env;
    jobject callback;
    jmethodID onFetch;

    std::string fetch(const std::string &tid_hex) const {
        std::string out;
        if (!callback || !onFetch) return out;
        jstring jtid = env->NewStringUTF(tid_hex.c_str());
        jbyteArray jbytes = (jbyteArray)env->CallObjectMethod(callback, onFetch, jtid);
        env->DeleteLocalRef(jtid);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
            return out;
        }
        if (!jbytes) return out;
        jsize n = env->GetArrayLength(jbytes);
        if (n > 0) {
            out.resize((size_t)n);
            env->GetByteArrayRegion(jbytes, 0, n, (jbyte *)out.data());
        }
        env->DeleteLocalRef(jbytes);
        return out;
    }
};

struct LogSink {
    JNIEnv *env;
    jobject callback;
    jmethodID onLine;

    void emit(const std::string &line) const {
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
        // PushLocalFrame because emit() creates a jstring per line and
        // ctrtool/makerom output can be tens of thousands of lines, blowing
        // the 512-entry local-ref table.
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
    lseek(fd, 0, SEEK_SET);
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
    int info_rc = 0;
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
    out.info_rc = rc;

    std::regex tid_re(R"(Title\s+[iI][dD]\s*:\s*([0-9a-fA-F]{16}))");
    std::smatch m;
    if (std::regex_search(text, m, tid_re)) {
        out.title_id = m[1].str();
        out.kind = classify_kind(out.title_id);
    }
    std::regex ver_re(R"(Title\s+[vV]ersion\s*:\s*(\d+))");
    if (std::regex_search(text, m, ver_re)) {
        out.title_version = m[1].str();
    } else {
        out.title_version = "0";
    }
    std::regex ct_re(R"(Crypto\s+Type\s*:\s*(None|Decrypted))",
                     std::regex::icase);
    if (std::regex_search(text, m, ct_re)) {
        out.already_decrypted = true;
    }
    if (text.find("NCSD:") != std::string::npos
        && text.find("CIA:") == std::string::npos) {
        out.is_3ds = true;
    }
    return out;
}

struct Partition {
    std::string path;
    int slot;
    uint32_t content_id;
};

bool splice_region(const std::string &part_path,
                   uint64_t dst_offset,
                   const std::string &src_path) {
    int src = open(src_path.c_str(), O_RDONLY);
    if (src < 0) return false;
    int dst = open(part_path.c_str(), O_WRONLY);
    if (dst < 0) { close(src); return false; }
    if (lseek(dst, (off_t)dst_offset, SEEK_SET) == (off_t)-1) {
        close(src); close(dst); return false;
    }
    constexpr size_t kBuf = 256 * 1024;
    std::vector<char> buf(kBuf);
    ssize_t n;
    bool ok = true;
    while ((n = read(src, buf.data(), kBuf)) > 0) {
        ssize_t w = 0;
        while (w < n) {
            ssize_t k = write(dst, buf.data() + w, (size_t)(n - w));
            if (k <= 0) { ok = false; break; }
            w += k;
        }
        if (!ok) break;
    }
    if (n < 0) ok = false;
    close(src);
    close(dst);
    return ok;
}

// NCCH header layout (offsets within the partition file):
//   0x00  RSA-2048 signature                       (256 bytes)
//   0x100 NcchCommonHeader                         (256 bytes)
//     0x180 exhdr_size (u32, little-endian)
//   ExHeader region:    file offset 0x200, size = exhdr_size + 0x400 (acex)
//   ExeFs region:       file offset = exefs_blk_offset * 0x200, size = exefs_blk_size * 0x200
//   RomFs region:       file offset = romfs_blk_offset * 0x200, size = romfs_blk_size * 0x200
struct NcchRegions {
    uint64_t exhdr_off, exhdr_size;
    uint64_t exefs_off, exefs_size;
    uint64_t romfs_off, romfs_size;
};

bool read_ncch_regions(const std::string &part_path, NcchRegions &out) {
    FILE *f = fopen(part_path.c_str(), "rb");
    if (!f) return false;
    unsigned char hdr[0x200];
    size_t got = fread(hdr, 1, sizeof(hdr), f);
    fclose(f);
    if (got != sizeof(hdr)) return false;
    auto rd_u32 = [&](size_t off) -> uint32_t {
        return (uint32_t)hdr[off]
             | ((uint32_t)hdr[off+1] << 8)
             | ((uint32_t)hdr[off+2] << 16)
             | ((uint32_t)hdr[off+3] << 24);
    };
    uint32_t exhdr_size_field = rd_u32(0x180);
    uint32_t exefs_blk_off    = rd_u32(0x1A0);
    uint32_t exefs_blk_size   = rd_u32(0x1A4);
    uint32_t romfs_blk_off    = rd_u32(0x1B0);
    uint32_t romfs_blk_size   = rd_u32(0x1B4);
    // The NCCH header includes the exheader and access_descriptor immediately
    // after the common header. Their combined region (what ctrtool emits as
    // --exheader) is exhdr_size + 0x400 bytes starting at file offset 0x200.
    // BUT ctrtool's --exheader writes only the exhdr_size portion. The
    // access_descriptor follows it; we leave it in place because it's not
    // encrypted.
    out.exhdr_off  = 0x200;
    out.exhdr_size = exhdr_size_field;
    out.exefs_off  = (uint64_t)exefs_blk_off * 0x200ULL;
    out.exefs_size = (uint64_t)exefs_blk_size * 0x200ULL;
    out.romfs_off  = (uint64_t)romfs_blk_off * 0x200ULL;
    out.romfs_size = (uint64_t)romfs_blk_size * 0x200ULL;
    return true;
}

bool list_extracted_partitions(const std::string &dir_path,
                               std::vector<Partition> &out) {
    DIR *d = opendir(dir_path.c_str());
    if (!d) return false;
    struct dirent *e;
    // ctrtool --contents=<prefix> writes "<prefix>.NNNN.HHHHHHHH" files where
    // NNNN is the partition slot and HHHHHHHH is the lower 32 bits of the
    // content id (hex). We parse both because makerom's -i flag wants both.
    std::regex re(R"(^c\.([0-9]{4})\.([0-9a-fA-F]{8})$)");
    while ((e = readdir(d)) != nullptr) {
        std::string name = e->d_name;
        if (name == "." || name == "..") continue;
        std::smatch m;
        if (!std::regex_match(name, m, re)) continue;
        Partition p;
        p.path = dir_path + "/" + name;
        p.slot = std::stoi(m[1].str(), nullptr, 10);
        p.content_id = (uint32_t)std::stoul(m[2].str(), nullptr, 16);
        out.push_back(std::move(p));
    }
    closedir(d);
    std::sort(out.begin(), out.end(),
              [](const Partition &a, const Partition &b) { return a.slot < b.slot; });
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
        default: return "Unknown";
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
    jobject logCallback,
    jobject seedFetcherCallback) {

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
    SeedFetcher seedFetcher{env, seedFetcherCallback, nullptr};
    if (seedFetcherCallback) {
        jclass cls = env->GetObjectClass(seedFetcherCallback);
        seedFetcher.onFetch = env->GetMethodID(cls, "onFetch", "(Ljava/lang/String;)[B");
        env->DeleteLocalRef(cls);
    }

    struct LocalFrameGuard {
        JNIEnv *e;
        LocalFrameGuard(JNIEnv *e, jint cap) : e(e) { e->PushLocalFrame(cap); }
        ~LocalFrameGuard() { e->PopLocalFrame(nullptr); }
    } _frame{env, 64};

    sink.emitf("== cia3ds: decrypt %s (wantCci=%d) ==",
               orig_name.c_str(), (int)wantCci);

    auto emit_workdir_help = [&]() {
        sink.emit("Most common reasons:");
        sink.emit("  - The device is out of free storage.");
        sink.emit("  - The app's cache directory is unwritable.");
        sink.emit("Free up space (or clear the app's cache from Android settings) and try again.");
    };

    if (!make_dir_p(tmp_dir)) {
        sink.emitf("ERR: cannot create tmp dir %s", tmp_dir.c_str());
        emit_workdir_help();
        return 1;
    }
    std::string work = tmp_dir + "/work";
    rmtree(work);
    if (!make_dir_p(work)) {
        sink.emitf("ERR: cannot create work dir %s", work.c_str());
        emit_workdir_help();
        return 1;
    }

    std::string input_path  = work + "/input.bin";
    std::string output_path = work + "/output.bin";
    std::string contents_dir = work + "/contents";
    std::string log_path = work + "/tool.log";
    if (!make_dir_p(contents_dir)) {
        sink.emitf("ERR: cannot create contents dir %s", contents_dir.c_str());
        emit_workdir_help();
        return 1;
    }

    progress.post(2, "Staging input");
    if (!copy_fd_to_path(inFd, input_path)) {
        sink.emitf("ERR: failed to stage input fd to %s", input_path.c_str());
        sink.emit("Most common reasons:");
        sink.emit("  - The device ran out of free storage while copying.");
        sink.emit("  - The source file is on a removable drive that was disconnected.");
        sink.emit("  - The source file is no longer reachable through the file picker.");
        sink.emit("Free up space and re-pick the input file.");
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
    sink.emitf("META: title_id=%s",
               info.title_id.empty() ? "" : info.title_id.c_str());
    sink.emitf("META: kind=%s", kind_to_suffix(info.kind));
    sink.emitf("META: version=%s",
               info.title_version.empty() ? "0" : info.title_version.c_str());

    if (info.info_rc != 0 && info.title_id.empty()) {
        sink.emit("ERR: ctrtool could not identify this file as a CIA or 3DS.");
        sink.emit("Most common reasons:");
        sink.emit("  - The file is already decrypted (re-running an output is a no-op).");
        sink.emit("  - The file is not a Nintendo 3DS CIA/3DS at all.");
        sink.emit("  - The file is corrupt or truncated.");
        sink.emit("Pick a still-encrypted .cia or .3ds and try again.");
        rmtree(work);
        return 12;
    }

    std::string cdn_seed_hex;
    if (!info.title_id.empty() && seedFetcherCallback) {
        sink.emitf("seed-fetch: querying CDN for %s", info.title_id.c_str());
        std::string raw = seedFetcher.fetch(info.title_id);
        if (raw.size() == 16) {
            char buf[33];
            for (size_t i = 0; i < 16; ++i) {
                snprintf(buf + i * 2, 3, "%02x", (unsigned char)raw[i]);
            }
            cdn_seed_hex = buf;
            sink.emitf("seed-fetch: using CDN seed %s", cdn_seed_hex.c_str());
        } else {
            sink.emit("seed-fetch: CDN miss, falling back to bundled seeddb.bin");
        }
    }

    if (info.already_decrypted) {
        progress.post(100, "Already decrypted; nothing to do.");
        sink.emit("input is already decrypted; copying input to output");
        if (!copy_path_to_fd(input_path, outFd)) {
            sink.emit("ERR: copy of input to output fd failed.");
            sink.emit("The file is already decrypted and the engine tried to copy it to");
            sink.emit("the output you picked, but the write failed.");
            sink.emit("Most common reasons:");
            sink.emit("  - The destination ran out of free storage.");
            sink.emit("  - The destination folder is no longer writable.");
            sink.emit("Free up space and pick the output again.");
            return 4;
        }
        rmtree(work);
        return 10; // rc=10: already-decrypted sentinel
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
        });
        if (!cdn_seed_hex.empty()) argv.push("--seed=" + cdn_seed_hex);
        argv.push(input_path);
        char *empty_envp[] = {nullptr};
    int rc = ctrtool_main(argv.size(), argv.data(), empty_envp);
        std::string out = cap.read();
        sink.emitBlock(out);
        sink.emitf("[ctrtool extract exit=%d]", rc);
        if (rc != 0) {
            sink.emit("ERR: ctrtool failed while extracting partitions from this file.");
            sink.emit("Most common reasons:");
            sink.emit("  - The file is corrupt or truncated (incomplete download).");
            sink.emit("  - A required title key or seed is missing for this title id.");
            sink.emit("  - The file uses encryption keys this build cannot handle.");
            sink.emit("Try re-downloading the file, or pick a different one.");
            return 5;
        }
    }

    std::vector<Partition> partitions;
    if (!list_extracted_partitions(contents_dir, partitions)) {
        int file_count = 0;
        bool any_c_prefix = false;
        DIR *d = opendir(contents_dir.c_str());
        if (d) {
            struct dirent *e;
            while ((e = readdir(d)) != nullptr) {
                std::string n = e->d_name;
                if (n == "." || n == "..") continue;
                file_count++;
                if (n.rfind("c.", 0) == 0) any_c_prefix = true;
                sink.emitf("  contents/%s", n.c_str());
            }
            closedir(d);
        }
        if (file_count == 0) {
            sink.emit("ERR: this file appears to already be decrypted.");
            sink.emit("ctrtool extracted no encrypted partitions, which means the");
            sink.emit("input has already been processed. Try installing it directly");
            sink.emit("in your emulator, or pick a still-encrypted CIA/3DS file.");
            return 11;
        }
        if (any_c_prefix) {
            sink.emit("ERR: ctrtool wrote partition files but with an unexpected naming pattern.");
            sink.emit("This usually means the engine and ctrtool versions are out of sync,");
            sink.emit("which is a packaging bug rather than a problem with your file.");
            sink.emit("Please report this with the log above so it can be fixed.");
        } else {
            sink.emitf("ERR: ctrtool extracted files into %s, but none look like NCCH partitions.", contents_dir.c_str());
            sink.emit("Most common reasons:");
            sink.emit("  - The CIA/3DS is malformed or truncated.");
            sink.emit("  - The title uses an unusual layout the engine doesn't recognise.");
            sink.emit("Try re-downloading the file, or pick a different one.");
        }
        return 6;
    }
    sink.emitf("extracted %zu partition(s)", partitions.size());
    for (auto &p : partitions) {
        sink.emitf("  slot=%d id=0x%08x %s", p.slot, p.content_id, p.path.c_str());
    }

    auto emit_region_help = [&]() {
        sink.emit("Most common reasons:");
        sink.emit("  - The CIA/3DS is malformed or truncated.");
        sink.emit("  - The device ran out of free storage mid-decryption.");
        sink.emit("  - A required title key or seed is missing for this title id.");
        sink.emit("Try re-downloading the file, free up space, and try again.");
    };

    progress.post(50, "Decrypting NCCH regions");
    for (auto &p : partitions) {
        NcchRegions reg;
        if (!read_ncch_regions(p.path, reg)) {
            sink.emitf("ERR: cannot read NCCH header from %s", p.path.c_str());
            emit_region_help();
            return 9;
        }
        sink.emitf("partition slot=%d regions: exhdr@0x%llx(%llu) exefs@0x%llx(%llu) romfs@0x%llx(%llu)",
                   p.slot,
                   (unsigned long long)reg.exhdr_off, (unsigned long long)reg.exhdr_size,
                   (unsigned long long)reg.exefs_off, (unsigned long long)reg.exefs_size,
                   (unsigned long long)reg.romfs_off, (unsigned long long)reg.romfs_size);

        std::string region_dir = work + "/regions_" + std::to_string(p.slot);
        if (!make_dir_p(region_dir)) {
            sink.emitf("ERR: cannot create region dir %s", region_dir.c_str());
            emit_region_help();
            return 9;
        }
        std::string eh_path = region_dir + "/exheader.bin";
        std::string ef_path = region_dir + "/exefs.bin";
        std::string rf_path = region_dir + "/romfs.bin";

        sink.emitf("$ ctrtool -n %d --exheader=%s --exefs=%s --romfs=%s --seeddb=%s %s",
                   p.slot, eh_path.c_str(), ef_path.c_str(), rf_path.c_str(),
                   seeddb_path.c_str(), input_path.c_str());
        StdoutCapture cap(log_path);
        Argv argv({
            "ctrtool",
            "-n", std::to_string(p.slot),
            "--exheader=" + eh_path,
            "--exefs=" + ef_path,
            "--romfs=" + rf_path,
            "--seeddb=" + seeddb_path,
        });
        if (!cdn_seed_hex.empty()) argv.push("--seed=" + cdn_seed_hex);
        argv.push(input_path);
        char *empty_envp2[] = {nullptr};
        int rc = ctrtool_main(argv.size(), argv.data(), empty_envp2);
        std::string out = cap.read();
        sink.emitBlock(out);
        sink.emitf("[ctrtool decrypt-regions exit=%d]", rc);
        if (rc != 0) {
            sink.emit("ERR: ctrtool failed while decrypting NCCH regions for this partition.");
            emit_region_help();
            return 9;
        }

        struct stat st;
        if (reg.exhdr_size > 0 && stat(eh_path.c_str(), &st) == 0) {
            sink.emitf("  splicing exheader (%lld bytes) -> 0x%llx",
                       (long long)st.st_size, (unsigned long long)reg.exhdr_off);
            if (!splice_region(p.path, reg.exhdr_off, eh_path)) {
                sink.emit("ERR: failed to splice exheader back into the partition.");
                emit_region_help();
                return 9;
            }
        }
        if (reg.exefs_size > 0 && stat(ef_path.c_str(), &st) == 0) {
            sink.emitf("  splicing exefs (%lld bytes) -> 0x%llx",
                       (long long)st.st_size, (unsigned long long)reg.exefs_off);
            if (!splice_region(p.path, reg.exefs_off, ef_path)) {
                sink.emit("ERR: failed to splice exefs back into the partition.");
                emit_region_help();
                return 9;
            }
        }
        if (reg.romfs_size > 0 && stat(rf_path.c_str(), &st) == 0) {
            sink.emitf("  splicing romfs (%lld bytes) -> 0x%llx",
                       (long long)st.st_size, (unsigned long long)reg.romfs_off);
            if (!splice_region(p.path, reg.romfs_off, rf_path)) {
                sink.emit("ERR: failed to splice romfs back into the partition.");
                emit_region_help();
                return 9;
            }
        }
    }

    progress.post(60, "Marking partitions decrypted");
    for (auto &p : partitions) {
        int rc = ncch_flags_mark_decrypted(p.path.c_str());
        if (rc != 0) {
            sink.emitf("ncch_flags_mark_decrypted(%s) -> %d (%s) [non-fatal]",
                       p.path.c_str(), rc, strerror(rc));
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
        for (auto &p : partitions) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%d:0x%08x", p.slot, p.content_id);
            argv.push("-i");
            argv.push(p.path + ":" + buf);
        }
        if (!info.title_version.empty()) {
            argv.push("-ver");
            argv.push(info.title_version);
        }
        std::string cmd = "$";
        for (int j = 0; j < argv.size(); ++j) cmd += " " + std::string(argv.data()[j]);
        sink.emit(cmd);
        StdoutCapture cap(log_path);
        int rc = makerom_main(argv.size(), argv.data());
        std::string out = cap.read();
        sink.emitBlock(out);
        sink.emitf("[makerom rebuild exit=%d]", rc);
        if (rc != 0) {
            sink.emit("ERR: makerom failed to rebuild the CIA from the decrypted partitions.");
            sink.emit("Most common reasons:");
            sink.emit("  - The decrypted partitions on disk got corrupted (often disk full).");
            sink.emit("  - The title's metadata (kind/version) is in a shape makerom rejects.");
            sink.emit("  - This is a bug in the engine; please report it with the log above.");
            sink.emit("Free up space and try again, or pick a different file.");
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
    bool produced_ncsd = false;
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
            produced_ncsd = true;
        }
    } else if (wantCci) {
        sink.emitf("WARN: %s is not a Game title; keeping CIA",
                   kind_to_suffix(info.kind));
    }
    sink.emitf("META: format_actual=%s", produced_ncsd ? "ncsd" : "cia");

    progress.post(90, "Saving output");
    if (!copy_path_to_fd(final_input, outFd)) {
        sink.emit("ERR: copy of result to caller fd failed.");
        sink.emit("The decrypt finished, but writing the output to the file you picked failed.");
        sink.emit("Most common reasons:");
        sink.emit("  - The destination ran out of free storage.");
        sink.emit("  - The destination folder is no longer writable.");
        sink.emit("Free up space and pick the output again.");
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
