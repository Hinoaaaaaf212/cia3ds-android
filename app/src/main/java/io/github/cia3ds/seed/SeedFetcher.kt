package io.github.cia3ds.seed

import android.content.Context
import android.util.Log
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.io.File
import java.net.HttpURLConnection
import java.net.URL
import java.util.Locale

class SeedFetcher(private val appCtx: Context) {

    private val seedDir: File = appCtx.cacheDir.resolve("seeds").apply { mkdirs() }

    suspend fun fetch(titleIdHex: String, log: (String) -> Unit = {}): ByteArray? =
        withContext(Dispatchers.IO) {
            val tid = titleIdHex.lowercase(Locale.US).removePrefix("0x")
            if (tid.length != 16) {
                log("seed: invalid title id '$titleIdHex'")
                return@withContext null
            }

            val cached = File(seedDir, "$tid.bin")
            if (cached.exists()) {
                when (cached.length()) {
                    16L -> {
                        log("seed: cache hit for $tid")
                        return@withContext cached.readBytes()
                    }
                    0L -> {
                        log("seed: cache says no CDN seed for $tid")
                        return@withContext null
                    }
                    else -> {
                        log("seed: corrupt cache for $tid (${cached.length()} bytes), re-fetching")
                        cached.delete()
                    }
                }
            }

            var allCountriesReturned404 = true
            for (country in COUNTRIES) {
                val tidUpper = tid.uppercase(Locale.US)
                val url = URL("https://kagiya-ctr.cdn.nintendo.net/title/0x$tidUpper/ext_key?country=$country")
                Log.d("SeedFetcher", "GET $url")
                try {
                    val conn = (url.openConnection() as HttpURLConnection).apply {
                        connectTimeout = 5_000
                        readTimeout = 5_000
                        instanceFollowRedirects = true
                        setRequestProperty("User-Agent", "cia3ds-android")
                        requestMethod = "GET"
                    }
                    try {
                        val code = conn.responseCode
                        when (code) {
                            200 -> {
                                val bytes = conn.inputStream.use { it.readBytes() }
                                if (bytes.size == 16) {
                                    cached.writeBytes(bytes)
                                    log("seed: got per-title seed from CDN ($country)")
                                    return@withContext bytes
                                }
                                Log.w("SeedFetcher", "bad response size ${bytes.size} from $country")
                                allCountriesReturned404 = false
                            }
                            404 -> {
                                Log.d("SeedFetcher", "404 from $country")
                            }
                            else -> {
                                Log.w("SeedFetcher", "HTTP $code from $country")
                                allCountriesReturned404 = false
                            }
                        }
                    } finally {
                        conn.disconnect()
                    }
                } catch (t: Throwable) {
                    Log.w("SeedFetcher", "fetch failed for $tid ($country)", t)
                    allCountriesReturned404 = false
                }
            }

            if (allCountriesReturned404) {
                log("seed: not on CDN; will use bundled seeddb.bin if needed")
                try { cached.createNewFile() } catch (_: Throwable) {}
            } else {
                log("seed: CDN unreachable for $tid; will use bundled seeddb.bin if needed (no negative cache written)")
            }
            null
        }

    companion object {
        private val COUNTRIES = listOf("US", "JP", "GB", "DE", "FR", "ES", "IT", "NL", "AU")
    }
}
