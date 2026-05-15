	/**
	 * @file RsaPssPadding.h
	 * @brief Declaration of tc::crypto::detail::RsaPssPadding
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2020/09/12
	 **/
#pragma once
#include <tc/types.h>
#include <tc/ByteData.h>

namespace tc { namespace crypto { namespace detail {

	/**
	 * @class RsaPssPadding
	 * @brief This class implements RSA PSS Padding as a template class.
	 * 
	 * @tparam HashFunction The class that implements the hash function used for padding generation.
	 */
template <typename HashFunction>
class RsaPssPadding
{
public:
	static const size_t kHashSize = HashFunction::kHashSize;

	enum class Result
	{
		kSuccess,
		kBadMessageDigestSize,
		kBadSaltSize,
		kBlockSizeTooSmall,
		kBadPadding,
		kBadInputData,
		kVerificationFailure
	};

		/**
		 * @note modulus_msb is usually (for byte aligned key sizes) ((block_size << 3) - 1)
		 * @note Where (modulus_msb % 8 == 0) this fails tests. Investigation required.
		 */
	RsaPssPadding::Result BuildPad(byte_t* out_block, size_t block_size, const byte_t* message_digest, size_t message_digest_size, const byte_t* salt, size_t salt_size, size_t modulus_msb)
	{
		size_t min_salt_size = kHashSize - 2;
		size_t expected_salt_size = 0;

		// the block size is large enough to support a full sized salt (hash size)
		if (block_size >= kHashSize + kHashSize + 2)
		{
			expected_salt_size = kHashSize;
		}
		// the block size is too small for a full sized salt, but is large enough for a smaller legal sized salt
		else if (block_size >= min_salt_size + kHashSize + 2)
		{
			expected_salt_size = block_size - kHashSize - 2;
		}
		// else the block size is too small for any valid salt size
		else
		{
			return Result::kBlockSizeTooSmall;
		}

		if (message_digest_size != kHashSize) { return Result::kBadMessageDigestSize; }

		// salt_size cannot have any variance from the expected size
		if (salt_size != expected_salt_size) { return Result::kBadSaltSize; }

		// initial config
		size_t signature_size = block_size;
		size_t db_offset = 0x00;

		/* Compensate for boundary condition when applying mask */
		if (modulus_msb % 8 == 0)
		{
			db_offset++;
			signature_size--;
		}

		// determine offsets and sizes
		size_t db_size = signature_size - kHashSize - 1;
		size_t db_padding_size = db_size - salt_size - 1;

		size_t salt_offset = db_offset + db_padding_size + 1;
		size_t message_digest_offset = db_offset + db_size;

		// clear block
		memset(out_block, 0, block_size);

		// write salt start marker
		out_block[db_offset + db_padding_size] = 0x01;
		// write salt
		memcpy(out_block + salt_offset, salt, salt_size);

		// write encoded message digest
		compute_encoded_message_digest(out_block + message_digest_offset, message_digest, salt, salt_size);

		// mask db
		apply_mgf1_mask<kHashSize>(out_block + db_offset, db_size, out_block + message_digest_offset, kHashSize);

		out_block[0] &= 0xFF >> ( signature_size * 8 - modulus_msb );

		// write BC to final byte of block when complete
		out_block[block_size - 1] = 0xBC;

		return Result::kSuccess;
	}

		/**
		 * @note modulus_msb is usually (for byte aligned key sizes) ((block_size << 3) - 1)
		 * @note Where (modulus_msb % 8 == 0) this fails tests. Investigation required.
		 */
	RsaPssPadding::Result CheckPad(const byte_t* message_digest, size_t message_digest_size, byte_t* block, size_t block_size, size_t modulus_msb)
	{
		size_t min_salt_size = kHashSize - 2;
		size_t salt_size = 0;

		// the block size is large enough to support a full sized salt (hash size)
		if (block_size >= kHashSize + kHashSize + 2)
		{
			salt_size = kHashSize;
		}
		// the block size is too small for a full sized salt, but is large enought for a smaller legal sized salt
		else if (block_size >= min_salt_size + kHashSize + 2)
		{
			salt_size = block_size - kHashSize - 2;
		}
		// else the block size is too small for any valid salt size
		else
		{
			return Result::kBlockSizeTooSmall;
		}

		size_t signature_size = block_size;
		size_t db_offset = 0x00;
		
		// check byte at end of block (written when padding is completed, so this should be here)
		if (block[block_size - 1] != 0xBC) { return Result::kBadPadding; }

		/*
		 * Note: EMSA-PSS verification is over the length of N - 1 bits
		 */
		if (block[0] >> ( 8 - block_size * 8 + modulus_msb )) { return Result::kBadInputData; }

		/* Compensate for boundary condition when applying mask */
		if (modulus_msb % 8 == 0)
		{
			db_offset++;
			signature_size--;
		}

		// determine offsets and sizes
		size_t db_size = signature_size - kHashSize - 1;
		size_t db_padding_size = db_size - salt_size - 1;

		size_t salt_offset = db_offset + db_padding_size + 1;
		size_t message_digest_offset = db_offset + db_size;

		// apply mask
		apply_mgf1_mask<kHashSize>(block + db_offset, db_size, block + message_digest_offset, kHashSize);

		// mask byte0
		block[0] &= 0xFF >> ( signature_size * 8 - modulus_msb );

		// constant time check
		byte_t bad = 0;

		// validate padding seeking 01 byte, and validating the supposed salt size
		bool salt_marker_located = false;
		for (size_t i = 0, size = salt_offset; i < size && salt_marker_located == false; i++)
		{
			// padding byte that should prefix the start marker
			if (block[i] == 0x00)
			{
				continue;
			}
			// if the byte is the salt start marker then check that the salt offset is correct
			else if (block[i] == 0x01)
			{
				bad |= (i + 1) != salt_offset;
				salt_marker_located = true;
			}
			// otherwise this is unexpected data
			else
			{
				bad |= 1;
				break;
			}
			
		}

		// update bad if marker did not exist
		bad |= salt_marker_located == false;
		
		// calculate encoded hash (all these offsets should be safe as they aren't provided by the user)
		std::array<byte_t, kHashSize> encoded_digest;
		compute_encoded_message_digest(encoded_digest.data(), message_digest, block + salt_offset, salt_size);

		// check encoded hash (all these offsets should be safe as they aren't provided by the user)
		for (size_t i = 0; i < kHashSize; i++)
			bad |= block[message_digest_offset + i] ^ encoded_digest[i];

		// return success if no errors
		return bad == 0 ? Result::kSuccess : Result::kVerificationFailure;
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

	inline void compute_encoded_message_digest(byte_t* dst, const byte_t* message_digest, const byte_t* salt, size_t salt_size)
	{
		HashFunction hash;
		std::array<byte_t, 8> prime;

		// initialize hash
		hash.initialize();

		// update hash with prime
		memset(prime.data(), 0, prime.size());
		hash.update(prime.data(), prime.size());

		// update hash with original message digest
		hash.update(message_digest, kHashSize);

		// update hash with salt
		hash.update(salt, salt_size);

		// compute final hash digest
		hash.getHash(dst);
	}
};

}}} // namespace tc::crypto::detail