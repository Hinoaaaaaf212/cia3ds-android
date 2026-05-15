	/**
	 * @file PseudoRandomByteGenerator.h
	 * @brief Declarations for API resources for generating pseudo-random data.
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/06/12
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/detail/PrbgImpl.h>

namespace tc { namespace crypto {

	/**
	 * @class PseudoRandomByteGenerator
	 * @brief Class for generating random data.
	 * 
	 * @details
	 * The underlying algorithm is CTR_DBRG.
	 * This class generates random data suitable for encryption use cases.
	 * - Initialization vectors
	 * - Salts / Nonces
	 * - HMAC keys
	 * - AES keys
	 */
class PseudoRandomByteGenerator
{
public:
		/**
		 * @brief Default constructor.
		 */
	PseudoRandomByteGenerator() :
		mImpl()
	{}

		/**
		 * @brief Populate array with random data.
		 * 
		 * @param[out] data Buffer to hold random data.
		 * @param[in] data_size Size of @p data buffer.
		 * 
		 * @throw tc::crypto::CryptoException An unexpected error has occurred.
		 * @throw tc::crypto::CryptoException Request too big.
		 */
	void getBytes(byte_t* data, size_t data_size)
	{
		mImpl.getBytes(data, data_size);
	}

private:
	detail::PrbgImpl mImpl;
};

	/**
	 * @brief Utility function for generating pseudo-random data.
	 * 
	 * @param[out] data Pointer to buffer storing pseudo-random data.
	 * @param[in]  data_size Size of pseudo-random data to generate.
	 * 
	 * @post
	 * - The generated pseudo-random data is written to <tt><var>data</var></tt>.
	 */
void GeneratePseudoRandomBytes(byte_t* data, size_t data_size);

}} // namespace tc::crypto