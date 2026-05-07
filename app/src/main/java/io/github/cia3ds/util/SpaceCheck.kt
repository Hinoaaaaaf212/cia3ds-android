package io.github.cia3ds.util

import android.content.Context
import android.net.Uri
import android.os.ParcelFileDescriptor
import android.os.StatFs
import android.util.Log
import androidx.documentfile.provider.DocumentFile

sealed interface SpaceCheckResult {
    data object Ok : SpaceCheckResult
    data class Low(val available: Long, val needed: Long) : SpaceCheckResult
    data object Unknown : SpaceCheckResult
}

private const val TAG = "cia3ds-space"

const val SPACE_HEADROOM_MULTIPLIER = 2.5

fun checkFreeSpace(
    ctx: Context,
    inputSizeBytes: Long,
    destinationUri: Uri?,
): SpaceCheckResult {
    if (inputSizeBytes <= 0L) return SpaceCheckResult.Unknown
    val needed = (inputSizeBytes * SPACE_HEADROOM_MULTIPLIER).toLong()

    val cacheAvail = runCatching { StatFs(ctx.cacheDir.path).availableBytes }
        .onFailure { Log.w(TAG, "StatFs(cache) failed", it) }
        .getOrNull()

    val destAvail = destinationUri?.let { availableForUri(ctx, it) }

    val limiting = listOfNotNull(cacheAvail, destAvail).minOrNull()
        ?: return SpaceCheckResult.Unknown

    return if (limiting < needed) SpaceCheckResult.Low(limiting, needed)
    else SpaceCheckResult.Ok
}

private fun availableForUri(ctx: Context, uri: Uri): Long? {
    runCatching {
        val doc = if (DocumentsContractTreeId(uri)) {
            DocumentFile.fromTreeUri(ctx, uri)
        } else {
            DocumentFile.fromSingleUri(ctx, uri)
        }
        val parent = doc?.parentFile ?: doc
        val raw = parent?.uri?.path
        if (raw != null) {
            val cut = raw.indexOf(':')
            val candidate = if (cut >= 0) raw.substring(cut + 1) else raw
            val f = java.io.File(candidate)
            if (f.exists()) {
                val avail = StatFs(f.path).availableBytes
                if (avail > 0) return avail
            }
        }
    }.onFailure { Log.w(TAG, "availableForUri tree path failed", it) }

    runCatching {
        ctx.contentResolver.openFileDescriptor(uri, "r")?.use { pfd ->
            return statFsFromFd(pfd)
        }
    }.onFailure { Log.w(TAG, "availableForUri fd path failed", it) }

    return null
}

private fun DocumentsContractTreeId(uri: Uri): Boolean {
    val segments = uri.pathSegments
    return segments.size >= 2 && segments[0] == "tree"
}

private fun statFsFromFd(pfd: ParcelFileDescriptor): Long? {
    return runCatching { StatFs("/proc/self/fd/${pfd.fd}").availableBytes }
        .getOrNull()
}

fun formatBytes(bytes: Long): String {
    if (bytes <= 0) return "0 B"
    val units = arrayOf("B", "KB", "MB", "GB", "TB")
    var v = bytes.toDouble()
    var i = 0
    while (v >= 1024.0 && i < units.lastIndex) {
        v /= 1024.0
        i++
    }
    return if (i == 0) "${bytes} B" else String.format("%.1f %s", v, units[i])
}
