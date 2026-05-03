package io.github.cia3ds.ui

import android.os.Build
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.dynamicDarkColorScheme
import androidx.compose.material3.dynamicLightColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext

private val LightColors = lightColorScheme(
    primary = Color(0xFF1F6FEB),
    secondary = Color(0xFF2E4F86),
    background = Color(0xFFF7F7FA),
    surface = Color(0xFFFFFFFF),
)

private val DarkColors = darkColorScheme(
    primary = Color(0xFF7AAEFF),
    secondary = Color(0xFFB8C8E0),
    background = Color(0xFF101418),
    surface = Color(0xFF1A1F25),
)

@Composable
fun Cia3dsTheme(
    pref: ThemePref = ThemePref.System,
    content: @Composable () -> Unit,
) {
    val ctx = LocalContext.current
    val isDark = when (pref) {
        ThemePref.Light -> false
        ThemePref.Dark -> true
        ThemePref.System -> isSystemInDarkTheme()
    }
    val colors = when {
        Build.VERSION.SDK_INT >= Build.VERSION_CODES.S ->
            if (isDark) dynamicDarkColorScheme(ctx) else dynamicLightColorScheme(ctx)
        isDark -> DarkColors
        else -> LightColors
    }
    MaterialTheme(colorScheme = colors, content = content)
}
