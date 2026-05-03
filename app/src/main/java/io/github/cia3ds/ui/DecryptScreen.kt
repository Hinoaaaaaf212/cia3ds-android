package io.github.cia3ds.ui

import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.net.Uri
import android.os.IBinder
import android.util.Log
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.statusBarsPadding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Close
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.FilterChip
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.LinearProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.documentfile.provider.DocumentFile
import io.github.cia3ds.R
import io.github.cia3ds.jni.ActualFormat
import io.github.cia3ds.jni.Cia3ds
import io.github.cia3ds.jni.DecryptMetadata
import io.github.cia3ds.jni.DecryptResult
import io.github.cia3ds.jni.DecryptUpdate
import io.github.cia3ds.jni.OutputFormat
import io.github.cia3ds.jni.TitleKind
import io.github.cia3ds.service.BatchItem
import io.github.cia3ds.service.BatchSource
import io.github.cia3ds.service.BatchState
import io.github.cia3ds.service.DecryptionService
import kotlinx.coroutines.CancellationException
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.flow.collectLatest
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.File
import java.util.UUID
import java.util.zip.ZipInputStream

private enum class InputMode { None, SingleFile, Zip }

@Composable
fun DecryptScreen() {
    val ctx = LocalContext.current
    val scope = rememberCoroutineScope()
    val service = rememberBoundService(ctx)

    var inputMode by remember { mutableStateOf(InputMode.None) }

    var singleFileUri by remember { mutableStateOf<Uri?>(null) }
    var singleFileName by remember { mutableStateOf<String?>(null) }
    var pickedZipUri by remember { mutableStateOf<Uri?>(null) }
    var pickedZipName by remember { mutableStateOf<String?>(null) }
    var zipEntryNames by remember { mutableStateOf<List<String>>(emptyList()) }
    var zipScanning by remember { mutableStateOf(false) }
    val zipEntryCount = zipEntryNames.size
    var output by remember { mutableStateOf<Uri?>(null) }

    var format by remember { mutableStateOf(OutputFormat.Cia) }
    var zipGrouped by remember { mutableStateOf(false) }

    var isRunning by remember { mutableStateOf(false) }
    var runningJob by remember { mutableStateOf<Job?>(null) }
    var percent by remember { mutableStateOf(0) }
    var status by remember { mutableStateOf("") }
    var lastResult by remember { mutableStateOf<DecryptResult?>(null) }
    var metadata by remember { mutableStateOf(DecryptMetadata()) }
    val logLines = remember { mutableStateListOf<String>() }
    var pendingTempFile by remember { mutableStateOf<File?>(null) }

    val batchState = service?.state?.collectAsState()

    val isBatch = when (inputMode) {
        InputMode.Zip -> zipEntryCount > 1
        else -> false
    }

    fun resetRuntime() {
        isRunning = false
        runningJob = null
        percent = 0
        status = ""
        lastResult = null
        metadata = DecryptMetadata()
        logLines.clear()
        output = null
        pendingTempFile?.let { runCatching { it.delete() } }
        pendingTempFile = null
    }

    fun resetForNewInput() {
        singleFileUri = null
        singleFileName = null
        pickedZipUri = null
        pickedZipName = null
        zipEntryNames = emptyList()
        zipScanning = false
        resetRuntime()
    }

    var singleFileError by remember { mutableStateOf<String?>(null) }

    val pickSingleFile = rememberLauncherForActivityResult(
        ActivityResultContracts.OpenDocument()
    ) { uri ->
        if (uri != null) {
            val name = DocumentFile.fromSingleUri(ctx, uri)?.name
            val lower = name?.lowercase() ?: ""
            if (lower.endsWith(".zip")) {
                singleFileError = ctx.getString(R.string.single_pick_rejected_zip)
                return@rememberLauncherForActivityResult
            }
            if (!lower.endsWith(".cia") && !lower.endsWith(".3ds")) {
                singleFileError = ctx.getString(R.string.single_pick_rejected_other)
                return@rememberLauncherForActivityResult
            }
            runCatching {
                ctx.contentResolver.takePersistableUriPermission(
                    uri, Intent.FLAG_GRANT_READ_URI_PERMISSION,
                )
            }
            resetForNewInput()
            singleFileError = null
            inputMode = InputMode.SingleFile
            singleFileUri = uri
            singleFileName = name
        }
    }

    val pickZipFile = rememberLauncherForActivityResult(
        ActivityResultContracts.OpenDocument()
    ) { uri ->
        if (uri != null) {
            runCatching {
                ctx.contentResolver.takePersistableUriPermission(
                    uri, Intent.FLAG_GRANT_READ_URI_PERMISSION,
                )
            }
            resetForNewInput()
            inputMode = InputMode.Zip
            pickedZipUri = uri
            pickedZipName = DocumentFile.fromSingleUri(ctx, uri)?.name
            zipScanning = true
            scope.launch {
                val names = withContext(Dispatchers.IO) { listZipEntries(ctx, uri) }
                zipEntryNames = names
                zipScanning = false
            }
        }
    }

    fun launchSingleDecrypt(inUri: Uri, outUri: Uri, displayName: String, tempFile: File?) {
        Log.i(TAG, "decrypt: input=$inUri output=$outUri format=$format")
        isRunning = true
        percent = 0
        status = ""
        lastResult = null
        metadata = DecryptMetadata()
        logLines.clear()
        logLines += "tap Decrypt: input=$displayName format=${format.extension}"
        pendingTempFile = tempFile
        runningJob = scope.launch {
            val engine = runCatching { Cia3ds.get(ctx) }
                .onFailure { t ->
                    Log.e(TAG, "Cia3ds.get failed", t)
                    logLines += "ERR: failed to load native engine: ${t.message}"
                    isRunning = false
                    lastResult = DecryptResult.Failure(t.message ?: "load failed")
                    status = ctx.getString(R.string.error_generic, lastResult.toString())
                    runningJob = null
                    pendingTempFile?.let { runCatching { it.delete() } }
                    pendingTempFile = null
                }
                .getOrNull() ?: return@launch
            try {
                engine.decryptAsFlow(
                    input = inUri,
                    output = outUri,
                    format = format,
                    originalName = displayName,
                ).collectLatest { upd ->
                    when (upd) {
                        is DecryptUpdate.Progress -> {
                            percent = upd.percent
                            status = upd.message
                        }
                        is DecryptUpdate.Log -> {
                            logLines += upd.line
                            if (logLines.size > MAX_LOG_LINES) logLines.removeAt(0)
                        }
                        is DecryptUpdate.Metadata -> { metadata = upd.metadata }
                        is DecryptUpdate.Finished -> {
                            isRunning = false
                            lastResult = upd.result
                            runningJob = null
                            status = when (val r = upd.result) {
                                is DecryptResult.Success -> ctx.getString(R.string.single_done)
                                is DecryptResult.AlreadyDecrypted -> ctx.getString(R.string.error_already_decrypted)
                                is DecryptResult.Failure -> ctx.getString(R.string.error_generic, r.message)
                            }
                            pendingTempFile?.let { runCatching { it.delete() } }
                            pendingTempFile = null
                        }
                    }
                }
            } catch (_: CancellationException) {
                logLines += "WARN: cancelled by user (engine will keep running in background until current step completes)"
                isRunning = false
                runningJob = null
                lastResult = DecryptResult.Failure(ctx.getString(R.string.single_cancelled))
                status = ctx.getString(R.string.single_cancelled)
                pendingTempFile?.let { runCatching { it.delete() } }
                pendingTempFile = null
            } catch (t: Throwable) {
                Log.e(TAG, "decrypt flow threw", t)
                logLines += "ERR: ${t.javaClass.simpleName}: ${t.message}"
                isRunning = false
                runningJob = null
                lastResult = DecryptResult.Failure(t.message ?: t.javaClass.simpleName)
                status = ctx.getString(R.string.error_generic, lastResult.toString())
                pendingTempFile?.let { runCatching { it.delete() } }
                pendingTempFile = null
            }
        }
    }

    val pickOutputFile = rememberLauncherForActivityResult(
        ActivityResultContracts.CreateDocument("application/octet-stream")
    ) { uri ->
        if (uri != null) {
            runCatching {
                ctx.contentResolver.takePersistableUriPermission(
                    uri, Intent.FLAG_GRANT_WRITE_URI_PERMISSION,
                )
            }
            output = uri
            when (inputMode) {
                InputMode.SingleFile -> {
                    val inUri = singleFileUri ?: return@rememberLauncherForActivityResult
                    launchSingleDecrypt(inUri, uri, singleFileName ?: "input", null)
                }
                InputMode.Zip -> {
                    val zipUri = pickedZipUri ?: return@rememberLauncherForActivityResult
                    val entryNamesSnapshot = zipEntryNames
                    val firstEntry = entryNamesSnapshot.firstOrNull()
                        ?: return@rememberLauncherForActivityResult
                    isRunning = true
                    status = "Extracting from zip…"
                    percent = 0
                    logLines.clear()
                    logLines += "extracting $firstEntry…"
                    runningJob = scope.launch {
                        val pair = withContext(Dispatchers.IO) {
                            extractZipEntryToTemp(ctx, zipUri, firstEntry)
                        }
                        if (pair == null) {
                            logLines += "ERR: could not extract entry from zip"
                            isRunning = false
                            runningJob = null
                            lastResult = DecryptResult.Failure("zip extract failed")
                            status = ctx.getString(R.string.error_generic, "zip extract failed")
                            return@launch
                        }
                        val (extracted, displayName) = pair
                        launchSingleDecrypt(
                            Uri.fromFile(extracted),
                            uri,
                            "${pickedZipName ?: "zip"} / $displayName",
                            extracted,
                        )
                    }
                }
                else -> {}
            }
        }
    }

    val pickOutputTree = rememberLauncherForActivityResult(
        ActivityResultContracts.OpenDocumentTree()
    ) { uri ->
        if (uri != null) {
            runCatching {
                ctx.contentResolver.takePersistableUriPermission(
                    uri,
                    Intent.FLAG_GRANT_READ_URI_PERMISSION or
                        Intent.FLAG_GRANT_WRITE_URI_PERMISSION,
                )
            }
            val zipUri = pickedZipUri ?: return@rememberLauncherForActivityResult
            val zipName = pickedZipName ?: "archive.zip"
            val entryNamesSnapshot = zipEntryNames.toList()
            val capturedFormat = format
            val capturedGrouped = zipGrouped
            scope.launch {
                val items = withContext(Dispatchers.IO) {
                    val outDir = DocumentFile.fromTreeUri(ctx, uri) ?: return@withContext null
                    buildBatchItemsForZip(
                        zipUri = zipUri,
                        zipName = zipName,
                        entryNames = entryNamesSnapshot,
                        outDir = outDir,
                        format = capturedFormat,
                        zipGrouped = capturedGrouped,
                    )
                } ?: emptyList()
                if (items.isEmpty()) return@launch
                val intent = Intent(ctx, DecryptionService::class.java)
                ctx.startForegroundService(intent)
                service?.startBatch(items, capturedFormat)
            }
        }
    }

    Row(
        modifier = Modifier
            .fillMaxSize()
            .statusBarsPadding()
            .padding(start = 12.dp, end = 12.dp, top = 12.dp, bottom = 8.dp),
        horizontalArrangement = Arrangement.spacedBy(12.dp),
    ) {
        Card(
            modifier = Modifier
                .weight(1f)
                .fillMaxHeight()
                .padding(top = 8.dp),
            elevation = CardDefaults.cardElevation(defaultElevation = 2.dp),
        ) {
            Column(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(12.dp)
                    .verticalScroll(rememberScrollState()),
                verticalArrangement = Arrangement.spacedBy(8.dp),
            ) {
                if (inputMode != InputMode.Zip) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(4.dp),
                    ) {
                        Button(
                            onClick = { pickSingleFile.launch(arrayOf("application/octet-stream", "*/*")) },
                            modifier = Modifier.weight(1f),
                        ) {
                            Text(
                                text = if (inputMode == InputMode.SingleFile)
                                    singleFileName ?: stringResource(R.string.single_pick)
                                else stringResource(R.string.single_pick),
                                maxLines = 1,
                                overflow = androidx.compose.ui.text.style.TextOverflow.Ellipsis,
                            )
                        }
                        if (inputMode == InputMode.SingleFile) {
                            IconButton(onClick = { resetForNewInput(); inputMode = InputMode.None }) {
                                Icon(
                                    imageVector = Icons.Filled.Close,
                                    contentDescription = stringResource(R.string.action_clear),
                                )
                            }
                        }
                    }
                    Text(
                        stringResource(R.string.single_pick_help),
                        style = MaterialTheme.typography.bodySmall,
                        color = MaterialTheme.colorScheme.onSurfaceVariant,
                    )
                    singleFileError?.let { msg ->
                        Text(
                            msg,
                            style = MaterialTheme.typography.bodySmall,
                            color = MaterialTheme.colorScheme.error,
                        )
                    }
                }
                if (inputMode != InputMode.SingleFile) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(4.dp),
                    ) {
                        OutlinedButton(
                            onClick = {
                                pickZipFile.launch(arrayOf("application/zip", "application/x-zip-compressed", "*/*"))
                            },
                            modifier = Modifier.weight(1f),
                        ) {
                            Text(
                                text = when {
                                    inputMode != InputMode.Zip -> stringResource(R.string.batch_pick_zip)
                                    zipScanning -> "${pickedZipName ?: "?"}  (scanning…)"
                                    else -> "${pickedZipName ?: "?"}  ($zipEntryCount inside)"
                                },
                                maxLines = 1,
                                overflow = androidx.compose.ui.text.style.TextOverflow.Ellipsis,
                            )
                        }
                        if (inputMode == InputMode.Zip) {
                            IconButton(onClick = { resetForNewInput(); inputMode = InputMode.None }) {
                                Icon(
                                    imageVector = Icons.Filled.Close,
                                    contentDescription = stringResource(R.string.action_clear),
                                )
                            }
                        }
                    }
                    if (zipScanning) {
                        LinearProgressIndicator(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(top = 4.dp),
                        )
                    }
                    Text(
                        stringResource(R.string.batch_pick_zip_help),
                        style = MaterialTheme.typography.bodySmall,
                        color = MaterialTheme.colorScheme.onSurfaceVariant,
                    )
                }

                MetadataLine(metadata)

                if (inputMode != InputMode.None) {
                    Text(
                        stringResource(R.string.format_label),
                        style = MaterialTheme.typography.labelLarge,
                    )
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(8.dp),
                    ) {
                        OutputFormat.entries.forEach { f ->
                            FilterChip(
                                selected = format == f,
                                onClick = { format = f },
                                label = { Text(".${f.extension}") },
                            )
                        }
                    }

                    if (isBatch && format != OutputFormat.Cia) {
                        Text(
                            stringResource(R.string.batch_format_fallback_warning),
                            style = MaterialTheme.typography.bodySmall,
                            color = MaterialTheme.colorScheme.onSurfaceVariant,
                        )
                    }

                    if (isBatch && inputMode == InputMode.Zip) {
                        Text(stringResource(R.string.batch_zip_output_label))
                        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                            FilterChip(
                                selected = !zipGrouped,
                                onClick = { zipGrouped = false },
                                label = { Text(stringResource(R.string.batch_zip_flat)) },
                            )
                            FilterChip(
                                selected = zipGrouped,
                                onClick = { zipGrouped = true },
                                label = { Text(stringResource(R.string.batch_zip_grouped)) },
                            )
                        }
                        Text(
                            text = if (zipGrouped)
                                stringResource(R.string.batch_zip_grouped_help)
                            else stringResource(R.string.batch_zip_flat_help),
                            style = MaterialTheme.typography.bodySmall,
                            color = MaterialTheme.colorScheme.onSurfaceVariant,
                        )
                    }

                    val canStart = !isRunning &&
                        (batchState?.value !is BatchState.Running) &&
                        !zipScanning &&
                        when (inputMode) {
                            InputMode.SingleFile -> singleFileUri != null
                            InputMode.Zip -> pickedZipUri != null && zipEntryCount > 0
                            else -> false
                        }
                    Button(
                        enabled = canStart,
                        modifier = Modifier.fillMaxWidth(),
                        onClick = {
                            if (isBatch) {
                                pickOutputTree.launch(null)
                            } else {
                                val baseName = when (inputMode) {
                                    InputMode.SingleFile -> singleFileName?.substringBeforeLast('.')
                                    InputMode.Zip -> pickedZipName?.substringBeforeLast('.')
                                    else -> null
                                } ?: "decrypted"
                                pickOutputFile.launch("$baseName-decrypted.${format.extension}")
                            }
                        },
                    ) {
                        Text(
                            if (isBatch) stringResource(R.string.batch_decrypt_all)
                            else stringResource(R.string.single_save_as)
                        )
                    }

                    if (isRunning || batchState?.value is BatchState.Running) {
                        OutlinedButton(
                            modifier = Modifier.fillMaxWidth(),
                            onClick = {
                                runningJob?.cancel()
                                service?.cancel()
                            },
                        ) { Text(stringResource(R.string.single_cancel)) }
                    }

                    FormatFallbackWarning(requested = format, metadata = metadata)

                    if (!isBatch && lastResult is DecryptResult.Success && output != null) {
                        OutlinedButton(
                            modifier = Modifier.fillMaxWidth(),
                            onClick = {
                                val send = Intent(Intent.ACTION_SEND).apply {
                                    type = "application/octet-stream"
                                    putExtra(Intent.EXTRA_STREAM, output)
                                    addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
                                }
                                ctx.startActivity(Intent.createChooser(send, "Share decrypted file"))
                            },
                        ) { Text(stringResource(R.string.single_share)) }
                    }
                }

                Box(modifier = Modifier.fillMaxWidth())
            }
        }

        Column(
            modifier = Modifier
                .weight(1.4f)
                .fillMaxHeight(),
            verticalArrangement = Arrangement.spacedBy(8.dp),
        ) {
            if (isBatch) {
                BatchStateBlock(
                    state = batchState?.value,
                    onCancel = { service?.cancel() },
                )
                LogPanel(
                    lines = (batchState?.value as? BatchState.Running)?.currentLog ?: emptyList(),
                    modifier = Modifier.weight(1f),
                    onCopy = {
                        val running = batchState?.value as? BatchState.Running ?: return@LogPanel
                        val cm = ctx.getSystemService(android.content.ClipboardManager::class.java)
                        cm?.setPrimaryClip(
                            android.content.ClipData.newPlainText(
                                "cia3ds log",
                                running.currentLog.joinToString("\n"),
                            )
                        )
                    },
                )
            } else {
                if (isRunning || lastResult != null) {
                    ProgressCard(
                        percent = percent,
                        statusMessage = status.ifEmpty { "Working…" },
                        isRunning = isRunning,
                        lastResult = lastResult,
                    )
                }
                LogPanel(
                    lines = logLines,
                    modifier = Modifier.weight(1f),
                    onCopy = {
                        val cm = ctx.getSystemService(android.content.ClipboardManager::class.java)
                        cm?.setPrimaryClip(
                            android.content.ClipData.newPlainText("cia3ds log", logLines.joinToString("\n"))
                        )
                    },
                )
            }
        }
    }
}

