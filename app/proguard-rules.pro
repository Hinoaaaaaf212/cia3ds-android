# Native methods called from JNI
-keepclasseswithmembernames class * {
    native <methods>;
}

# Progress callback invoked from native code
-keep class io.github.cia3ds.jni.NativeProgressCallback { *; }
-keep class io.github.cia3ds.jni.Cia3ds { *; }
-keep class io.github.cia3ds.jni.DecryptResult { *; }
