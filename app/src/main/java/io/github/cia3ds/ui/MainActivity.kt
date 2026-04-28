package io.github.cia3ds.ui

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Folder
import androidx.compose.material.icons.filled.InsertDriveFile
import androidx.compose.material.icons.filled.Info
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.material3.adaptive.navigationsuite.NavigationSuiteScaffold
import androidx.compose.material3.adaptive.navigationsuite.NavigationSuiteType
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.core.splashscreen.SplashScreen.Companion.installSplashScreen
import io.github.cia3ds.R

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        installSplashScreen()
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent { Cia3dsApp() }
    }
}

@Composable
fun Cia3dsApp() {
    Cia3dsTheme {
        Surface(modifier = Modifier.fillMaxSize(), color = MaterialTheme.colorScheme.background) {
            var selected by rememberSaveable { mutableStateOf(NavTab.Single) }
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
                    NavTab.Single -> SingleScreen()
                    NavTab.Batch -> BatchScreen()
                    NavTab.About -> AboutScreen()
                }
            }
        }
    }
}

enum class NavTab(val labelRes: Int) {
    Single(R.string.nav_single),
    Batch(R.string.nav_batch),
    About(R.string.nav_about);

    fun icon() = when (this) {
        Single -> Icons.Filled.InsertDriveFile
        Batch -> Icons.Filled.Folder
        About -> Icons.Filled.Info
    }
}

@Composable
private fun stringRes(id: Int): String = androidx.compose.ui.res.stringResource(id)
