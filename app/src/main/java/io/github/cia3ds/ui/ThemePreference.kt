package io.github.cia3ds.ui

import android.content.Context
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.compositionLocalOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.platform.LocalContext

enum class ThemePref { System, Light, Dark }

private const val PREFS_NAME = "cia3ds-prefs"
private const val KEY_THEME = "theme"

private fun Context.themePrefs() = getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)

fun loadThemePref(ctx: Context): ThemePref {
    val raw = ctx.themePrefs().getString(KEY_THEME, null) ?: return ThemePref.System
    return runCatching { ThemePref.valueOf(raw) }.getOrDefault(ThemePref.System)
}

fun saveThemePref(ctx: Context, pref: ThemePref) {
    ctx.themePrefs().edit().putString(KEY_THEME, pref.name).apply()
}

val LocalThemePref = compositionLocalOf<MutableState<ThemePref>> {
    error("LocalThemePref not provided")
}

@Composable
fun rememberThemePrefState(): MutableState<ThemePref> {
    val ctx = LocalContext.current
    return remember { mutableStateOf(loadThemePref(ctx)) }
}
