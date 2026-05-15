	/**
	 * @file CbcEncryptor.h
	 * @brief Declaration of tc::crypto::CbcEncryptor
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/07/04
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/detail/CbcModeImpl.h>

namespace tc { namespace crypto {

	/**
	 * @class CbcEncryptor
	 * @brief Class for CBC mode encryption/decryption.
	 * 
	 * @tparam BlockCipher The class that implements the block cipher used for CBC mode encryption/decryption.
	 * 
	 * @details
	 * Cipher block chaining (CBC) mode is intended to encrypt whole sets of data. Random access is not a feature of CBC mode.
	 * 
	 * This class is a template class that takes a block cipher implementation class as template parameter.
	 * See @ref Aes128CbcEncryptor or similar for supplied realizations of this template class.
	 * 
	 * The implementation of @a BlockCipher must satisfies the following conditions.
	 * See @ref AesEncryptor or similar class, for more information including parameters to each function.
	 * 
	 * -# Has a @p kBlockSize constant that defines the size of the block to process.
	 * -# Has a @p kKeySize constant that defines the required key size to initialize the block cipher.
	 * -# Has an @p initialize method that initializes the state of the block cipher.
	 * -# Has an @p encrypt method that encrypts a block of input data.
	 * -# Has a @p decrypt method that decrypts a block of input data.
	 * 
	 * This class has two states:
	 * - None : Not ready
	 * - Initialized : Ready to process data
	 * 
	 * General usage of this class is as follows:
	 * - Initialize CBC state with @ref initialize().
	 * - Encrypt or decrypt data using @ref encrypt() or @ref decrypt().
	 */
template <class BlockCipher>
class CbcEncryptor
{
public:
	static const size_t kKeySize   = BlockCipher::kKeySize; /**< CBC mode key size. */
	static const size_t kBlockSize = BlockCipher::kBlockSize; /**< CBC mode block processing size. */

		/**
		 * @brief Default constructor.
		 * 
		 * @post
		 * - State is None. @ref initialize() must be called before use.
		 */
	CbcEncryptor() :
		mImpl()
	{}

		/**
		 * @brief Initializes the CBC encryption state.
		 * 
		 * @param[in] key Pointer to key data.
		 * @param[in] key_size Size in bytes of key data.
		 * @param[in] iv Pointer to initialization vector.
		 * @param[in] iv_size Size in bytes of initialization vector.
		 * 
		 * @pre
		 * - @p key_size == @ref kKeySize.
		 * - @p iv_size == @ref kBlockSize.
		 * 
		 * @post
		 * - Instance is now in a Initialized state.
		 * 
		 * @details
		 * This resets the CBC state, initializing the key schedule and initialization vector.
		 * 
		 * @note
		 * - This must be called before performing encryption/decryption.
		 * 
		 * @throw tc::ArgumentNullException @p key was null.
		 * @throw tc::ArgumentOutOfRangeException @p key_size did not equal kKeySize.
		 * @throw tc::ArgumentNullException @p iv was null.
		 * @throw tc::ArgumentOutOfRangeException @p iv_size did not equal kBlockSize.
		 */
	void initialize(const byte_t* key, size_t key_size, const byte_t* iv, size_t iv_size)
	{
		mImpl.initialize(key, key_size, iv, iv_size);
	}

		/**
		 * @brief Updates the CBC initialization vector.
		 * 
		 * @param[in] iv Pointer to initialization vector.
		 * @param[in] iv_size Size in bytes of initialization vector.
		 * 
		 * @pre
		 * - @p iv_size == @ref kBlockSize.
		 * - Instance is in a Initialized state.
		 * 
		 * @post
		 * - Initialization vector is updated.
		 * 
		 * @details
		 * This updates the CBC state, initializing the initialization vector. The intended use is when data is encrypted/decrypted out of order, so the initialization vector needs to be updated manually.
		 * 
		 * @throw tc::ArgumentNullException @p key was null.
		 * @throw tc::ArgumentOutOfRangeException @p key_size did not equal kKeySize.
		 * @throw tc::ArgumentNullException @p iv was null.
		 * @throw tc::ArgumentOutOfRangeException @p iv_size did not equal kBlockSize.
		 */
	void update_iv(const byte_t* iv, size_t iv_size)
	{
		mImpl.update_iv(iv, iv_size);
	}

		/**
		 * @brief Encrypt data.
		 * 
		 * @param[out] dst Buffer where encrypted data will be written.
		 * @param[in]  src Pointer to data to encrypt.
		 * @param[in]  size Size in bytes of data to encrypt.
		 * 
		 * @pre
		 * - @p size is a multiple of @ref kBlockSize.
		 * 
		 * @post
		 * - Encrypted data is written to @p dst.
		 * 
		 * @details
		 * This encrypts the data in @p src, writing it to @p dst.
		 * 
		 * @note
		 * - @p dst and @p src can be the same pointer.
		 * 
		 * @throw tc::ArgumentNullException @p dst was null.
		 * @throw tc::ArgumentNullException @p src was null.
		 * @throw tc::ArgumentOutOfRangeException @p size size was not a multiple of @ref kBlockSize.
		 */
	void encrypt(byte_t* dst, const byte_t* src, size_t size)
	{
		mImpl.encrypt(dst, src, size);
	}

		/**
		 * @brief Decrypt data.
		 * 
		 * @param[out] dst Buffer where decrypted data will be written.
		 * @param[in]  src Pointer to data to decrypt.
		 * @param[in]  size Size in bytes of data to decrypt.
		 * 
		 * @pre
		 * - @p size is a multiple of @ref kBlockSize.
		 * 
		 * @post
		 * - Decrypted data is written to @p dst.
		 * 
		 * @details
		 * This decrypts the data in @p src, writing it to @p dst.
		 * 
		 * @note
		 * - @p dst and @p src can be the same pointer.
		 * 
		 * @throw tc::ArgumentNullException @p dst was null.
		 * @throw tc::ArgumentNullException @p src was null.
		 * @throw tc::ArgumentOutOfRangeException @p size size was not a multiple of @ref kBlockSize.
		 */
	void decrypt(byte_t* dst, const byte_t* src, size_t size)
	{
		mImpl.decrypt(dst, src, size);
	}

private:
	detail::CbcModeImpl<BlockCipher> mImpl;
};

}} // namespace tc::crypto