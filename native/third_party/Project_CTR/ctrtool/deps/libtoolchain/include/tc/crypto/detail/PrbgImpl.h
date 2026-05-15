	/**
	 * @file PrbgImpl.h
	 * @brief Declaration of tc::crypto::detail::PrbgImpl
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/06/12
	 **/
#pragma once
#include <tc/types.h>

#include <tc/crypto/CryptoException.h>

namespace tc { namespace crypto { namespace detail {

	/**
	 * @class PrbgImpl
	 * @brief This class implements a Psuedo Random Byte Generator.
	 * 
	 * @details
	 * The underlying algorithm is CTR_DBRG.
	 * This class generates random data suitable for encryption use cases.
	 * - Initialization vectors
	 * - Salts / Nonces
	 * - HMAC keys
	 * - AES keys
	 */
class PrbgImpl
{
public:
		/**
		 * @brief Default constructor
		 * @details
		 * This initializes random number generator state
		 */
	PrbgImpl();

		/**
		 * @brief Destructor
		 * @details
		 * Cleans up random number generator state
		 */
	~PrbgImpl();

		/**
		 * @brief Populate array with random data.
		 * 
		 * @param[out] data Buffer to hold random data.
		 * @param[in] data_size Size of @p data buffer.
		 * 
		 * @throw tc::crypto::CryptoException An unexpected error has occurred.
		 * @throw tc::crypto::CryptoException Request too big.
		 */
	void getBytes(byte_t* data, size_t data_size);
private:
	static const std::string kClassName;	

	struct ImplCtx;
	std::unique_ptr<ImplCtx> mImplCtx;
};

}}} // namespace tc::crypto::detail