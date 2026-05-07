package io.github.cia3ds.ui

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Info
import androidx.compose.material.icons.filled.InsertDriveFile
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.material3.adaptive.navigationsuite.NavigationSuiteScaffold
import androidx.compose.runtime.Composable
import androidx.compose.runtime.CompositionLocalProvider
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.compositionLocalOf
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.core.splashscreen.SplashScreen.Companion.installSplashScreen
import io.github.cia3ds.R
import io.github.cia3ds.prefs.rememberOutputTemplateState

val LocalIncomingUri = compositionLocalOf<MutableState<Uri?>> {
    error("LocalIncomingUri not provided")
}

val LocalOutputTemplate = compositionLocalOf<MutableState<String>> {
    error("LocalOutputTemplate not provided")
}

class MainActivity : ComponentActivity() {

    private var incomingUriState: MutableState<Uri?>? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        installSplashScreen()
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        val initial = extractUri(intent)
        setContent {
            val incoming = remember { mutableStateOf(initial) }
            incomingUriState = incoming
            Cia3dsApp(incoming)
        }
    }

    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
        setIntent(intent)
        extractUri(intent)?.let { uri ->
            incomingUriState?.value = uri
        }
    }

    private fun extractUri(intent: Intent?): Uri? {
        if (intent == null) return null
        return when (intent.action) {
            Intent.ACTION_VIEW -> intent.data
            else -> null
        }
    }
}

@Composable
fun Cia3dsApp(incomingUri: MutableState<Uri?>) {
    val themePref = rememberThemePrefState()
    val outputTemplate = rememberOutputTemplateState()
    CompositionLocalProvider(
        LocalThemePref provides themePref,
        LocalIncomingUri provides incomingUri,
        LocalOutputTemplate provides outputTemplate,
    ) {
        Cia3dsTheme(pref = themePref.value) {
            Surface(modifier = Modifier.fillMaxSize(), color = MaterialTheme.colorScheme.background) {
                var selected by rememberSaveable { mutableStateOf(NavTab.Decrypt) }
                NavigationSuiteScaffold(
                    navigationSuiteItems = {
                        NavTab.entries.forEach { tab ->
                            item(
                                selected = selected == tab,
                                onClick = { selected = tab },
                                icon = { Icon(tab.icon(), contentDescription = null) },
                                label = { Text(stringRes(tab.labelRes)) },
                            )
                        }
                    },
                ) {
                    when (selected) {
                        NavTab.Decrypt -> DecryptScreen()
                        NavTab.Settings -> SettingsScreen()
                        NavTab.About -> AboutScreen()
                    }
                }
            }
        }
    }
}

enum class NavTab(val labelRes: Int) {
    Decrypt(R.string.nav_decrypt),
    Settings(R.string.nav_settings),
    About(R.string.nav_about);

    fun icon() = when (this) {
        Decrypt -> Icons.Filled.InsertDriveFile
        Settings -> Icons.Filled.Settings
        About -> Icons.Filled.Info
    }
}

@Composable
private fun stringRes(id: Int): String = androidx.compose.ui.res.stringResource(id)
