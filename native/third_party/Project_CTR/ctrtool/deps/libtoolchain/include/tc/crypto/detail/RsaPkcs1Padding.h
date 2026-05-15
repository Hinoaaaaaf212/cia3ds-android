	/**
	 * @file RsaPkcs1Padding.h
	 * @brief Declaration of tc::crypto::detail::RsaPkcs1Padding
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2020/09/12
	 **/
#pragma once
#include <tc/types.h>
#include <tc/ByteData.h>

namespace tc { namespace crypto { namespace detail {

	/**
	 * @class RsaPkcs1Padding
	 * @brief This class implements RSA PKCS1 Padding as a template class.
	 * 
	 * @tparam HashFunction The class that implements the hash function used for padding generation.
	 */
template <typename HashFunction>
class RsaPkcs1Padding
{
public:
	static const size_t kHashSize = HashFunction::kHashSize;

	enum class Result
	{
		kSuccess,
		kBadMessageDigestSize,
		kBlockSizeTooSmall,
		kVerificationFailure
	};

	RsaPkcs1Padding::Result BuildPad(byte_t* out_block, size_t block_size, const byte_t* message_digest, size_t message_digest_size)
	{
		if (message_digest_size != kHashSize) { return Result::kBadMessageDigestSize; }

		// the minimum block size has 0 padding and the ASN1 OID data, message digest and marker bytes
		if (block_size < 2 + 1 + HashFunction::kAsn1OidData.size() + kHashSize) { return Result::kBlockSizeTooSmall; }

		// determine sizes
		size_t padding_size = block_size - 2 - 1 - HashFunction::kAsn1OidDataSize - kHashSize;

		// determine offsets
		size_t padding_offset = 0x02;
		size_t asn1oid_offset = padding_offset + padding_size + 1;
		size_t message_digest_offset = asn1oid_offset + HashFunction::kAsn1OidData.size();

		// clear block
		memset(out_block, 0, block_size);

		// write begin marker
		out_block[0] = 0x00;
		out_block[1] = 0x01;
		
		// write padding
		memset(out_block + padding_offset, 0xff, padding_size);

		// write payload marker
		out_block[padding_offset + padding_size] = 0x00;

		// write ASN.1 encoded OID
		memcpy(out_block + asn1oid_offset, HashFunction::kAsn1OidData.data(), HashFunction::kAsn1OidData.size());

		// write message digest
		memcpy(out_block + message_digest_offset, message_digest, kHashSize);

		return Result::kSuccess;
	}

	RsaPkcs1Padding::Result CheckPad(const byte_t* message_digest, size_t message_digest_size, byte_t* block, size_t block_size)
	{
		if (message_digest_size != kHashSize) { return Result::kBadMessageDigestSize; }

		// the minimum block size has 0 padding and the ASN1 OID data, message digest and marker bytes
		if (block_size < 2 + 1 + HashFunction::kAsn1OidData.size() + kHashSize) { return Result::kBlockSizeTooSmall; }

		// determine sizes
		size_t padding_size = block_size - 2 - 1 - HashFunction::kAsn1OidDataSize - kHashSize;

		// determine offsets
		size_t padding_offset = 0x02;
		size_t asn1oid_offset = padding_offset + padding_size + 1;
		size_t message_digest_offset = asn1oid_offset + HashFunction::kAsn1OidData.size();

		byte_t bad = 0;

		// validate start marker
		bad |= block[0] != 0x00;
		bad |= block[1] != 0x01;

		// validate padding
		for (size_t i = 0; i < padding_size; i++)
		{
			bad |= block[padding_offset + i] != 0xFF;
		}
		
		// validate payload marker
		bad |= block[padding_offset + padding_size] != 0x00;

		// validate ASN.1 data
		for (size_t i = 0; i < HashFunction::kAsn1OidData.size(); i++)
		{
			bad |= block[asn1oid_offset + i] != HashFunction::kAsn1OidData[i];
		}

		// validate message digest
		for (size_t i = 0; i < kHashSize; i++)
		{
			bad |= block[message_digest_offset + i] != message_digest[i];
		}

		return bad == 0? Result::kSuccess : Result::kVerificationFailure;
	}
};

}}} // namespace tc::crypto::detail