package io.github.cia3ds.jni

import android.content.Context
import android.net.Uri
import android.util.Log
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.callbackFlow
import kotlinx.coroutines.flow.flowOn
import kotlinx.coroutines.suspendCancellableCoroutine
import kotlinx.coroutines.withContext
import java.io.File
import kotlin.coroutines.resume

/**
 * Thin wrapper around libcia3ds.so. The native side does the heavy lifting;
 * this layer is responsible for materializing seeddb.bin, opening fds from
 * SAF Uris, and serializing access to the engine (the C/C++ tools we wrap
 * are not reentrant).
 */
class Cia3ds private constructor(private val appCtx: Context) {

    init {
        System.loadLibrary("cia3ds")
    }

    /** Native entrypoint; returns 0 on success, 10 if the input was already
     *  decrypted (and the original bytes were copied to the output), or any
     *  other non-zero value on failure. The native side treats Cci and
     *  ThreeDs identically; the only difference at the Kotlin layer is the
     *  suggested file extension. */
    private external fun nativeDecryptCia(
        inFd: Int,
        outFd: Int,
        seedDbPath: String,
        tmpDir: String,
        originalName: String,
        wantCci: Boolean,
        progress: NativeProgressCallback?,
    ): Int

    private external fun nativeVersion(): String

    fun version(): String = nativeVersion()

    /**
     * Decrypt a single .cia/.3ds file referenced by an SAF input Uri,
     * writing the result to the SAF output Uri.
     *
     * @param progressEmit invoked from a background thread with (percent, message).
     */
    suspend fun decrypt(
        input: Uri,
        output: Uri,
        format: OutputFormat,
        originalName: String,
        progressEmit: (Int, String) -> Unit,
    ): DecryptResult = withContext(Dispatchers.IO) {
        ensureSeedDb()
        val seedDbPath = seedDbFile().absolutePath
        val tmpDir = appCtx.cacheDir.resolve("cia3ds-work").apply { mkdirs() }.absolutePath

        val cr = appCtx.contentResolver
        val inPfd = cr.openFileDescriptor(input, "r")
            ?: return@withContext DecryptResult.Failure("Cannot open input")
        val outPfd = cr.openFileDescriptor(output, "rwt")
            ?: run {
                inPfd.close()
                return@withContext DecryptResult.Failure("Cannot open output")
            }

        try {
            // Run on a single, dedicated thread because ctrtool/makerom are not reentrant.
            val rc = suspendCancellableCoroutine<Int> { cont ->
                val t = Thread({
                    val cb = NativeProgressCallback { pct, msg -> progressEmit(pct, msg) }
                    val code = nativeDecryptCia(
                        inPfd.fd,
                        outPfd.fd,
                        seedDbPath,
                        tmpDir,
                        originalName,
                        format.useNcsdRebuild,
                        cb,
                    )
                    if (cont.isActive) cont.resume(code)
                }, "cia3ds-engine")
                t.isDaemon = true
                t.start()
            }
            when (rc) {
                0 -> DecryptResult.Success
                10 -> DecryptResult.AlreadyDecrypted
                else -> DecryptResult.Failure("Engine error code $rc")
            }
        } catch (t: Throwable) {
            Log.e("Cia3ds", "Native decrypt threw", t)
            DecryptResult.Failure(t.message ?: "Unknown native error")
        } finally {
            try { inPfd.close() } catch (_: Throwable) {}
            try { outPfd.close() } catch (_: Throwable) {}
        }
    }

    fun decryptAsFlow(
        input: Uri,
        output: Uri,
        format: OutputFormat,
        originalName: String,
    ): Flow<DecryptUpdate> = callbackFlow {
        val result = decrypt(input, output, format, originalName) { pct, msg ->
            trySend(DecryptUpdate.Progress(pct, msg))
        }
        send(DecryptUpdate.Finished(result))
        close()
    }.flowOn(Dispatchers.IO)

    private fun seedDbFile(): File =
        appCtx.cacheDir.resolve("seeddb.bin")

    /**
     * Copy the bundled assets/seeddb.bin to cacheDir on first use. Doing this
     * each app start would be wasteful for a 60 KB file but is harmless if
     * we add a length check.
     */
    private fun ensureSeedDb() {
        val target = seedDbFile()
        val asset = appCtx.assets.open(SEEDDB_ASSET).use { it.readBytes() }
        if (target.exists() && target.length() == asset.size.toLong()) return
        target.writeBytes(asset)
    }

    companion object {
        private const val SEEDDB_ASSET = "seeddb.bin"

        @Volatile private var instance: Cia3ds? = null

        fun get(context: Context): Cia3ds =
            instance ?: synchronized(this) {
                instance ?: Cia3ds(context.applicationContext).also { instance = it }
            }
    }
}

/** Callable from native code via JNI; do not rename without updating ProGuard. */
fun interface NativeProgressCallback {
    fun onProgress(percent: Int, message: String)
}

sealed interface DecryptResult {
    data object Success : DecryptResult
    data object AlreadyDecrypted : DecryptResult
    data class Failure(val message: String) : DecryptResult
}

sealed interface DecryptUpdate {
    data class Progress(val percent: Int, val message: String) : DecryptUpdate
    data class Finished(val result: DecryptResult) : DecryptUpdate
}

/**
 * What to write to the user-chosen output Uri after decryption.
 *
 * Cci and ThreeDs produce byte-identical NCSD output via makerom -ciatocci;
 * they only differ in the file extension we suggest to the user. CIA stays
 * a CIA — no NCSD repack — and is the safest pick for emulator install.
 */
enum class OutputFormat(val extension: String, val useNcsdRebuild: Boolean) {
    Cia("cia", false),
    Cci("cci", true),
    ThreeDs("3ds", true),
}
