	/**
	 * @file EcbEncryptor.h
	 * @brief Declaration of tc::crypto::EcbEncryptor
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/07/04
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/detail/EcbModeImpl.h>

namespace tc { namespace crypto {

	/**
	 * @class EcbEncryptor
	 * @brief Class for ECB mode encryption/decryption.
	 * 
	 * @tparam BlockCipher The class that implements the block cipher used for ECB mode encryption/decryption.
	 * 
	 * @details
	 * Electronic Codebook (ECB) mode is akin to using a block cipher in raw mode.
	 * 
	 * This class is a template class that takes a block cipher implementation class as template parameter.
	 * See @ref Aes128EcbEncryptor or similar for supplied realizations of this template class.
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
	 * - Initialize ECB state with @ref initialize().
	 * - Encrypt or decrypt data using @ref encrypt() or @ref decrypt().
	 */
template <class BlockCipher>
class EcbEncryptor
{
public:
	static const size_t kKeySize   = BlockCipher::kKeySize; /**< ECB mode key size. */
	static const size_t kBlockSize = BlockCipher::kBlockSize; /**< ECB mode block processing size. */

		/**
		 * @brief Default constructor.
		 * 
		 * @post
		 * - State is None. @ref initialize() must be called before use.
		 */
	EcbEncryptor() :
		mImpl()
	{}

		/**
		 * @brief Initializes the ECB encryption state.
		 * 
		 * @param[in] key Pointer to key data.
		 * @param[in] key_size Size in bytes of key data.
		 * 
		 * @pre
		 * - @p key_size == @ref kKeySize.
		 * 
		 * @post
		 * - Instance is now in a Initialized state.
		 * 
		 * @details
		 * This resets the ECB state, initializing the key schedule.
		 * 
		 * @note
		 * - This must be called before performing encryption/decryption.
		 * 
		 * @throw tc::ArgumentNullException @p key was null.
		 * @throw tc::ArgumentOutOfRangeException @p key_size did not equal kKeySize.
		 */
	void initialize(const byte_t* key, size_t key_size)
	{
		mImpl.initialize(key, key_size);
	}

		/**
		 * @brief Encrypt data.
		 * 
		 * @param[out] dst Buffer where encrypted data will be written.
		 * @param[in]  src Pointer to data to encrypt.
		 * @param[in]  size Size in bytes of data to encrypt.
		 * 
		 * @pre
		 * - @p size >= @ref kBlockSize.
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
		 * @throw tc::ArgumentOutOfRangeException @p size was less than @ref kBlockSize.
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
		 * - @p size >= @ref kBlockSize.
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
		 * @throw tc::ArgumentOutOfRangeException @p size was less than @ref kBlockSize.
		 */
	void decrypt(byte_t* dst, const byte_t* src, size_t size)
	{
		mImpl.decrypt(dst, src, size);
	}

private:
	detail::EcbModeImpl<BlockCipher> mImpl;
};

}} // namespace tc::crypto