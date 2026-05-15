	/**
	 * @file bitarray.h
	 * @brief Declaration of tc::bn::bitarray
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2021/02/27
	 */
#pragma once
#include <tc/types.h>

namespace tc { namespace bn {

	/**
	 * @struct bitarray
	 * @brief This struct is a literal bitarray, with configurable byte and bit endianness.
	 * 
	 * @tparam T size in bytes of the bitarray.
	 * @tparam byte_order_le Boolean, true: byte order is little endian, false: byte order is big endian.
	 * @tparam bit_order_le Boolean, true: bit order is little endian, false: bit order is big endian.
	 * 
	 * @details
	 * This struct is meant to be used when defining written-to-disk structures, like file headers.
	 */ 
template <size_t T, bool byte_order_le = true, bool bit_order_le = true>
struct bitarray
{
public:
#define __BITARRAY_BYTE_INDEX_MATH(x) (byte_order_le? (x / 8) : (T - 1 - (x / 8)))
#define __BITARRAY_BIT_INDEX_MATH(x) (bit_order_le? (1 << (x % 8)) : (1 << (7 - (x % 8))))

		/// Returns the size in bits of this bitarray
	size_t bit_size() const { return T * 8; }

		/// Sets a given bit in this bitarray
	void set(size_t bit)
	{
		bit %= (T*8);
		mArray[__BITARRAY_BYTE_INDEX_MATH(bit)] |= __BITARRAY_BIT_INDEX_MATH(bit);
	}

		/// Clears a given bit in this bitarray
	void reset(size_t bit)
	{
		bit %= (T*8);
		mArray[__BITARRAY_BYTE_INDEX_MATH(bit)] &= ~(uint8_t(__BITARRAY_BIT_INDEX_MATH(bit)));
	}

		/// Flips a given bit in this bitarray
	void flip(size_t bit)
	{
		bit %= (T*8);
		test(bit) ? reset(bit) : set(bit);
	}

		/// Checks a given bit in this bitarray
	bool test(size_t bit) const
	{
		bit %= (T*8);
		return (mArray[__BITARRAY_BYTE_INDEX_MATH(bit)] & (__BITARRAY_BIT_INDEX_MATH(bit))) != 0;
	}

#undef __BITARRAY_BYTE_INDEX_MATH
#undef __BITARRAY_BIT_INDEX_MATH

private:
	std::array<uint8_t, T> mArray;
};

}} // namespace tc::bn