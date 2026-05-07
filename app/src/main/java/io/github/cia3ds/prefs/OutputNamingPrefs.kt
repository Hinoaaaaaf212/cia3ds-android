package io.github.cia3ds.prefs

import android.content.Context
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.platform.LocalContext
import io.github.cia3ds.jni.OutputFormat

const val DEFAULT_OUTPUT_TEMPLATE = "{name}-decrypted"

private const val PREFS_NAME = "cia3ds-prefs"
private const val KEY_TEMPLATE = "output_template"

private fun Context.outputPrefs() = getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)

fun loadOutputTemplate(ctx: Context): String =
    ctx.outputPrefs().getString(KEY_TEMPLATE, null)?.takeIf { it.isNotBlank() }
        ?: DEFAULT_OUTPUT_TEMPLATE

fun saveOutputTemplate(ctx: Context, template: String) {
    ctx.outputPrefs().edit().putString(KEY_TEMPLATE, template).apply()
}

fun applyTemplate(template: String, name: String, format: OutputFormat): String {
    val raw = template.takeIf { it.isNotBlank() } ?: DEFAULT_OUTPUT_TEMPLATE
    var body = raw
        .replace("{name}", name)
        .replace("{format}", format.extension)
    val trailing = ".${format.extension}"
    if (body.endsWith(trailing, ignoreCase = true)) {
        body = body.dropLast(trailing.length)
    }
    return "$body.${format.extension}"
}

@Composable
fun rememberOutputTemplateState(): MutableState<String> {
    val ctx = LocalContext.current
    return remember { mutableStateOf(loadOutputTemplate(ctx)) }
}