@Composable
private fun MetadataLine(metadata: DecryptMetadata) {
    val parts = buildList {
        metadata.kind?.takeIf { it != TitleKind.Unknown }?.let { add(it.display) }
        metadata.version?.takeIf { it.isNotBlank() && it != "0" }?.let { add("v$it") }
        metadata.titleId?.takeIf { it.isNotBlank() }?.let { add(it) }
    }
    if (parts.isEmpty()) return
    Text(
        text = parts.joinToString("  ·  "),
        style = MaterialTheme.typography.bodySmall,
        color = MaterialTheme.colorScheme.onSurfaceVariant,
    )
}

@Composable
private fun FormatFallbackWarning(requested: OutputFormat, metadata: DecryptMetadata) {
    val actual = metadata.actualFormat ?: return
    val mismatch = requested.useNcsdRebuild && actual == ActualFormat.Cia
    if (!mismatch) return
    Text(
        text = stringResource(R.string.single_format_fallback, requested.extension),
        style = MaterialTheme.typography.bodySmall,
        color = MaterialTheme.colorScheme.tertiary,
    )
}

@Composable
private fun BatchStateBlock(state: BatchState?, onCancel: () -> Unit) {
    val ctx = LocalContext.current
    when (state) {
        null, BatchState.Idle -> {}
        is BatchState.Running -> {
            Text(
                stringResource(R.string.batch_overall_progress) + ": " +
                    stringResource(R.string.batch_progress, state.index + 1, state.total),
                fontWeight = FontWeight.Medium,
            )
            LinearProgressIndicator(
                progress = { (state.index.toFloat() / state.total.coerceAtLeast(1)) },
                modifier = Modifier.fillMaxWidth(),
            )
            Text(
                stringResource(R.string.batch_file_progress) + ": ${state.currentName}",
                style = MaterialTheme.typography.bodySmall,
            )
            val runningMeta = parseMetadata(state.currentLog)
            val runningSubtitle = formatSubtitle(runningMeta)
            if (runningSubtitle.isNotEmpty()) {
                Text(
                    runningSubtitle,
                    style = MaterialTheme.typography.bodySmall,
                    color = MaterialTheme.colorScheme.onSurfaceVariant,
                )
            }
            LinearProgressIndicator(
                progress = { (state.currentPercent.coerceIn(0, 100)) / 100f },
                modifier = Modifier.fillMaxWidth(),
            )
            OutlinedButton(
                onClick = onCancel,
                modifier = Modifier.fillMaxWidth(),
            ) { Text(stringResource(R.string.batch_cancel)) }
        }
        is BatchState.Done -> {
            val ok = state.results.count { it.result is DecryptResult.Success }
            val skipped = state.results.count { it.result is DecryptResult.AlreadyDecrypted }
            val failed = state.results.size - ok - skipped
            Text(
                stringResource(R.string.batch_done_summary, ok, skipped, failed),
                fontWeight = FontWeight.Medium,
            )
            state.results.forEach { r ->
                val color = when (r.result) {
                    is DecryptResult.Success -> MaterialTheme.colorScheme.onSurface
                    is DecryptResult.AlreadyDecrypted -> MaterialTheme.colorScheme.tertiary
                    is DecryptResult.Failure -> MaterialTheme.colorScheme.error
                }
                val tag = when (val rr = r.result) {
                    is DecryptResult.Success -> "ok"
                    is DecryptResult.AlreadyDecrypted -> "skipped"
                    is DecryptResult.Failure -> rr.message
                }
                Text(
                    "• ${r.name}: $tag",
                    color = color,
                    style = MaterialTheme.typography.bodySmall,
                )
                val meta = parseMetadata(r.log)
                val subtitle = formatSubtitle(meta)
                if (subtitle.isNotEmpty()) {
                    Text(
                        "    $subtitle",
                        color = MaterialTheme.colorScheme.onSurfaceVariant,
                        style = MaterialTheme.typography.bodySmall,
                    )
                }
                val mismatch = state.requestedFormat.useNcsdRebuild &&
                    meta.actualFormat == ActualFormat.Cia &&
                    r.result is DecryptResult.Success
                if (mismatch) {
                    Text(
                        "    " + stringResource(
                            R.string.single_format_fallback,
                            state.requestedFormat.extension,
                        ),
                        color = MaterialTheme.colorScheme.tertiary,
                        style = MaterialTheme.typography.bodySmall,
                    )
                }
            }
        }
    }
}

