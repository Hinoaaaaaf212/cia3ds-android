	/**
	 * @file BlockUtilImpl.h
	 * @brief Declaration of block utility functions for tc::crypto
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/10/04
	 **/
#pragma once
#include <tc/types.h>

namespace tc { namespace crypto { namespace detail {

template <size_t BlockSize>
inline void incr_counter(byte_t* counter, uint64_t incr)
{
	for(uint64_t i = 0; i < incr; i++) {
		for (uint32_t j = BlockSize; j > 0; j--) {
			// increment u8 by 1
			counter[j-1]++;

			// if it didn't overflow to 0, then we can exit now
			if (counter[j-1])
				break;

			// if we reach here, the next u8 needs to be incremented
			if (j == 1)
				j = BlockSize;
		}
	}
}

template <>
inline void incr_counter<16>(byte_t* counter, uint64_t incr)
{
	tc::bn::be64<uint64_t>* counter_words = (tc::bn::be64<uint64_t>*)counter;

	uint64_t carry = incr;
	for (size_t i = 0; carry != 0 ; i = ((i + 1) % 2))
	{
		uint64_t word = counter_words[1 - i].unwrap();
		uint64_t remaining = std::numeric_limits<uint64_t>::max() - word;

		if (remaining > carry)
		{
			counter_words[1 - i].wrap(word + carry);
			carry = 0;
		}
		else
		{
			counter_words[1 - i].wrap(carry - remaining - 1);
			carry = 1;
		}
	}
}

template <size_t BlockSize>
inline void xor_block(byte_t* dst, const byte_t* src_a, const byte_t* src_b)
{
	for (size_t i = 0; i < BlockSize; i++) { dst[i] = src_a[i] ^ src_b[i];}
}

template <>
inline void xor_block<16>(byte_t* dst, const byte_t* src_a, const byte_t* src_b)
{
	((uint64_t*)dst)[0] = ((uint64_t*)src_a)[0] ^ ((uint64_t*)src_b)[0];
	((uint64_t*)dst)[1] = ((uint64_t*)src_a)[1] ^ ((uint64_t*)src_b)[1];
}

}}} // namespace tc::crypto::detail