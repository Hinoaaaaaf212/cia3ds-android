package io.github.cia3ds.ui

import android.content.Intent
import android.net.Uri
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
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.FilterChip
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import androidx.documentfile.provider.DocumentFile
import io.github.cia3ds.R
import io.github.cia3ds.jni.Cia3ds
import io.github.cia3ds.jni.DecryptResult
import io.github.cia3ds.jni.DecryptUpdate
import io.github.cia3ds.jni.OutputFormat
import kotlinx.coroutines.flow.collectLatest
import kotlinx.coroutines.launch

private fun OutputFormat.descriptionRes(): Int = when (this) {
    OutputFormat.Cia -> R.string.format_desc_cia
    OutputFormat.Cci -> R.string.format_desc_cci
    OutputFormat.ThreeDs -> R.string.format_desc_3ds
}

@Composable
fun SingleScreen() {
    val ctx = LocalContext.current
    val scope = rememberCoroutineScope()

    var input by remember { mutableStateOf<Uri?>(null) }
    var inputName by remember { mutableStateOf<String?>(null) }
    var output by remember { mutableStateOf<Uri?>(null) }
    var format by remember { mutableStateOf(OutputFormat.Cia) }
    var status by remember { mutableStateOf<String>("") }
    var percent by remember { mutableStateOf(0) }
    var isRunning by remember { mutableStateOf(false) }
    var lastResult by remember { mutableStateOf<DecryptResult?>(null) }
    val logLines = remember { mutableStateListOf<String>() }

    val pickInput = rememberLauncherForActivityResult(
        ActivityResultContracts.OpenDocument()
    ) { uri ->
        Log.i(TAG, "OpenDocument result: $uri")
        if (uri != null) {
            runCatching {
                ctx.contentResolver.takePersistableUriPermission(
                    uri, Intent.FLAG_GRANT_READ_URI_PERMISSION,
                )
            }.onFailure { Log.w(TAG, "takePersistableUriPermission(read) failed: ${it.message}") }
            input = uri
            inputName = DocumentFile.fromSingleUri(ctx, uri)?.name
            output = null
            lastResult = null
            status = ""
            percent = 0
            logLines.clear()
        }
    }

    fun launchDecrypt(inUri: Uri, outUri: Uri) {
        Log.i(TAG, "decrypt: input=$inUri output=$outUri format=$format")
        isRunning = true
        percent = 0
        status = ""
        lastResult = null
        logLines.clear()
        logLines += "tap Decrypt: input=${inputName ?: "?"} format=${format.extension}"
        scope.launch {
            val engine = runCatching { Cia3ds.get(ctx) }
                .onFailure { t ->
                    Log.e(TAG, "Cia3ds.get failed", t)
                    logLines += "ERR: failed to load native engine: ${t.message}"
                    isRunning = false
                    lastResult = DecryptResult.Failure(t.message ?: "load failed")
                    status = ctx.getString(R.string.error_generic, lastResult.toString())
                }
                .getOrNull() ?: return@launch
            try {
                engine.decryptAsFlow(
                    input = inUri,
                    output = outUri,
                    format = format,
                    originalName = inputName ?: "input",
                ).collectLatest { upd ->
                    when (upd) {
                        is DecryptUpdate.Progress -> {
                            percent = upd.percent
                            status = upd.message
                        }
                        is DecryptUpdate.Log -> {
                            logLines += upd.line
                            if (logLines.size > MAX_LOG_LINES) {
                                logLines.removeAt(0)
                            }
                        }
                        is DecryptUpdate.Finished -> {
                            isRunning = false
                            lastResult = upd.result
                            Log.i(TAG, "decrypt finished: ${upd.result}")
                            status = when (val r = upd.result) {
                                is DecryptResult.Success -> ctx.getString(R.string.single_done)
                                is DecryptResult.AlreadyDecrypted -> ctx.getString(R.string.error_already_decrypted)
                                is DecryptResult.Failure -> ctx.getString(R.string.error_generic, r.message)
                            }
                        }
                    }
                }
            } catch (t: Throwable) {
                Log.e(TAG, "decrypt flow threw", t)
                logLines += "ERR: ${t.javaClass.simpleName}: ${t.message}"
                t.stackTrace.take(8).forEach { logLines += "  at $it" }
                isRunning = false
                lastResult = DecryptResult.Failure(t.message ?: t.javaClass.simpleName)
                status = ctx.getString(R.string.error_generic, lastResult.toString())
            }
        }
    }

    val pickOutput = rememberLauncherForActivityResult(
        ActivityResultContracts.CreateDocument("application/octet-stream")
    ) { uri ->
        Log.i(TAG, "CreateDocument result: $uri")
        if (uri != null) {
            runCatching {
                ctx.contentResolver.takePersistableUriPermission(
                    uri, Intent.FLAG_GRANT_WRITE_URI_PERMISSION,
                )
            }.onFailure { Log.w(TAG, "takePersistableUriPermission(write) failed: ${it.message}") }
            output = uri
            input?.let { inUri -> launchDecrypt(inUri, uri) }
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
                    .padding(12.dp),
                verticalArrangement = Arrangement.spacedBy(8.dp),
            ) {
                Button(
                    onClick = { pickInput.launch(arrayOf("application/octet-stream", "*/*")) },
                    modifier = Modifier.fillMaxWidth(),
                ) {
                    Text(inputName ?: stringResource(R.string.single_pick))
                }
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
                Button(
                    enabled = input != null && !isRunning,
                    modifier = Modifier.fillMaxWidth(),
                    onClick = {
                        val baseName = inputName?.substringBeforeLast('.') ?: "decrypted"
                        Log.i(TAG, "save-as: launching CreateDocument for $baseName.${format.extension}")
                        pickOutput.launch("$baseName-decrypted.${format.extension}")
                    },
                ) { Text(stringResource(R.string.single_save_as)) }

                if (lastResult is DecryptResult.Success && output != null) {
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

                Box(modifier = Modifier.weight(1f).fillMaxWidth())
            }
        }

        Column(
            modifier = Modifier
                .weight(1.4f)
                .fillMaxHeight(),
            verticalArrangement = Arrangement.spacedBy(8.dp),
        ) {
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

private const val MAX_LOG_LINES = 2000
private const val TAG = "cia3ds-ui"
