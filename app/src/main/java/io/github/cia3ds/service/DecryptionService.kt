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

/**
 * Long-running batch decryption runs here so the engine survives Activity
 * recreation (rotation, launching the SAF picker, etc.). The Service exposes
 * its progress as a [StateFlow] consumed by the UI.
 */
class DecryptionService : Service() {

    private val scope = CoroutineScope(SupervisorJob() + Dispatchers.IO)
    private var batchJob: Job? = null

    override fun onBind(intent: Intent?): IBinder = LocalBinder()

    inner class LocalBinder : android.os.Binder() {
        val service: DecryptionService get() = this@DecryptionService
    }

    private val _state = MutableStateFlow<BatchState>(BatchState.Idle)
    val state: StateFlow<BatchState> = _state.asStateFlow()

    fun startBatch(items: List<BatchItem>, wantCci: Boolean) {
        if (batchJob?.isActive == true) return
        startForegroundCompat(items.size, 0)
        _state.value = BatchState.Running(0, items.size, "", emptyList())
        batchJob = scope.launch {
            val engine = Cia3ds.get(this@DecryptionService)
            val results = mutableListOf<BatchResult>()
            items.forEachIndexed { index, item ->
                _state.value = BatchState.Running(index, items.size, item.displayName, results.toList())
                updateNotification(items.size, index)
                val r = runCatching {
                    engine.decrypt(item.input, item.output, wantCci, item.displayName) { _, _ -> }
                }.getOrElse { DecryptResult.Failure(it.message ?: "crash") }
                results += BatchResult(item.displayName, r)
            }
            _state.value = BatchState.Done(results.toList())
            stopForegroundCompat()
        }
    }

    fun cancel() {
        batchJob?.cancel()
        batchJob = null
        _state.value = BatchState.Idle
        stopForegroundCompat()
    }

    private fun startForegroundCompat(total: Int, done: Int) {
        startForeground(NOTIF_ID, buildNotification(total, done), serviceTypeData())
    }

    private fun updateNotification(total: Int, done: Int) {
        val nm = androidx.core.app.NotificationManagerCompat.from(this)
        if (nm.areNotificationsEnabled()) {
            try {
                nm.notify(NOTIF_ID, buildNotification(total, done))
            } catch (_: SecurityException) {
                // POST_NOTIFICATIONS not granted on 13+; ignore.
            }
        }
    }

    private fun buildNotification(total: Int, done: Int): Notification {
        val pi = PendingIntent.getActivity(
            this, 0,
            Intent(this, MainActivity::class.java),
            PendingIntent.FLAG_IMMUTABLE,
        )
        return NotificationCompat.Builder(this, getString(R.string.notif_channel_id))
            .setSmallIcon(R.drawable.ic_launcher_foreground)
            .setContentTitle(getString(R.string.notif_title))
            .setContentText(getString(R.string.notif_text, done, total))
            .setProgress(total, done, false)
            .setOngoing(true)
            .setContentIntent(pi)
            .build()
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
        fun bindIntent(ctx: Context) = Intent(ctx, DecryptionService::class.java)
    }
}

data class BatchItem(
    val input: Uri,
    val output: Uri,
    val displayName: String,
)

data class BatchResult(val name: String, val result: DecryptResult)

sealed interface BatchState {
    data object Idle : BatchState
    data class Running(
        val index: Int,
        val total: Int,
        val currentName: String,
        val finishedSoFar: List<BatchResult>,
    ) : BatchState
    data class Done(val results: List<BatchResult>) : BatchState
}
