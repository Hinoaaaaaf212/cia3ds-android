	/**
	 * @file AesEncryptor.h
	 * @brief Declarations for API resources related to AES block encryption.
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/07/04
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/detail/AesImpl.h>

#include <tc/ArgumentOutOfRangeException.h>
#include <tc/ArgumentNullException.h>

namespace tc { namespace crypto {

	/**
	 * @class AesEncryptor
	 * @brief Class for AES encryption/decryption.
	 * 
	 * @tparam KeySize Size in bytes of the AES encryption key. This must be 16, 24 or 32.
	 * 
	 * @details
	 * This class has three states:
	 * - None : Not ready
	 * - Initialized : Ready to process data
	 * 
	 * General usage of this class is as follows:
	 * - Initialize AES state with @ref initialize().
	 * - Encrypt or decrypt block(s) using @ref encrypt() or @ref decrypt().
	 */
template <size_t KeySize> 
class AesEncryptor
{
public:
	static_assert(KeySize == 16 || KeySize == 24 || KeySize == 32, "Unsupported AES KeySize");

	static const size_t kKeySize   = KeySize; /**< AES key size. */
	static const size_t kBlockSize = 16; /**< AES processing block size. */

		/**
		 * @brief Default constructor.
		 * 
		 * @post
		 * - State is None. @ref initialize() must be called before use.
		 */
	AesEncryptor() :
		mImpl()
	{}

		/**
		 * @brief Initialize AES state with key.
		 * 
		 * @param[in] key Pointer to key data.
		 * @param[in] key_size Size in bytes of key data.
		 * 
		 * @pre 
		 * - @p key_size == @ref kKeySize.
		 * @post
		 * - Instance is now in initialized state.
		 * 
		 * @throw tc::ArgumentNullException @p key was null.
		 * @throw tc::ArgumentOutOfRangeException @p key_size did not equal @ref kKeySize.
		 */
	void initialize(const byte_t* key, size_t key_size)
	{
		if (key_size != kKeySize) { throw tc::ArgumentOutOfRangeException("AesEncryptor::initialize()", "key_size did not equal kKeySize."); }

		mImpl.initialize(key, key_size);
	}

		/**
		 * @brief Encrypt data block.
		 * 
		 * @param[out] dst Buffer to store encrypted block.
		 * @param[in]  src Pointer to block to encrypt.
		 * 
		 * @pre 
		 * - Instance is in initialized state.
		 * 
		 * @details
		 * This encrypts @ref kBlockSize number of bytes of data from @p src, writing it to @p dst.
		 * 
		 * @note 
		 *  - @p dst and @p src can be the same pointer.
		 * 
		 * @throw tc::ArgumentNullException @p dst was null.
		 * @throw tc::ArgumentNullException @p src was null.
		 */
	void encrypt(byte_t* dst, const byte_t* src)
	{
		mImpl.encrypt(dst, src);
	}

		/**
		 * @brief Decrypt data block.
		 * 
		 * @param[out] dst Buffer to store decrypted block.
		 * @param[in]  src Pointer to block to decrypt.
		 * 
		 * @pre 
		 * - Instance is in initialized state.
		 * 
		 * @details
		 * This decrypts @ref kBlockSize number of bytes of data from @p src, writing it to @p dst.
		 * 
		 * @note 
		 *  - @p dst and @p src can be the same pointer.
		 * 
		 * @throw tc::ArgumentNullException @p dst was null.
		 * @throw tc::ArgumentNullException @p src was null.
		 */
	void decrypt(byte_t* dst, const byte_t* src)
	{
		mImpl.decrypt(dst, src);
	}

private:
	detail::AesImpl mImpl;
};

	/**
	 * @typedef Aes128Encryptor
	 * @brief Class for AES-128 encryption/decryption.
	 */
using Aes128Encryptor = AesEncryptor<16>;

	/**
	 * @typedef Aes192Encryptor
	 * @brief Class for AES-192 encryption/decryption.
	 */
using Aes192Encryptor = AesEncryptor<24>;

	/**
	 * @typedef Aes256Encryptor
	 * @brief Class for AES-256 encryption/decryption.
	 */
using Aes256Encryptor = AesEncryptor<32>;

}} // namespace tc::crypto