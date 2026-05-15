
	/**
	 * @file utf8.h
	 * @brief Declaration of UTF-8 constants and macros
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2021/01/26
	 */
#pragma once
#include <tc/types.h>

namespace tc { namespace string { namespace detail {

static const char32_t kUtf8AsciiStart = 0x00;
static const char32_t kUtf8AsciiEnd = 0x7F;
static const char32_t kUtf82ByteStart = 0x80;
static const char32_t kUtf82ByteEnd = 0x7FF;
static const char32_t kUtf83ByteStart = 0x800;
static const char32_t kUtf83ByteEnd = 0x7FFF;
static const char32_t kUtf84ByteStart = 0x8000;
static const char32_t kUtf84ByteEnd = 0x10FFFF;

static inline uint8_t make_utf8_prefix(uint8_t prefix_bits) { return ((uint8_t)(-1)) << (8 - prefix_bits); }
static inline uint8_t make_utf8_mask(uint8_t prefix_bits) { return ((uint8_t)(-1)) >> (prefix_bits + 1); }
static inline uint8_t make_utf8(uint8_t prefix_bits, uint8_t data) { return make_utf8_prefix(prefix_bits) | (data & make_utf8_mask(prefix_bits)); }
static inline uint8_t get_utf8_data(uint8_t prefix_bits, uint8_t utf8_chr) { return utf8_chr & make_utf8_mask(prefix_bits); }
static inline bool utf8_has_prefix(uint8_t prefix_bits, uint8_t utf8_chr) { return ((utf8_chr & make_utf8_prefix(prefix_bits)) == make_utf8_prefix(prefix_bits)) && ((utf8_chr & ~make_utf8_mask(prefix_bits)) == make_utf8_prefix(prefix_bits)); }
static inline uint8_t get_utf8_prefix(uint8_t utf8_chr)
{
	uint8_t prefix = 0;
	while ((utf8_chr & (1 << 7)) != 0)
	{
		utf8_chr <<= 1;
		prefix++;
	}
	return prefix;
}

}}} // namespace tc::string::detail