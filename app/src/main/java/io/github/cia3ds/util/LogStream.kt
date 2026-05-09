package io.github.cia3ds.util

import android.content.Context
import java.io.File
import java.io.FileWriter
import java.io.PrintWriter

object LogStream {
    @Volatile private var writer: PrintWriter? = null
    @Volatile private var currentFile: File? = null

    fun file(ctx: Context): File {
        val dir = ctx.cacheDir.resolve("decrypt-logs").apply { mkdirs() }
        return dir.resolve("current.log")
    }

    @Synchronized
    fun start(ctx: Context, append: Boolean = false) {
        runCatching { writer?.flush(); writer?.close() }
        val f = file(ctx)
        currentFile = f
        runCatching { f.parentFile?.mkdirs() }
        writer = runCatching { PrintWriter(FileWriter(f, append), false) }.getOrNull()
    }

    @Synchronized
    fun append(line: String) {
        val w = writer ?: return
        runCatching {
            w.println(line)
            w.flush()
        }
    }

    @Synchronized
    fun stop() {
        runCatching { writer?.flush(); writer?.close() }
        writer = null
    }
}
