package io.github.cia3ds.ui

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import io.github.cia3ds.BuildConfig
import io.github.cia3ds.R
import io.github.cia3ds.jni.Cia3ds

@Composable
fun AboutScreen() {
    val ctx = LocalContext.current
    val engineVersion = remember { runCatching { Cia3ds.get(ctx).version() }.getOrDefault("?") }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp),
    ) {
        Text(stringResource(R.string.about_title), style = MaterialTheme.typography.headlineSmall)
        Text(stringResource(R.string.about_version, BuildConfig.VERSION_NAME))
        Text("Engine: $engineVersion", style = MaterialTheme.typography.bodySmall)
        Text(stringResource(R.string.about_blurb))
        Text(stringResource(R.string.about_attribution), style = MaterialTheme.typography.bodySmall)
        Text(stringResource(R.string.about_license), style = MaterialTheme.typography.bodySmall)
    }
}
