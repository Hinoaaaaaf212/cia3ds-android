package io.github.cia3ds.ui

import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.net.Uri
import android.os.IBinder
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.FilterChip
import androidx.compose.material3.LinearProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.documentfile.provider.DocumentFile
import io.github.cia3ds.R
import io.github.cia3ds.jni.DecryptResult
import io.github.cia3ds.jni.OutputFormat
import io.github.cia3ds.service.BatchItem
import io.github.cia3ds.service.BatchState
import io.github.cia3ds.service.DecryptionService

@Composable
fun BatchScreen() {
    val ctx = LocalContext.current
    val service = rememberBoundService(ctx)

    var sourceTree by remember { mutableStateOf<Uri?>(null) }
    val files = remember { mutableStateListOf<DocumentFile>() }
    var format by remember { mutableStateOf(OutputFormat.Cia) }

    val pickFolder = rememberLauncherForActivityResult(
        ActivityResultContracts.OpenDocumentTree()
    ) { uri ->
        if (uri != null) {
            ctx.contentResolver.takePersistableUriPermission(
                uri,
                Intent.FLAG_GRANT_READ_URI_PERMISSION or
                    Intent.FLAG_GRANT_WRITE_URI_PERMISSION,
            )
            sourceTree = uri
            files.clear()
            DocumentFile.fromTreeUri(ctx, uri)?.listFiles()?.forEach { f ->
                val n = f.name?.lowercase() ?: return@forEach
                if (n.endsWith(".cia") || n.endsWith(".3ds")) {
                    files.add(f)
                }
            }
        }
    }

    val state = service?.state?.collectAsState()

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp),
    ) {
        Text(stringResource(R.string.batch_title), style = MaterialTheme.typography.headlineSmall)

        Button(onClick = { pickFolder.launch(null) }) {
            Text(stringResource(R.string.batch_pick_folder))
        }

        if (sourceTree == null) {
            Text(stringResource(R.string.batch_idle))
        } else if (files.isEmpty()) {
            Text(stringResource(R.string.batch_no_files))
        } else {
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
            Button(
                enabled = (state?.value !is BatchState.Running),
                onClick = {
                    val tree = DocumentFile.fromTreeUri(ctx, sourceTree!!) ?: return@Button
                    val outDir = tree.findFile(ctx.getString(R.string.batch_output_subfolder))
                        ?: tree.createDirectory(ctx.getString(R.string.batch_output_subfolder))
                        ?: return@Button
                    val items = files.mapNotNull { f ->
                        val baseName = f.name?.substringBeforeLast('.') ?: return@mapNotNull null
                        val outName = "$baseName-decrypted.${format.extension}"
                        val outFile = outDir.findFile(outName)?.also { it.delete() }
                            .let { outDir.createFile("application/octet-stream", outName) }
                            ?: return@mapNotNull null
                        BatchItem(
                            input = f.uri,
                            output = outFile.uri,
                            displayName = f.name ?: "input",
                        )
                    }
                    val intent = Intent(ctx, DecryptionService::class.java)
                    ctx.startForegroundService(intent)
                    service?.startBatch(items, format)
                },
                modifier = Modifier.fillMaxWidth(),
            ) {
                Text(stringResource(R.string.batch_decrypt_all))
            }

            BatchStateBlock(state?.value, total = files.size)

            LazyColumn(verticalArrangement = Arrangement.spacedBy(6.dp)) {
                items(files, key = { it.uri.toString() }) { f ->
                    Card(elevation = CardDefaults.cardElevation(1.dp)) {
                        Text(
                            f.name ?: "?",
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(12.dp),
                        )
                    }
                }
            }
        }
    }
}

@Composable
private fun BatchStateBlock(state: BatchState?, total: Int) {
    when (state) {
        null, BatchState.Idle -> {}
        is BatchState.Running -> {
            Text(
                stringResource(R.string.batch_progress, state.index + 1, state.total),
                fontWeight = FontWeight.Medium,
            )
            LinearProgressIndicator(
                progress = { (state.index.toFloat() / state.total.coerceAtLeast(1)) },
                modifier = Modifier.fillMaxWidth(),
            )
            Text(state.currentName, style = MaterialTheme.typography.bodySmall)
        }
        is BatchState.Done -> {
            val ok = state.results.count { it.result is DecryptResult.Success }
            val skipped = state.results.count { it.result is DecryptResult.AlreadyDecrypted }
            val failed = state.results.size - ok - skipped
            Text("Done: $ok ok / $skipped skipped / $failed failed", fontWeight = FontWeight.Medium)
            state.results.filter { it.result is DecryptResult.Failure }.forEach { r ->
                val msg = (r.result as DecryptResult.Failure).message
                Text("• ${r.name}: $msg", style = MaterialTheme.typography.bodySmall)
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
