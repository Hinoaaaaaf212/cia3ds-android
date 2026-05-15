cmake_minimum_required(VERSION 3.22)
project(cia3ds C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT ANDROID)
    message(FATAL_ERROR "This CMakeLists is only intended for Android NDK builds.")
endif()

# Silence warnings in vendored third-party code so build logs stay readable.
add_compile_options(
    -Wno-deprecated-declarations
    -Wno-format
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-unused-function
    -Wno-sign-compare
    -Wno-implicit-function-declaration
    -Wno-incompatible-pointer-types
    -Wno-int-conversion
    -Wno-pointer-sign
    -Wno-shift-op-parentheses
    -Wno-empty-body
    -Wno-pointer-bool-conversion
    -Wno-tautological-constant-out-of-range-compare
)

add_compile_options(
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Release>:-fvisibility=hidden>
    $<$<CONFIG:Release>:-fvisibility-inlines-hidden>
    $<$<CONFIG:Release>:-ffunction-sections>
    $<$<CONFIG:Release>:-fdata-sections>
)
if(ANDROID_ABI STREQUAL "arm64-v8a")
    add_compile_options($<$<CONFIG:Release>:-march=armv8-a+crypto+aes>)
elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
    add_compile_options($<$<CONFIG:Release>:-mfpu=neon>)
endif()
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

# Android bionic does not ship libiconv. Define a tiny shim that passes UTF-8 through
# unchanged, which is sufficient for makerom's only use of iconv (filename conversion).
add_compile_definitions(CIA3DS_BUILD=1)
add_compile_definitions(CIA3DS_HW_AES=1)

