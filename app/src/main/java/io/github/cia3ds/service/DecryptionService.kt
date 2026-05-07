package io.github.cia3ds.service

import android.app.Notification
import android.app.PendingIntent
import android.app.Service
import android.content.Context
import android.content.Intent
import android.content.pm.ServiceInfo
import android.net.Uri
import android.os.Build
import android.os.IBinder
import androidx.core.app.NotificationCompat
import io.github.cia3ds.R
import io.github.cia3ds.jni.Cia3ds
import io.github.cia3ds.jni.DecryptResult
import io.github.cia3ds.jni.OutputFormat
import io.github.cia3ds.ui.MainActivity
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.cancel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch

class DecryptionService : Service() {

    private val scope = CoroutineScope(SupervisorJob() + Dispatchers.IO)
    private var batchJob: Job? = null
    private val pendingTempFiles = mutableListOf<java.io.File>()

    override fun onBind(intent: Intent?): IBinder = LocalBinder()

    inner class LocalBinder : android.os.Binder() {
        val service: DecryptionService get() = this@DecryptionService
    }

    private val _state = MutableStateFlow<BatchState>(BatchState.Idle)
    val state: StateFlow<BatchState> = _state.asStateFlow()

    fun startBatch(items: List<BatchItem>, format: OutputFormat) {
        if (batchJob?.isActive == true) return
        startForegroundCompat(items.size, 0, 0)
        _state.value = BatchState.Running(0, items.size, "", emptyList(), emptyList())
        synchronized(pendingTempFiles) { pendingTempFiles.clear() }
        batchJob = scope.launch {
            val engine = Cia3ds.get(this@DecryptionService)
            val tmpRoot = cacheDir.resolve("cia3ds-zip-tmp").apply { mkdirs() }
            val results = mutableListOf<BatchResult>()
            var failed = 0
            items.forEachIndexed { index, item ->
                val currentLog = mutableListOf<String>()
                var currentPercent = 0
                fun publish() {
                    _state.value = BatchState.Running(
                        index = index,
                        total = items.size,
                        currentName = item.displayName,
                        finishedSoFar = results.toList(),
                        currentLog = currentLog.toList(),
                        currentPercent = currentPercent,
                    )
                }
                publish()
                updateNotification(items.size, index, failed)

                var tempFile: java.io.File? = null
                val r = try {
                    val inputUri: Uri = when (val src = item.source) {
                        is BatchSource.Direct -> src.uri
                        is BatchSource.ZipEntry -> {
                            currentLog += "extracting ${src.entryName} from zip…"
                            publish()
                            val tmp = extractEntryToTemp(src.zipUri, src.entryName, tmpRoot)
                            if (tmp == null) {
                                results += BatchResult(
                                    item.displayName,
                                    DecryptResult.Failure("zip extract failed"),
                                    currentLog.toList(),
                                )
                                failed += 1
                                return@forEachIndexed
                            }
                            tempFile = tmp
                            synchronized(pendingTempFiles) { pendingTempFiles.add(tmp) }
                            Uri.fromFile(tmp)
                        }
                    }
                    runCatching {
                        engine.decrypt(
                            input = inputUri,
                            output = item.output,
                            format = format,
                            originalName = item.displayName,
                            progressEmit = { pct, _ ->
                                currentPercent = pct
                                publish()
                            },
                            logEmit = { line ->
                                currentLog += line
                                if (currentLog.size > MAX_PER_FILE_LOG) currentLog.removeAt(0)
                                publish()
                            },
                        )
                    }.getOrElse { DecryptResult.Failure(it.message ?: "crash") }
                } finally {
                    tempFile?.let { tf ->
                        runCatching { tf.delete() }
                        synchronized(pendingTempFiles) { pendingTempFiles.remove(tf) }
                    }
                }
                if (r is DecryptResult.Failure) failed += 1
                results += BatchResult(item.displayName, r, currentLog.toList())
            }
            _state.value = BatchState.Done(results.toList(), format)
            stopForegroundCompat()
            postDoneNotification(results)
        }
    }

    private fun extractEntryToTemp(
        zipUri: Uri,
        entryName: String,
        tmpRoot: java.io.File,
    ): java.io.File? {
        val safeName = entryName.substringAfterLast('/').ifBlank { "entry.bin" }
        val tmp = java.io.File(tmpRoot, java.util.UUID.randomUUID().toString() + "_" + safeName)
        return runCatching {
            contentResolver.openInputStream(zipUri)?.use { input ->
                java.util.zip.ZipInputStream(input).use { zin ->
                    var entry = zin.nextEntry
                    while (entry != null) {
                        if (!entry.isDirectory && entry.name == entryName) {
                            tmp.outputStream().use { os -> zin.copyTo(os) }
                            zin.closeEntry()
                            return@runCatching tmp
                        }
                        zin.closeEntry()
                        entry = zin.nextEntry
                    }
                }
            }
            null
        }.getOrElse {
            runCatching { tmp.delete() }
            null
        }
    }

