package io.github.cia3ds.seed

import android.content.Context
import android.net.ConnectivityManager
import android.net.NetworkCapabilities
import android.util.Log
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.io.File
import java.net.HttpURLConnection
import java.net.URL
import java.security.KeyStore
import java.security.cert.CertificateFactory
import java.security.cert.X509Certificate
import java.util.Locale
import javax.net.ssl.HostnameVerifier
import javax.net.ssl.HttpsURLConnection
import javax.net.ssl.SSLContext
import javax.net.ssl.SSLSession
import javax.net.ssl.SSLSocketFactory
import javax.net.ssl.TrustManagerFactory
import javax.net.ssl.X509TrustManager

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

            if (!isOnline()) {
                log("seed: offline; will use bundled seeddb.bin if needed")
                return@withContext null
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
                        if (this is HttpsURLConnection) {
                            cdnSocketFactory()?.let { sslSocketFactory = it }
                            hostnameVerifier = cdnHostnameVerifier
                        }
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

    private fun isOnline(): Boolean {
        val cm = appCtx.getSystemService(Context.CONNECTIVITY_SERVICE) as? ConnectivityManager
            ?: return true
        val net = cm.activeNetwork ?: return false
        val caps = cm.getNetworkCapabilities(net) ?: return false
        return caps.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
    }

    private fun cdnSocketFactory(): SSLSocketFactory? {
        cachedFactory?.let { return it }
        return synchronized(SeedFetcher) {
            cachedFactory ?: runCatching {
                val cert = appCtx.assets.open(CDN_CERT_ASSET).use { input ->
                    CertificateFactory.getInstance("X.509")
                        .generateCertificate(input) as X509Certificate
                }
                val ks = KeyStore.getInstance(KeyStore.getDefaultType()).apply {
                    load(null, null)
                    setCertificateEntry("cdn-nintendo", cert)
                }
                val tmf = TrustManagerFactory.getInstance(
                    TrustManagerFactory.getDefaultAlgorithm()
                ).apply { init(ks) }
                val tms = tmf.trustManagers.filterIsInstance<X509TrustManager>()
                val ctx = SSLContext.getInstance("TLS").apply {
                    init(null, tms.toTypedArray(), null)
                }
                ctx.socketFactory
            }.getOrNull().also { cachedFactory = it }
        }
    }

    companion object {
        private const val CDN_CERT_ASSET = "cdn-nintendo-leaf.pem"
        private const val CDN_HOST = "kagiya-ctr.cdn.nintendo.net"
        @Volatile private var cachedFactory: SSLSocketFactory? = null

        private val cdnHostnameVerifier = HostnameVerifier { hostname, session: SSLSession ->
            if (hostname != CDN_HOST) return@HostnameVerifier false
            val peer = session.peerCertificates.firstOrNull() as? X509Certificate
                ?: return@HostnameVerifier false
            val cn = peer.subjectX500Principal.name
                .split(',').map { it.trim() }
                .firstOrNull { it.startsWith("CN=") }?.removePrefix("CN=")
            cn == "*.cdn.nintendo.net"
        }

        private val COUNTRIES = listOf("US", "JP", "GB", "DE", "FR", "ES", "IT", "NL", "AU")
    }
}
