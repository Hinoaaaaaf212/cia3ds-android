package io.github.cia3ds.ui

import android.content.Intent
import android.net.Uri
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.LinearProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.FilterChip
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
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

    val pickInput = rememberLauncherForActivityResult(
        ActivityResultContracts.OpenDocument()
    ) { uri ->
        if (uri != null) {
            ctx.contentResolver.takePersistableUriPermission(
                uri, Intent.FLAG_GRANT_READ_URI_PERMISSION,
            )
            input = uri
            inputName = DocumentFile.fromSingleUri(ctx, uri)?.name
            output = null
            lastResult = null
            status = ""
            percent = 0
        }
    }

    val pickOutput = rememberLauncherForActivityResult(
        ActivityResultContracts.CreateDocument("application/octet-stream")
    ) { uri ->
        if (uri != null) {
            ctx.contentResolver.takePersistableUriPermission(
                uri, Intent.FLAG_GRANT_WRITE_URI_PERMISSION,
            )
            output = uri
        }
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp),
    ) {
        Text(
            stringResource(R.string.single_title),
            style = MaterialTheme.typography.headlineSmall,
        )

        Card(elevation = CardDefaults.cardElevation(defaultElevation = 2.dp)) {
            Column(modifier = Modifier.padding(16.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                Text(stringResource(R.string.single_pick))
                Button(onClick = {
                    pickInput.launch(arrayOf("application/octet-stream", "*/*"))
                }) {
                    Text(inputName ?: stringResource(R.string.single_pick))
                }
                Text(stringResource(R.string.format_label))
                Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    OutputFormat.entries.forEach { f ->
                        FilterChip(
                            selected = format == f,
                            onClick = { format = f },
                            label = { Text(".${f.extension}") },
                        )
                    }
                }
                Text(
                    stringResource(format.descriptionRes()),
                    style = MaterialTheme.typography.bodySmall,
                )
                Button(
                    enabled = input != null && !isRunning,
                    onClick = {
                        val baseName = inputName?.substringBeforeLast('.') ?: "decrypted"
                        pickOutput.launch("$baseName-decrypted.${format.extension}")
                    },
                ) { Text(stringResource(R.string.single_save_as)) }
            }
        }

        if (output != null && !isRunning && lastResult == null) {
            Button(onClick = {
                val inUri = input ?: return@Button
                val outUri = output ?: return@Button
                isRunning = true
                percent = 0
                status = ""
                scope.launch {
                    Cia3ds.get(ctx).decryptAsFlow(
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
                            is DecryptUpdate.Finished -> {
                                isRunning = false
                                lastResult = upd.result
                                status = when (val r = upd.result) {
                                    is DecryptResult.Success -> ctx.getString(R.string.single_done)
                                    is DecryptResult.AlreadyDecrypted -> ctx.getString(R.string.error_already_decrypted)
                                    is DecryptResult.Failure -> ctx.getString(R.string.error_generic, r.message)
                                }
                            }
                        }
                    }
                }
            }, modifier = Modifier.fillMaxWidth()) {
                Text(stringResource(R.string.single_decrypt))
            }
        }

        if (isRunning) {
            LinearProgressIndicator(
                progress = { (percent.coerceIn(0, 100)) / 100f },
                modifier = Modifier.fillMaxWidth(),
            )
        }

        if (status.isNotEmpty()) {
            Text(status, style = MaterialTheme.typography.bodyMedium)
        }

        if (lastResult is DecryptResult.Success && output != null) {
            OutlinedButton(onClick = {
                val send = Intent(Intent.ACTION_SEND).apply {
                    type = "application/octet-stream"
                    putExtra(Intent.EXTRA_STREAM, output)
                    addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
                }
                ctx.startActivity(Intent.createChooser(send, "Share decrypted file"))
            }) { Text(stringResource(R.string.single_share)) }
        }
    }
}
