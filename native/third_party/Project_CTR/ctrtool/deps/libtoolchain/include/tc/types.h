	/**
	 * @file types.h
	 * @brief Declaration of generic types used by libtoolchain
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2020/04/05
	 **/
#pragma once
#include <string>
#include <array>
#include <vector>
#include <map>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <limits>
#include <tc/bn.h>

#ifdef _WIN32
#define NOMINMAX
#endif

	/// Alias uint8_t to byte_t to more explicity indicate its role in memory related contexts
using byte_t = uint8_t;

	/**
	 * @brief Round a value up to an alignment value.
	 */
template <typename T>
inline T roundup(T value, T alignment)
{
	return value + alignment - value % alignment;
}

	/**
	 * @brief Align a value to an alignment value.
	 */
template <typename T>
inline T align(T value, T alignment)
{
	if(value % alignment != 0)
		return roundup(value,alignment);
	else
		return value;
}

namespace tc {
		/// Returns if type size_t is not 64bit.
	bool is_size_t_not_64bit();

		/// Returns if a value of type size_t is too large to be stored as int64_t.
	bool is_size_t_too_large_for_int64_t(size_t val);

		/// Returns if a value of type uint64_t is too large to be stored as int64_t.
	bool is_uint64_t_too_large_for_int64_t(uint64_t val);

		/// Returns if a value of type int64_t is too large to be stored as size_t.
	bool is_int64_t_too_large_for_size_t(int64_t val);

		/// Returns if a value of type uint64_t is too large to be stored as size_t.
	bool is_uint64_t_too_large_for_size_t(uint64_t val);
}