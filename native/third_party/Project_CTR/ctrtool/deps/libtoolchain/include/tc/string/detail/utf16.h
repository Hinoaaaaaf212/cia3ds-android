/**
 * @file utf16.h
 * @brief Declaration of UTF-16 constants and macros
 * @author Jack (jakcron)
 * @version 0.1
 * @date 2019/01/15
 */
#pragma once
#include <tc/types.h>

namespace tc { namespace string { namespace detail {

static const char32_t kUtf16EncodeMax = 0x10FFFF;
static const char32_t kUtf16NonNativeStart = 0x10000;
static const char16_t kUtf16SurrogateBits = 10;
static const char16_t kUtf16SurrogateMask = (1 << kUtf16SurrogateBits) - 1;
static const char16_t kUtf16HighSurrogateStart = 0xD800;
static const char16_t kUtf16HighSurrogateEnd = kUtf16HighSurrogateStart | kUtf16SurrogateMask;
static const char16_t kUtf16LowSurrogateStart = 0xDC00;
static const char16_t kUtf16LowSurrogateEnd = kUtf16LowSurrogateStart | kUtf16SurrogateMask;

}}} // namespace tc::string::detail