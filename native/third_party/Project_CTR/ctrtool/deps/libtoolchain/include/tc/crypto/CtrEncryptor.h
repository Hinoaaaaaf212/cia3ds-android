	/**
	 * @file CtrEncryptor.h
	 * @brief Declaration of tc::crypto::CtrEncryptor
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/07/04
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/detail/CtrModeImpl.h>

namespace tc { namespace crypto {

	/**
	 * @class CtrEncryptor
	 * @brief Class for CTR mode encryption/decryption.
	 * 
	 * @tparam BlockCipher The class that implements the block cipher used for CTR mode encryption/decryption.
	 * 
	 * @details
	 * Counter (CTR) mode encrypts blocks of data independently (and uniquely) of each other, acting as a psuedo stream cipher. Random access supported by CTR mode.
	 * Encryption and decryption operations are identical.
	 * 
	 * This class is a template class that takes a block cipher implementation class as template parameter.
	 * See @ref Aes128CtrEncryptor or similar for supplied realizations of this template class.
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
	 * - Initialize CTR state with @ref initialize().
	 * - Encrypt or decrypt data using @ref encrypt() or @ref decrypt().
	 */
template <class BlockCipher>
class CtrEncryptor
{
public:
	static const size_t kKeySize   = BlockCipher::kKeySize; /**< CTR mode key size. */
	static const size_t kBlockSize = BlockCipher::kBlockSize; /**< CTR mode block processing size. */

		/**
		 * @brief Default constructor.
		 * 
		 * @post
		 * - State is None. @ref initialize() must be called before use.
		 */
	CtrEncryptor() :
		mImpl()
	{}

		/**
		 * @brief Initializes the CTR encryption state.
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
		 * This resets the CTR state, initializing the key schedule and initialization vector.
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
		 * @brief Encrypt data.
		 * 
		 * @param[out] dst Buffer where encrypted data will be written.
		 * @param[in]  src Pointer to data to encrypt.
		 * @param[in]  size Size in bytes of data to encrypt.
		 * @param[in]  block_number Block number of initial block to encrypt.
		 * 
		 * @pre
		 * - @p size > 0.
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
		 * @throw tc::ArgumentOutOfRangeException @p size size was 0.
		 */
	void encrypt(byte_t* dst, const byte_t* src, size_t size, uint64_t block_number)
	{
		mImpl.crypt(dst, src, size, block_number);
	}

		/**
		 * @brief Decrypt data.
		 * 
		 * @param[out] dst Buffer where decrypted data will be written.
		 * @param[in]  src Pointer to data to decrypt.
		 * @param[in]  size Size in bytes of data to decrypt.
		 * @param[in]  block_number Block number of initial block to encrypt.
		 * 
		 * @pre
		 * - @p size > 0.
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
		 * @throw tc::ArgumentOutOfRangeException @p size size was 0.
		 */
	void decrypt(byte_t* dst, const byte_t* src, size_t size, uint64_t block_number)
	{
		mImpl.crypt(dst, src, size, block_number);
	}

private:
	detail::CtrModeImpl<BlockCipher> mImpl;
};

}} // namespace tc::crypto