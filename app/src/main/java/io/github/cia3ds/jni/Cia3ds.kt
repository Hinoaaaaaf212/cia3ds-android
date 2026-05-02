package io.github.cia3ds.jni

import android.content.Context
import android.net.Uri
import android.util.Log
import io.github.cia3ds.seed.SeedFetcher
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.callbackFlow
import kotlinx.coroutines.flow.flowOn
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.suspendCancellableCoroutine
import kotlinx.coroutines.withContext
import java.io.File
import kotlin.coroutines.resume

class Cia3ds private constructor(private val appCtx: Context) {

    init {
        System.loadLibrary("cia3ds")
    }

    /** Returns 0 on success, 10 if already-decrypted (original bytes copied to output), non-zero on failure. */
    private external fun nativeDecryptCia(
        inFd: Int,
        outFd: Int,
        seedDbPath: String,
        tmpDir: String,
        originalName: String,
        wantCci: Boolean,
        progress: NativeProgressCallback?,
        log: NativeLogCallback?,
        seedFetcher: NativeSeedFetcherCallback?,
    ): Int

    private external fun nativeVersion(): String

    fun version(): String = nativeVersion()

    suspend fun decrypt(
        input: Uri,
        output: Uri,
        format: OutputFormat,
        originalName: String,
        progressEmit: (Int, String) -> Unit,
        logEmit: (String) -> Unit = {},
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
            val rc = suspendCancellableCoroutine<Int> { cont ->
                val t = Thread({
                    val pcb = NativeProgressCallback { pct, msg -> progressEmit(pct, msg) }
                    val lcb = NativeLogCallback { line -> logEmit(line) }
                    val fetcher = SeedFetcher(appCtx)
                    val scb = NativeSeedFetcherCallback { tid ->
                        // Native side calls us synchronously; we bridge to the
                        // suspending fetcher with runBlocking on this engine thread.
                        runBlocking { fetcher.fetch(tid) { line -> logEmit(line) } }
                    }
                    val code = nativeDecryptCia(
                        inPfd.fd,
                        outPfd.fd,
                        seedDbPath,
                        tmpDir,
                        originalName,
                        format.useNcsdRebuild,
                        pcb,
                        lcb,
                        scb,
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
        val result = decrypt(
            input = input,
            output = output,
            format = format,
            originalName = originalName,
            progressEmit = { pct, msg -> trySend(DecryptUpdate.Progress(pct, msg)) },
            logEmit = { line -> trySend(DecryptUpdate.Log(line)) },
        )
        send(DecryptUpdate.Finished(result))
        close()
    }.flowOn(Dispatchers.IO)

    private fun seedDbFile(): File =
        appCtx.cacheDir.resolve("seeddb.bin")

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

fun interface NativeProgressCallback {
    fun onProgress(percent: Int, message: String)
}

fun interface NativeLogCallback {
    fun onLine(line: String)
}

fun interface NativeSeedFetcherCallback {
    fun onFetch(titleIdHex: String): ByteArray?
}

sealed interface DecryptResult {
    data object Success : DecryptResult
    data object AlreadyDecrypted : DecryptResult
    data class Failure(val message: String) : DecryptResult
}

sealed interface DecryptUpdate {
    data class Progress(val percent: Int, val message: String) : DecryptUpdate
    data class Log(val line: String) : DecryptUpdate
    data class Finished(val result: DecryptResult) : DecryptUpdate
}

enum class OutputFormat(val extension: String, val useNcsdRebuild: Boolean) {
    Cia("cia", false),
    Cci("cci", true),
    ThreeDs("3ds", true),
}
