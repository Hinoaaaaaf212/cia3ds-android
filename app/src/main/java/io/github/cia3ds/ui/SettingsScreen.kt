package io.github.cia3ds.ui

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import io.github.cia3ds.R
import io.github.cia3ds.jni.OutputFormat
import io.github.cia3ds.prefs.DEFAULT_OUTPUT_TEMPLATE
import io.github.cia3ds.prefs.applyTemplate
import io.github.cia3ds.prefs.saveOutputTemplate

@Composable
fun SettingsScreen() {
    val ctx = LocalContext.current
    val templateState = LocalOutputTemplate.current
    var template by templateState
    val scroll = rememberScrollState()

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(scroll)
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp),
    ) {
        Text(
            stringResource(R.string.settings_title),
            style = MaterialTheme.typography.headlineSmall,
        )

        Card(
            modifier = Modifier.fillMaxWidth(),
            elevation = CardDefaults.cardElevation(defaultElevation = 1.dp),
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                verticalArrangement = Arrangement.spacedBy(8.dp),
            ) {
                Text(
                    stringResource(R.string.settings_output_naming_title),
                    style = MaterialTheme.typography.titleSmall,
                    fontWeight = FontWeight.SemiBold,
                )
                OutlinedTextField(
                    value = template,
                    onValueChange = {
                        template = it
                        saveOutputTemplate(ctx, it)
                    },
                    label = { Text(stringResource(R.string.settings_output_naming_template_label)) },
                    singleLine = true,
                    modifier = Modifier.fillMaxWidth(),
                )
                Text(
                    stringResource(R.string.settings_output_naming_help),
                    style = MaterialTheme.typography.bodySmall,
                    color = MaterialTheme.colorScheme.onSurfaceVariant,
                )
                val example = applyTemplate(template, "MyGame", OutputFormat.Cia)
                Text(
                    stringResource(R.string.settings_output_naming_example, example),
                    style = MaterialTheme.typography.bodySmall,
                )
                TextButton(
                    onClick = {
                        template = DEFAULT_OUTPUT_TEMPLATE
                        saveOutputTemplate(ctx, DEFAULT_OUTPUT_TEMPLATE)
                    },
                ) {
                    Text(stringResource(R.string.settings_output_naming_reset))
                }
            }
        }
    }
}
