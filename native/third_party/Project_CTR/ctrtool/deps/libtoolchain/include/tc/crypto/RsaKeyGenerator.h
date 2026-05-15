	/**
	 * @file RsaKeyGenerator.h
	 * @brief Declarations for API resources for generating RSA keys.
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/09/12
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/detail/RsaKeyGeneratorImpl.h>
#include <tc/crypto/RsaKey.h>

namespace tc { namespace crypto {

	/**
	 * @class RsaKeyGenerator
	 * @brief Class for generating RSA keys.
	 * 
	 * @details
	 * The underlying PRNG algorithm is CTR_DBRG.
	 */
class RsaKeyGenerator
{
public:
		/**
		 * @brief Default constructor.
		 */
	RsaKeyGenerator() :
		mImpl()
	{}

		/**
		 * @brief Generate an RSA key.
		 * 
		 * @param[out] key Buffer to generated RSA key.
		 * @param[in]  key_bit_size Size in bits of RSA key to generate.
		 * 
		 * @post
		 * - The generated key is written to <tt><var>key</var></tt>.
		 * 
		 * @throw tc::crypto::ArgumentException @p key_bit_size was not a multiple of 8.
		 */
	void generateKey(RsaKey& key, size_t key_bit_size)
	{
		if (key_bit_size == 0 || (key_bit_size % 8) != 0) throw tc::ArgumentException("tc::crypto::RsaKeyGenerator::generateKey()", "key_bit_size was not a multiple of 8.");

		key.n = tc::ByteData(key_bit_size/8);
		key.d = tc::ByteData(key_bit_size/8);
		key.e = tc::ByteData(4);

		mImpl.generateKey(key_bit_size, key.n.data(), key.n.size(), nullptr, 0, nullptr, 0, key.d.data(), key.d.size(), key.e.data(), key.e.size());
	}

private:
	detail::RsaKeyGeneratorImpl mImpl;
};

	/**
	 * @brief Utility function for generating an RSA key.
	 * 
	 * @param[out] key Buffer to generated RSA key.
	  * @param[in]  key_bit_size Size in bits of RSA key to generate.
	 * 
	 * @post
	 * - The generated key is written to <tt><var>key</var></tt>.
	 * 
	 * @throw tc::crypto::ArgumentException @p key_bit_size was not a multiple of 8.
	 */
void GenerateRsaKey(RsaKey& key, size_t key_bit_size);

}} // namespace tc::crypto