@Composable
private fun rememberBoundService(ctx: Context): DecryptionService? {
    var service by remember { mutableStateOf<DecryptionService?>(null) }
    DisposableEffect(ctx) {
        val conn = object : ServiceConnection {
            override fun onServiceConnected(name: ComponentName?, binder: IBinder?) {
                service = (binder as? DecryptionService.LocalBinder)?.service
            }
            override fun onServiceDisconnected(name: ComponentName?) { service = null }
        }
        ctx.bindService(
            Intent(ctx, DecryptionService::class.java),
            conn, Context.BIND_AUTO_CREATE,
        )
        onDispose { runCatching { ctx.unbindService(conn) } }
    }
    return service
}

private fun parseMetadata(log: List<String>): DecryptMetadata {
    var meta = DecryptMetadata()
    for (line in log) {
        if (line.trim().startsWith("META:")) meta = meta.mergedWith(line)
    }
    return meta
}

private fun formatSubtitle(meta: DecryptMetadata): String {
    val parts = buildList {
        meta.kind?.takeIf { it != TitleKind.Unknown }?.let { add(it.display) }
        meta.version?.takeIf { it.isNotBlank() && it != "0" }?.let { add("v$it") }
        meta.titleId?.takeIf { it.isNotBlank() }?.let { add(it) }
    }
    return parts.joinToString("  ·  ")
}

