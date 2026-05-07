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
                10, 11 -> DecryptResult.AlreadyDecrypted
                12 -> DecryptResult.Failure("Not a recognised CIA/3DS (already decrypted, wrong file type, or corrupt)")
                6 -> DecryptResult.Failure("Could not find any NCCH partitions in this file (malformed, truncated, or unusual layout)")
                5 -> DecryptResult.Failure("ctrtool could not extract partitions (file may be corrupt, truncated, or missing keys/seed)")
                1 -> DecryptResult.Failure("Could not create the engine's work directory (out of storage, or app cache unwritable)")
                2 -> DecryptResult.Failure("Could not stage the input file (out of storage, or source no longer reachable)")
                4 -> DecryptResult.Failure("File was already decrypted, but copying it to the chosen output failed (out of storage, or destination unwritable)")
                7 -> DecryptResult.Failure("makerom could not rebuild the CIA (out of storage, corrupt partitions, or unusual title metadata)")
                8 -> DecryptResult.Failure("Decrypted, but writing the result to your chosen output failed (out of storage, or destination unwritable)")
                9 -> DecryptResult.Failure("Could not decrypt or splice an NCCH region (file may be corrupt, truncated, or missing keys/seed)")
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
        var meta = DecryptMetadata()
        val result = decrypt(
            input = input,
            output = output,
            format = format,
            originalName = originalName,
            progressEmit = { pct, msg -> trySend(DecryptUpdate.Progress(pct, msg)) },
            logEmit = { line ->
                trySend(DecryptUpdate.Log(line))
                if (line.trim().startsWith("META:")) {
                    val updated = meta.mergedWith(line)
                    if (updated != meta) {
                        meta = updated
                        trySend(DecryptUpdate.Metadata(meta))
                    }
                }
            },
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

data class DecryptMetadata(
    val titleId: String? = null,
    val kind: TitleKind? = null,
    val version: String? = null,
    val actualFormat: ActualFormat? = null,
) {
    fun mergedWith(line: String): DecryptMetadata {
        val m = META_RE.matchEntire(line.trim()) ?: return this
        val key = m.groupValues[1]
        val value = m.groupValues[2]
        return when (key) {
            "title_id" -> copy(titleId = value.takeIf { it.isNotEmpty() })
            "kind" -> copy(kind = TitleKind.fromNative(value))
            "version" -> copy(version = value.takeIf { it.isNotEmpty() })
            "format_actual" -> copy(actualFormat = ActualFormat.fromNative(value))
            else -> this
        }
    }

    companion object {
        private val META_RE = Regex("""META:\s*([a-z_]+)=(.*)""")
    }
}

enum class TitleKind(val display: String) {
    Game("Game"),
    Demo("Demo"),
    System("System"),
    DLC("DLC"),
    Patch("Update"),
    TWL("DSiWare"),
    Unknown("Unknown");

    companion object {
        fun fromNative(s: String): TitleKind = when (s) {
            "Game" -> Game
            "Demo" -> Demo
            "System" -> System
            "DLC" -> DLC
            "Patch" -> Patch
            "TWL" -> TWL
            else -> Unknown
        }
    }
}

enum class ActualFormat {
    Cia,
    Ncsd;
    companion object {
        fun fromNative(s: String): ActualFormat? = when (s) {
            "cia" -> Cia
            "ncsd" -> Ncsd
            else -> null
        }
    }
}

sealed interface DecryptUpdate {
    data class Progress(val percent: Int, val message: String) : DecryptUpdate
    data class Log(val line: String) : DecryptUpdate
    data class Metadata(val metadata: DecryptMetadata) : DecryptUpdate
    data class Finished(val result: DecryptResult) : DecryptUpdate
}

enum class OutputFormat(val extension: String, val useNcsdRebuild: Boolean) {
    Cia("cia", false),
    Cci("cci", true),
    ThreeDs("3ds", true),
}