add_library(cia3ds_hw_aes_static STATIC ${CMAKE_CURRENT_SOURCE_DIR}/aes_armv8.c)
target_include_directories(cia3ds_hw_aes_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(CTR_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/Project_CTR)
set(CTRTOOL_ROOT ${CTR_ROOT}/ctrtool)
set(MAKEROM_ROOT ${CTR_ROOT}/makerom)

#-----------------------------------------------------------------------------
# Vendored mbedTLS 2.x (the 3DSGuy fork shipped under ctrtool/deps/libmbedtls).
# Used by libtoolchain (ctrtool side). makerom ships its own copy of crypto
# code under makerom/src/crypto.c, so we only need this single mbedtls.
#-----------------------------------------------------------------------------
set(MBEDTLS_ROOT ${CTRTOOL_ROOT}/deps/libmbedtls)
file(GLOB MBEDTLS_SOURCES "${MBEDTLS_ROOT}/src/*.c")
add_library(mbedtls_static STATIC ${MBEDTLS_SOURCES})
target_include_directories(mbedtls_static PUBLIC ${MBEDTLS_ROOT}/include)
target_include_directories(mbedtls_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mbedtls_static PUBLIC cia3ds_hw_aes_static)

#-----------------------------------------------------------------------------
# libfmt (header + 2 .cc).
#-----------------------------------------------------------------------------
set(FMT_ROOT ${CTRTOOL_ROOT}/deps/libfmt)
add_library(fmt_static STATIC
    ${FMT_ROOT}/src/format.cc
    ${FMT_ROOT}/src/os.cc
)
target_include_directories(fmt_static PUBLIC ${FMT_ROOT}/include)

#-----------------------------------------------------------------------------
# libtoolchain (ByteData, file streams, crypto wrappers, OptionParser, etc.).
#-----------------------------------------------------------------------------
set(TC_ROOT ${CTRTOOL_ROOT}/deps/libtoolchain)
file(GLOB_RECURSE TC_SOURCES "${TC_ROOT}/src/*.cpp")
add_library(toolchain_static STATIC ${TC_SOURCES})
target_include_directories(toolchain_static PUBLIC
    ${TC_ROOT}/include
    ${FMT_ROOT}/include
    ${MBEDTLS_ROOT}/include
)
target_link_libraries(toolchain_static PUBLIC fmt_static mbedtls_static)
target_compile_definitions(toolchain_static PUBLIC
    FMT_HEADER_ONLY=0
)

#-----------------------------------------------------------------------------
# libbroadon-es (placeholder Dummy.cpp; needed to satisfy ctrtool's include).
#-----------------------------------------------------------------------------
set(BES_ROOT ${CTRTOOL_ROOT}/deps/libbroadon-es)
add_library(broadon_es_static STATIC ${BES_ROOT}/src/Dummy.cpp)
target_include_directories(broadon_es_static PUBLIC ${BES_ROOT}/include)
target_link_libraries(broadon_es_static PUBLIC toolchain_static)

#-----------------------------------------------------------------------------
# libnintendo-n3ds (CIA/NCCH/RomFs snapshot generators consumed by ctrtool).
#-----------------------------------------------------------------------------
set(N3DS_ROOT ${CTRTOOL_ROOT}/deps/libnintendo-n3ds)
file(GLOB_RECURSE N3DS_SOURCES "${N3DS_ROOT}/src/*.cpp")
add_library(nintendo_n3ds_static STATIC ${N3DS_SOURCES})
target_include_directories(nintendo_n3ds_static PUBLIC
    ${N3DS_ROOT}/include
    ${TC_ROOT}/include
    ${BES_ROOT}/include
)
target_link_libraries(nintendo_n3ds_static PUBLIC toolchain_static broadon_es_static)

#-----------------------------------------------------------------------------
# ctrtool itself, built as a static lib. We rename main() so the binary is
# callable as a function from the JNI layer.
#-----------------------------------------------------------------------------
file(GLOB CTRTOOL_SOURCES
    "${CTRTOOL_ROOT}/src/*.cpp"
    "${CTRTOOL_ROOT}/src/*.c"
)
list(REMOVE_ITEM CTRTOOL_SOURCES "${CTRTOOL_ROOT}/src/main.cpp")
add_library(ctrtool_static STATIC
    ${CTRTOOL_SOURCES}
    ${CTRTOOL_ROOT}/src/main.cpp
)
target_include_directories(ctrtool_static PUBLIC
    ${CTRTOOL_ROOT}/src
    ${TC_ROOT}/include
    ${N3DS_ROOT}/include
    ${BES_ROOT}/include
    ${FMT_ROOT}/include
)
target_link_libraries(ctrtool_static PUBLIC
    nintendo_n3ds_static
    broadon_es_static
    toolchain_static
)
# Rename main() -> ctrtool_main() at compile time so we can call it in-process.
target_compile_definitions(ctrtool_static PRIVATE main=ctrtool_main)

#-----------------------------------------------------------------------------
# makerom: vendored mbedtls (separate copy under makerom/deps/libmbedtls),
# libblz, libyaml, plus its src/. Built into a static lib with renamed main().
#-----------------------------------------------------------------------------
set(MAKEROM_MBEDTLS_ROOT ${MAKEROM_ROOT}/deps/libmbedtls)
file(GLOB MAKEROM_MBEDTLS_SOURCES "${MAKEROM_MBEDTLS_ROOT}/library/*.c")
if(NOT MAKEROM_MBEDTLS_SOURCES)
    file(GLOB MAKEROM_MBEDTLS_SOURCES "${MAKEROM_MBEDTLS_ROOT}/src/*.c")
endif()
add_library(makerom_mbedtls_static STATIC ${MAKEROM_MBEDTLS_SOURCES})
target_include_directories(makerom_mbedtls_static PUBLIC
    ${MAKEROM_MBEDTLS_ROOT}/include
)
target_include_directories(makerom_mbedtls_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(makerom_mbedtls_static PUBLIC cia3ds_hw_aes_static)

set(BLZ_ROOT ${MAKEROM_ROOT}/deps/libblz)
file(GLOB BLZ_SOURCES "${BLZ_ROOT}/src/*.c")
add_library(blz_static STATIC ${BLZ_SOURCES})
target_include_directories(blz_static PUBLIC ${BLZ_ROOT}/include)

set(YAML_ROOT ${MAKEROM_ROOT}/deps/libyaml)
file(GLOB YAML_SOURCES "${YAML_ROOT}/src/*.c")
add_library(yaml_static STATIC ${YAML_SOURCES})
target_include_directories(yaml_static PUBLIC ${YAML_ROOT}/include)
target_compile_definitions(yaml_static PRIVATE
    HAVE_CONFIG_H=0
    YAML_VERSION_MAJOR=0
    YAML_VERSION_MINOR=2
    YAML_VERSION_PATCH=5
    YAML_VERSION_STRING="0.2.5"
)

# Build makerom proper. oschar.c uses iconv(); patch it via a wrapper header
# defined in patch/iconv_shim.h that supplies a UTF-8-passthrough no-op.
file(GLOB MAKEROM_SOURCES "${MAKEROM_ROOT}/src/*.c")
list(REMOVE_ITEM MAKEROM_SOURCES "${MAKEROM_ROOT}/src/makerom.c")
add_library(makerom_static STATIC
    ${MAKEROM_SOURCES}
    ${MAKEROM_ROOT}/src/makerom.c
)
target_include_directories(makerom_static PUBLIC
    ${MAKEROM_ROOT}/src
    ${MAKEROM_MBEDTLS_ROOT}/include
    ${BLZ_ROOT}/include
    ${YAML_ROOT}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/patch
)
target_link_libraries(makerom_static PUBLIC
    makerom_mbedtls_static
    blz_static
    yaml_static
)
target_compile_definitions(makerom_static PRIVATE
    main=makerom_main
    NO_ICONV=1
)

#-----------------------------------------------------------------------------
# Our code: NCCH-flags patcher + JNI surface.
#-----------------------------------------------------------------------------
add_library(cia3ds SHARED
    jni/cia3ds_jni.cpp
    jni/metadata_sniff.cpp
    patch/ncch_flags.c
    patch/iconv_shim.c
)
target_include_directories(cia3ds PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/patch
    ${CTRTOOL_ROOT}/src
    ${MAKEROM_ROOT}/src
)
target_link_libraries(cia3ds PRIVATE
    ctrtool_static
    makerom_static
    cia3ds_hw_aes_static
    log
    android
)

# Strip release builds.
set_target_properties(cia3ds PROPERTIES
    LINK_FLAGS_RELEASE "-Wl,--gc-sections -Wl,--strip-all"
)
