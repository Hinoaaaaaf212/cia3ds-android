	/**
	 * @file RsaOaepPadding.h
	 * @brief Declaration of tc::crypto::detail::RsaOaepPadding
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2020/09/12
	 **/
#pragma once
#include <tc/types.h>
#include <tc/ByteData.h>

namespace tc { namespace crypto { namespace detail {

	/**
	 * @class RsaOaepPadding
	 * @brief This class implements RSA OAEP Padding as a template class.
	 * 
	 * @tparam HashFunction The class that implements the hash function used for padding generation.
	 */
template <typename HashFunction>
class RsaOaepPadding
{
public:
	static const size_t kHashSize = HashFunction::kHashSize;

	enum class Result
	{
		kSuccess,
		kBadSeedSize,
		kBadLabelDigestSize,
		kBlockSizeTooSmall,
		kBadPadding,
		kOutputBufferTooSmall
	};

	RsaOaepPadding::Result BuildPad(byte_t* out_block, size_t block_size, const byte_t* label_digest, size_t label_digest_size, const byte_t* raw_message, size_t raw_message_size, const byte_t* seed, size_t seed_size)
	{
		if (seed_size != kHashSize) { return Result::kBadSeedSize; }
		if (label_digest_size != kHashSize) { return Result::kBadLabelDigestSize; }
		if (block_size < (1 + seed_size + label_digest_size + 1 + raw_message_size)) { return Result::kBlockSizeTooSmall; }

		size_t seed_offset = 0x01;
		size_t label_digest_offset = seed_offset + seed_size;
		size_t padding_offset = label_digest_offset + label_digest_size;
		size_t padding_size = block_size - (1 + seed_size + label_digest_size + 1 + raw_message_size);
		size_t msg_offset = padding_offset + padding_size + 0x01;

		out_block[0] = 0x00;
		memcpy(out_block + seed_offset, seed, seed_size);
		memcpy(out_block + label_digest_offset, label_digest, label_digest_size);
		memset(out_block + padding_offset, 00, padding_size);
		out_block[padding_offset + padding_size] = 0x01;
		memcpy(out_block + msg_offset, raw_message, raw_message_size);

		// apply mask
		apply_mgf1_mask<kHashSize>(out_block + label_digest_offset, block_size - label_digest_offset, out_block + seed_offset, seed_size);
		apply_mgf1_mask<kHashSize>(out_block + seed_offset, seed_size, out_block + label_digest_offset, block_size - label_digest_offset);

		return Result::kSuccess;
	}

	RsaOaepPadding::Result RecoverFromPad(byte_t* out_message, size_t out_size, size_t& message_size, const byte_t* label_digest, size_t label_digest_size, byte_t* block, size_t block_size)
	{
		size_t seed_size = kHashSize;

		if (out_size == 0) { return Result::kOutputBufferTooSmall; }
		if (label_digest_size != kHashSize) { return Result::kBadLabelDigestSize; }
		if (block_size < (1 + seed_size + label_digest_size + 1 + 1)) { return Result::kBlockSizeTooSmall; }

		size_t seed_offset = 0x01;
		size_t label_digest_offset = seed_offset + seed_size;
		size_t padding_offset = label_digest_offset + label_digest_size;
		size_t padding_size = 0; // set later
		size_t msg_offset = 0;// set later
		size_t msg_size = 0;// set later

		// constant time check
		byte_t bad = 0;

		// check byte 0
		bad |= block[0] != 0x00;

		// apply mask
		apply_mgf1_mask<kHashSize>(block + seed_offset, seed_size, block + label_digest_offset, block_size - label_digest_offset);
		apply_mgf1_mask<kHashSize>(block + label_digest_offset, block_size - label_digest_offset, block + seed_offset, seed_size);

		// check label
		for (size_t i = 0; i < label_digest_size; i++)
			bad |= block[label_digest_offset + i] ^ label_digest[i];

		// seek message begin {0x00, ..., 0x01, message} 
		bool is0x01MarkerLocated = false;
		for (size_t i = 0, size = block_size - padding_offset; i < size && is0x01MarkerLocated == false; i++)
		{
			// padding byte that should prefix the start marker
			if (block[padding_offset + i] == 0x00)
			{
				continue;
			}
			// if the byte is the start marker then set other offsets/sizes and note the marker was located
			else if (block[padding_offset + i] == 0x01)
			{
				padding_size = i;
				msg_offset = padding_offset + padding_size + 0x01;
				msg_size = block_size - msg_offset;
				is0x01MarkerLocated = true;
			}
			// otherwise this is unexpected data
			else
			{
				bad |= 1;
				break;
			}
			
		}

		// throw error if bad
		if (is0x01MarkerLocated == false || bad != 0)
		{
			return Result::kBadPadding;
		}

		// throw error if out_size isn't large enough
		if (out_size < msg_size)
		{
			return Result::kOutputBufferTooSmall;
		}

		// export message
		memcpy(out_message, &block[msg_offset], msg_size);
		message_size = msg_size;

		return Result::kSuccess;
	}

private:
	template <size_t HashSize>
	inline void apply_mgf1_mask(byte_t* dst, size_t dst_size, const byte_t* src, size_t src_size)
	{
		HashFunction hash;
		std::array<byte_t, HashSize> mask;
		tc::bn::be32<uint32_t> beRoundNum;

		for (size_t round_idx = 0, round_num = (dst_size + HashSize - 1) / HashSize; round_idx < round_num; round_idx++)
		{
			hash.initialize();

			// update using src data
			hash.update(src, src_size);
			
			// update using big endian round num
			beRoundNum.wrap((uint32_t)round_idx);
			hash.update((byte_t*)&beRoundNum, sizeof(tc::bn::be32<uint32_t>));

			// get mask
			hash.getHash(mask.data());

			// merge mask and dst
			size_t dst_pos = round_idx * HashSize;

			for (size_t i = 0, len = std::min(dst_size - dst_pos, HashSize); i < len; i++)
			{
				dst[dst_pos + i] ^= mask[i];
			}
		}
	}
};

}}} // namespace tc::crypto::detail