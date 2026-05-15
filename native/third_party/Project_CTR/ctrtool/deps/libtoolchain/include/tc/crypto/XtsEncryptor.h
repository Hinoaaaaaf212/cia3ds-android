	/**
	 * @file XtsEncryptor.h
	 * @brief Declaration of tc::crypto::XtsEncryptor
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/07/04
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/detail/XtsModeImpl.h>

namespace tc { namespace crypto {

	/**
	 * @class XtsEncryptor
	 * @brief Class for XTS mode encryption/decryption.
	 * 
	 * @tparam BlockCipher The class that implements the block cipher used for XTS mode encryption/decryption.
	 * 
	 * @details
	 * XTS (<b>X</b>EX mode with cipher<b>t</b>ext <b>s</b>tealing) mode is designed for disk encryption, and as such operates on sectors of data rather than blocks.
	 * Unlike XEX (<b>X</b>OR <b>e</b>ncrypt <b>X</b>OR) mode the sector size does not have to be a multiple of the block size.
	 * 
	 * This class is a template class that takes a block cipher implementation class as template parameter.
	 * See @ref Aes128XtsEncryptor or similar for supplied realizations of this template class.
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
	 * - Initialize XTS state with @ref initialize().
	 * - Encrypt or decrypt sector(s) using @ref encrypt() or @ref decrypt().
	 */
template <class BlockCipher>
class XtsEncryptor
{
public:
	static const size_t kKeySize   = BlockCipher::kKeySize; /**< XTS mode key size. */
	static const size_t kBlockSize = BlockCipher::kBlockSize; /**< XTS mode block processing size. */

		/**
		 * @brief Get specified sector size.
		 */
	size_t sector_size() const
	{
		return mImpl.sector_size();
	}

		/**
		 * @brief Default constructor.
		 * 
		 * @post
		 * - State is None. @ref initialize() must be called before use.
		 */
	XtsEncryptor() :
		mImpl()
	{}

		/**
		 * @brief Initializes the XTS encryption state.
		 * 
		 * @param[in] key1 Pointer to key1 data.
		 * @param[in] key1_size Size in bytes of key1 data.
		 * @param[in] key2 Pointer to key2 data.
		 * @param[in] key2_size Size in bytes of key2 data.
		 * @param[in] sector_size Size in bytes of the XTS data unit.
		 * @param[in] tweak_word_order Boolean to determine word order of tweak. True = LittleEndian, False = BigEndian.
		 * 
		 * @pre
		 * - @p key1_size == @ref kKeySize.
		 * - @p key2_size == @ref kKeySize.
		 * - @p sector_size >= @ref kBlockSize. 
		 * 
		 * @post
		 * - Instance is now in a Initialized state.
		 * 
		 * @details
		 * This resets the XTS state, initializes the two key schedules and the initialization vector (IV).
		 * The IV should be the IV for sector 0. Using @ref encrypt() / @ref decrypt() will update the IV as required.
		 * 
		 * @note
		 * - This must be called before performing encryption/decryption.
		 * 
		 * @throw tc::ArgumentNullException @p key1 was null.
		 * @throw tc::ArgumentOutOfRangeException @p key1_size did not equal kKeySize.
		 * @throw tc::ArgumentNullException @p key2 was null.
		 * @throw tc::ArgumentOutOfRangeException @p key2_size did not equal kKeySize.
		 * @throw tc::ArgumentOutOfRangeException @p sector_size was less than kBlockSize.
		 */
	void initialize(const byte_t* key1, size_t key1_size, const byte_t* key2, size_t key2_size, size_t sector_size, bool tweak_word_order)
	{
		mImpl.initialize(key1, key1_size, key2, key2_size, sector_size, tweak_word_order);
	}

		/**
		 * @brief Encrypts data in multiples of the sector size.
		 * 
		 * @param[out] dst Buffer where encrypted data will be written.
		 * @param[in]  src Pointer to data to encrypt.
		 * @param[in]  size Size in bytes of data to encrypt.
		 * @param[in]  sector_number Initial sector number of sector to encrypt.
		 * 
		 * @pre
		 * - @p size is a multiple of the sector size.
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
		 * @throw tc::ArgumentOutOfRangeException @p size was not a multiple of the sector size.
		 */
	void encrypt(byte_t* dst, const byte_t* src, size_t size, uint64_t sector_number)
	{
		mImpl.encrypt(dst, src, size, sector_number);
	}

		/**
		 * @brief Decrypts data in multiples of the sector size.
		 * 
		 * @param[out] dst Buffer where decrypted data will be written.
		 * @param[in]  src Pointer to data to decrypt.
		 * @param[in]  size Size in bytes of data to decrypt.
		 * @param[in]  sector_number Initial sector number of sector to decrypt.
		 * 
		 * @pre
		 * - @p size is a multiple of the sector size.
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
		 * @throw tc::ArgumentOutOfRangeException @p size was not a multiple of the sector size.
		 */
	void decrypt(byte_t* dst, const byte_t* src, size_t size, uint64_t sector_number)
	{
		mImpl.decrypt(dst, src, size, sector_number);
	}

private:
	detail::XtsModeImpl<BlockCipher> mImpl;
};

}} // namespace tc::crypto