	/**
	 * @file Aes128XtsEncryptor.h
	 * @brief Declarations for API resources for related to AES128-XTS mode encryption/decryption.
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/07/04
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/AesEncryptor.h>
#include <tc/crypto/XtsEncryptor.h>

namespace tc { namespace crypto {

	/**
	 * @typedef Aes128XtsEncryptor
	 * @brief Class for AES-XTS encryption/decryption with a keysize of 128 bits.
	 * 
	 * @details This class encrypts/decrypts data using using AES128-XTS.
	 * For more information refer to @ref XtsEncryptor.
	 */
using Aes128XtsEncryptor = XtsEncryptor<Aes128Encryptor>;

	/**
	 * @brief Utility function for AES128-XTS encryption.
	 * 
	 * @param[out] dst Buffer where encrypted data will be written.
	 * @param[in]  src Pointer to data to encrypt.
	 * @param[in]  size Size in bytes of data to encrypt.
	 * @param[in]  sector_number Initial sector number of sector to encrypt.
	 * @param[in]  key1 Pointer to key1 data.
	 * @param[in]  key1_size Size in bytes of key1 data.
	 * @param[in]  key2 Pointer to key2 data.
	 * @param[in]  key2_size Size in bytes of key2 data.
	 * @param[in]  sector_size Size in bytes of the XTS data unit.
	 * @param[in]  tweak_word_order Boolean to determine endianness of tweak. True = LittleEndian, False = BigEndian.
	 * 
	 * @pre
	 * - @p size is a multiple of @p sector_size.
	 * - @p key1_size == @ref Aes128XtsEncryptor::kKeySize.
	 * - @p key2_size == @ref Aes128XtsEncryptor::kKeySize.
	 * - @p sector_size >= @ref Aes128XtsEncryptor::kBlockSize. 
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
	 * @throw tc::ArgumentOutOfRangeException @p size was not a multiple of @p sector_size.
	 * @throw tc::ArgumentNullException @p key1 was null.
	 * @throw tc::ArgumentOutOfRangeException @p key1_size did not equal @ref Aes128XtsEncryptor::kKeySize.
	 * @throw tc::ArgumentNullException @p key2 was null.
	 * @throw tc::ArgumentOutOfRangeException @p key2_size did not equal @ref Aes128XtsEncryptor::kKeySize.
	 * @throw tc::ArgumentOutOfRangeException @p sector_size was less than @ref Aes128XtsEncryptor::kBlockSize.
	 */
void EncryptAes128Xts(byte_t* dst, const byte_t* src, size_t size, uint64_t sector_number, const byte_t* key1, size_t key1_size, const byte_t* key2, size_t key2_size, size_t sector_size, bool tweak_word_order);

	/**
	 * @brief Utility function for AES128-XTS decryption.
	 * 
	 * @param[out] dst Buffer where decrypted data will be written.
	 * @param[in]  src Pointer to data to decrypt.
	 * @param[in]  size Size in bytes of data to decrypt.
	 * @param[in]  sector_number Initial sector number of sector to decrypt.
	 * @param[in]  key1 Pointer to key1 data.
	 * @param[in]  key1_size Size in bytes of key1 data.
	 * @param[in]  key2 Pointer to key2 data.
	 * @param[in]  key2_size Size in bytes of key2 data.
	 * @param[in]  sector_size Size in bytes of the XTS data unit.
	 * @param[in]  tweak_word_order Boolean to determine endianness of tweak. True = LittleEndian, False = BigEndian.
	 * 
	 * @pre
	 * - @p size is a multiple of @p sector_size.
	 * - @p key1_size == @ref Aes128XtsEncryptor::kKeySize.
	 * - @p key2_size == @ref Aes128XtsEncryptor::kKeySize.
	 * - @p sector_size >= @ref Aes128XtsEncryptor::kBlockSize. 
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
	 * @throw tc::ArgumentOutOfRangeException @p size was not a multiple of @p sector_size.
	 * @throw tc::ArgumentNullException @p key1 was null.
	 * @throw tc::ArgumentOutOfRangeException @p key1_size did not equal @ref Aes128XtsEncryptor::kKeySize.
	 * @throw tc::ArgumentNullException @p key2 was null.
	 * @throw tc::ArgumentOutOfRangeException @p key2_size did not equal @ref Aes128XtsEncryptor::kKeySize.
	 * @throw tc::ArgumentOutOfRangeException @p sector_size was less than  @ref Aes128XtsEncryptor::kBlockSize.
	 */
void DecryptAes128Xts(byte_t* dst, const byte_t* src, size_t size, uint64_t sector_number, const byte_t* key1, size_t key1_size, const byte_t* key2, size_t key2_size, size_t sector_size, bool tweak_word_order);

}} // namespace tc::crypto
