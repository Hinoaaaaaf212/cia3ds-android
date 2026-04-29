package io.github.cia3ds.seed

import android.content.Context
import android.util.Log
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.io.File
import java.net.HttpURLConnection
import java.net.URL
import java.util.Locale

/**
 * Fetches a 16-byte per-title seed from Nintendo's eShop CDN.
 *
 * The CDN endpoint is `https://kagiya-ctr.cdn.nintendo.net/title/0x<TID>/ext_key?country=<XX>`.
 * It returns 200 + 16 raw bytes when the title has a seed, 404 when it
 * doesn't. HTTPS is mandatory (Nintendo's certs).
 *
 * Results are cached on disk under `cacheDir/seeds/<tid>.bin` so subsequent
 * runs of the same title don't hit the network. Negative results are also
 * cached as zero-byte files to avoid re-querying titles that returned 404.
 */
class SeedFetcher(private val appCtx: Context) {

    private val seedDir: File = appCtx.cacheDir.resolve("seeds").apply { mkdirs() }

    /**
     * @return 16-byte seed if available either from cache or CDN, null on any failure
     *         (offline, 404, or HTTP/SSL error).
     */
    suspend fun fetch(titleIdHex: String, log: (String) -> Unit = {}): ByteArray? =
        withContext(Dispatchers.IO) {
            val tid = titleIdHex.lowercase(Locale.US).removePrefix("0x")
            if (tid.length != 16) {
                log("seed-fetch: invalid title id '$titleIdHex'")
                return@withContext null
            }

            val cached = File(seedDir, "$tid.bin")
            if (cached.exists()) {
                return@withContext when (cached.length()) {
                    16L -> {
                        log("seed-fetch: hit cache for $tid")
                        cached.readBytes()
                    }
                    0L -> {
                        log("seed-fetch: negative cache for $tid (no seed)")
                        null
                    }
                    else -> {
                        log("seed-fetch: corrupt cache for $tid (${cached.length()} bytes), re-fetching")
                        cached.delete()
                        null
                    }
                }
            }

            // Country preference order. Most titles have the same seed across
            // regions, but querying multiple raises the success rate slightly
            // for region-specific titles.
            for (country in COUNTRIES) {
                val tidUpper = tid.uppercase(Locale.US)
                val url = URL("https://kagiya-ctr.cdn.nintendo.net/title/0x$tidUpper/ext_key?country=$country")
                log("seed-fetch: GET $url")
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
                                    log("seed-fetch: ok ($country, 16 bytes) -> cached")
                                    return@withContext bytes
                                }
                                log("seed-fetch: bad response size ${bytes.size} from $country")
                            }
                            404 -> {
                                log("seed-fetch: 404 from $country")
                                // try next country
                            }
                            else -> log("seed-fetch: HTTP $code from $country")
                        }
                    } finally {
                        conn.disconnect()
                    }
                } catch (t: Throwable) {
                    Log.w("SeedFetcher", "fetch failed for $tid ($country)", t)
                    log("seed-fetch: ${t.javaClass.simpleName}: ${t.message}")
                }
            }

            // All countries returned 404 or errored. Cache the negative result
            // only if the failures were authoritative 404s; for transport
            // errors we'd rather retry next time. Simplest: write zero-byte
            // file so we don't keep hammering the CDN. The user can wipe
            // cacheDir to force a refetch.
            try { cached.createNewFile() } catch (_: Throwable) {}
            null
        }

    fun toHex(bytes: ByteArray): String = buildString(bytes.size * 2) {
        for (b in bytes) {
            val v = b.toInt() and 0xff
            append(HEX[v ushr 4])
            append(HEX[v and 0xf])
        }
    }

    companion object {
        private val COUNTRIES = listOf("US", "JP", "GB", "DE", "FR", "ES", "IT", "NL", "AU")
        private val HEX = "0123456789abcdef".toCharArray()
    }
}
