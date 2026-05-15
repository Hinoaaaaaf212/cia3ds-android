	/**
	 * @file binary_utils.h
	 * @brief Declaration of inlines and classes for literal bit manipulation and other low-level operations.
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/12/20
	 */
#pragma once
#include <tc/types.h>

namespace tc { namespace bn {

	/**
	 * @brief Generate struct magic 32bit number.
	 * @details This generates a little endian 32bit integer from char[4].
	 * 
	 * @param[in] magic Pointer to array of magic bytes.
	 * 
	 * @return Little endian magic uint32_t.
	 */ 
constexpr uint32_t make_struct_magic_uint32(const char magic[4])
{
	return uint32_t((uint32_t)(magic[3]) << 24 | (uint32_t)(magic[2]) << 16 | (uint32_t)(magic[1]) << 8 | (uint32_t)(magic[0]));
}

	/**
	 * @brief Generate struct magic 64bit number.
	 * @details This generates a little endian 64bit integer from char[8].
	 * 
	 * @param[in] magic Pointer to array of magic bytes.
	 * 
	 * @return Little endian magic uint64_t.
	 */ 
constexpr uint64_t make_struct_magic_uint64(const char magic[8])
{
	return uint64_t((uint64_t)(magic[7]) << 56 | (uint64_t)(magic[6]) << 48 | (uint64_t)(magic[5]) << 40 | (uint64_t)(magic[4]) << 32 | (uint64_t)(magic[3]) << 24 | (uint64_t)(magic[2]) << 16 | (uint64_t)(magic[1]) << 8 | (uint64_t)(magic[0]));
}

}} // namespace tc::bn