private fun listZipEntries(ctx: Context, uri: Uri): List<String> {
    val out = mutableListOf<String>()
    runCatching {
        ctx.contentResolver.openInputStream(uri)?.use { input ->
            ZipInputStream(input).use { zin ->
                var entry = zin.nextEntry
                while (entry != null) {
                    val n = entry.name.lowercase()
                    if (!entry.isDirectory && (n.endsWith(".cia") || n.endsWith(".3ds"))) {
                        out.add(entry.name)
                    }
                    zin.closeEntry()
                    entry = zin.nextEntry
                }
            }
        }
    }
    return out
}

private fun extractZipEntryToTemp(
    ctx: Context,
    zipUri: Uri,
    entryName: String,
): Pair<File, String>? {
    val tmpRoot = ctx.cacheDir.resolve("cia3ds-zip-tmp").apply { mkdirs() }
    val safeName = entryName.substringAfterLast('/').ifBlank { "entry.bin" }
    val tmp = File(tmpRoot, UUID.randomUUID().toString() + "_" + safeName)
    return runCatching {
        ctx.contentResolver.openInputStream(zipUri)?.use { input ->
            ZipInputStream(input).use { zin ->
                var entry = zin.nextEntry
                while (entry != null) {
                    if (!entry.isDirectory && entry.name == entryName) {
                        tmp.outputStream().use { os -> zin.copyTo(os) }
                        zin.closeEntry()
                        return@runCatching tmp to safeName
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

private fun buildBatchItemsForZip(
    zipUri: Uri,
    zipName: String,
    entryNames: List<String>,
    outDir: DocumentFile,
    format: OutputFormat,
    zipGrouped: Boolean,
): List<BatchItem> {
    val zipBaseName = zipName.substringBeforeLast('.').ifBlank { "archive" }
    val targetDir: DocumentFile = if (zipGrouped) {
        outDir.findFile(zipBaseName)?.takeIf { it.isDirectory }
            ?: outDir.createDirectory(zipBaseName)
            ?: return emptyList()
    } else {
        outDir
    }
    val out = mutableListOf<BatchItem>()
    for (entryName in entryNames) {
        val entryFileName = entryName.substringAfterLast('/')
        val baseName = entryFileName.substringBeforeLast('.')
        val outName = "$baseName-decrypted.${format.extension}"
        val outFile = targetDir.findFile(outName)?.also { it.delete() }
            .let { targetDir.createFile("application/octet-stream", outName) }
            ?: continue
        out.add(
            BatchItem(
                source = BatchSource.ZipEntry(zipUri, entryName),
                output = outFile.uri,
                displayName = "$zipName / $entryFileName",
            )
        )
    }
    return out
}

private const val MAX_LOG_LINES = 2000
private const val TAG = "cia3ds-ui"