    fun cancel() {
        runCatching { Cia3ds.get(this).cancel() }
        batchJob?.cancel()
        batchJob = null
        _state.value = BatchState.Idle
        stopForegroundCompat()
        synchronized(pendingTempFiles) {
            pendingTempFiles.forEach { runCatching { it.delete() } }
            pendingTempFiles.clear()
        }
    }

    private fun startForegroundCompat(total: Int, done: Int, failed: Int) {
        startForeground(NOTIF_ID, buildNotification(total, done, failed), serviceTypeData())
    }

    private fun updateNotification(total: Int, done: Int, failed: Int) {
        val nm = androidx.core.app.NotificationManagerCompat.from(this)
        if (nm.areNotificationsEnabled()) {
            try {
                nm.notify(NOTIF_ID, buildNotification(total, done, failed))
            } catch (_: SecurityException) {
            }
        }
    }

    private fun buildNotification(total: Int, done: Int, failed: Int): Notification {
        val pi = PendingIntent.getActivity(
            this, 0,
            Intent(this, MainActivity::class.java),
            PendingIntent.FLAG_IMMUTABLE,
        )
        val text = if (failed > 0) {
            getString(R.string.notif_text_with_failures, done, total, failed)
        } else {
            getString(R.string.notif_text, done, total)
        }
        return NotificationCompat.Builder(this, getString(R.string.notif_channel_id))
            .setSmallIcon(R.drawable.ic_launcher_foreground)
            .setContentTitle(getString(R.string.notif_title))
            .setContentText(text)
            .setProgress(total, done, false)
            .setOngoing(true)
            .setContentIntent(pi)
            .build()
    }

    private fun postDoneNotification(results: List<BatchResult>) {
        val ok = results.count { it.result is DecryptResult.Success }
        val skipped = results.count { it.result is DecryptResult.AlreadyDecrypted }
        val failed = results.size - ok - skipped
        val nm = androidx.core.app.NotificationManagerCompat.from(this)
        if (!nm.areNotificationsEnabled()) return
        val pi = PendingIntent.getActivity(
            this, 0,
            Intent(this, MainActivity::class.java),
            PendingIntent.FLAG_IMMUTABLE,
        )
        val notif = NotificationCompat.Builder(this, getString(R.string.notif_channel_id))
            .setSmallIcon(R.drawable.ic_launcher_foreground)
            .setContentTitle(getString(R.string.notif_done_title))
            .setContentText(getString(R.string.notif_done_text, ok, skipped, failed))
            .setAutoCancel(true)
            .setContentIntent(pi)
            .build()
        try {
            nm.notify(NOTIF_DONE_ID, notif)
        } catch (_: SecurityException) {
        }
    }

    @Suppress("DEPRECATION")
    private fun stopForegroundCompat() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            stopForeground(STOP_FOREGROUND_REMOVE)
        } else {
            stopForeground(true)
        }
    }

    private fun serviceTypeData(): Int =
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
            ServiceInfo.FOREGROUND_SERVICE_TYPE_DATA_SYNC else 0

    override fun onDestroy() {
        scope.cancel()
        super.onDestroy()
    }

    companion object {
        private const val NOTIF_ID = 0xC1A
        private const val NOTIF_DONE_ID = 0xC1B
        fun bindIntent(ctx: Context) = Intent(ctx, DecryptionService::class.java)
    }
}

data class BatchItem(
    val source: BatchSource,
    val output: Uri,
    val displayName: String,
)

sealed interface BatchSource {
    data class Direct(val uri: Uri) : BatchSource
    data class ZipEntry(val zipUri: Uri, val entryName: String) : BatchSource
}

data class BatchResult(
    val name: String,
    val result: DecryptResult,
    val log: List<String> = emptyList(),
)

sealed interface BatchState {
    data object Idle : BatchState
    data class Running(
        val index: Int,
        val total: Int,
        val currentName: String,
        val finishedSoFar: List<BatchResult>,
        val currentLog: List<String>,
        val currentPercent: Int = 0,
    ) : BatchState
    data class Done(
        val results: List<BatchResult>,
        val requestedFormat: OutputFormat,
    ) : BatchState
}

private const val MAX_PER_FILE_LOG = 2000
