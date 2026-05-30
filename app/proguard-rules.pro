# Native methods called from JNI
-keepclasseswithmembernames class * {
    native <methods>;
}

# Progress / log / seed callbacks invoked from native code
-keep class io.github.cia3ds.jni.NativeProgressCallback { *; }
-keep class io.github.cia3ds.jni.NativeLogCallback { *; }
-keep class io.github.cia3ds.jni.NativeSeedFetcherCallback { *; }
-keep class io.github.cia3ds.jni.Cia3ds { *; }
-keep class io.github.cia3ds.jni.DecryptResult { *; }

# PreviewResult is constructed and its fields set from native code (JNI FindClass
# + GetFieldID), so its name, no-arg ctor, and fields must survive shrinking.
-keep class io.github.cia3ds.jni.Cia3ds$PreviewResult { *; }
