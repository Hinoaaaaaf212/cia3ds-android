	/**
	 * @file Aes192CtrEncryptor.h
	 * @brief Declarations for API resources for related to AES192-CTR mode encryption/decryption.
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/07/04
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/AesEncryptor.h>
#include <tc/crypto/CtrEncryptor.h>

namespace tc { namespace crypto {

	/**
	 * @typedef Aes192CtrEncryptor
	 * @brief Class for AES-CTR encryption/decryption with a keysize of 192 bits.
	 * 
	 * @details This class encrypts/decrypts data using using AES192-CTR.
	 * For more information refer to @ref CtrEncryptor.
	 */
using Aes192CtrEncryptor = CtrEncryptor<Aes192Encryptor>;

	/**
	 * @brief Utility function for AES192-CTR encryption.
	 * 
	 * @param[out] dst Buffer where encrypted data will be written.
	 * @param[in]  src Pointer to data to encrypt.
	 * @param[in]  size Size in bytes of data to encrypt.
	 * @param[in]  block_number Block number of initial block to encrypt.
	 * @param[in]  key Pointer to key data.
	 * @param[in]  key_size Size in bytes of key data.
	 * @param[in]  iv Pointer to initialization vector.
	 * @param[in]  iv_size Size in bytes of initialization vector.
	 * 
	 * @pre
	 * - @p size > 0.
	 * - @p key_size == @ref Aes192CtrEncryptor::kKeySize.
	 * - @p iv_size == @ref Aes192CtrEncryptor::kBlockSize.
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
	 * @throw tc::ArgumentOutOfRangeException @p size was 0.
	 * @throw tc::ArgumentNullException @p key was null.
	 * @throw tc::ArgumentOutOfRangeException @p key_size did not equal @ref Aes192CtrEncryptor::kKeySize.
	 * @throw tc::ArgumentNullException @p iv was null.
	 * @throw tc::ArgumentOutOfRangeException @p iv_size did not equal @ref Aes192CtrEncryptor::kBlockSize.
	 */
void EncryptAes192Ctr(byte_t* dst, const byte_t* src, size_t size, uint64_t block_number, const byte_t* key, size_t key_size, const byte_t* iv, size_t iv_size);

	/**
	 * @brief Utility function for AES192-CTR decryption.
	 * 
	 * @param[out] dst Buffer where decrypted data will be written.
	 * @param[in]  src Pointer to data to decrypt.
	 * @param[in]  size Size in bytes of data to decrypt.
	 * @param[in]  block_number Block number of initial block to encrypt.
	 * @param[in]  key Pointer to key data.
	 * @param[in]  key_size Size in bytes of key data.
	 * @param[in]  iv Pointer to initialization vector.
	 * @param[in]  iv_size Size in bytes of initialization vector.
	 * 
	 * @pre
	 * - @p size > 0.
	 * - @p key_size == @ref Aes192CtrEncryptor::kKeySize.
	 * - @p iv_size == @ref Aes192CtrEncryptor::kBlockSize.
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
	 * @throw tc::ArgumentOutOfRangeException @p size was 0.
	 * @throw tc::ArgumentNullException @p key was null.
	 * @throw tc::ArgumentOutOfRangeException @p key_size did not equal @ref Aes192CtrEncryptor::kKeySize.
	 * @throw tc::ArgumentNullException @p iv was null.
	 * @throw tc::ArgumentOutOfRangeException @p iv_size did not equal @ref Aes192CtrEncryptor::kBlockSize.
	 */
void DecryptAes192Ctr(byte_t* dst, const byte_t* src, size_t size, uint64_t block_number, const byte_t* key, size_t key_size, const byte_t* iv, size_t iv_size);

	/**
	 * @brief Utility function for incrementing a AES192-CTR block counter.
	 * 
	 * @param[in,out] counter Pointer to block counter to increment.
	 * @param[in]     incr Value to increment the block counter with.
	 * 
	 * @pre
	 * - @p counter != nullptr
	 * 
	 * @post
	 * - Block counter @p counter is incremented by the value of @p incr.
	 * 
	 * @details
	 * This increments the block counter (@p counter) (used in CTR-Mode as the initialization vector) by the value of @p incr.
	 * 
	 * @throw tc::ArgumentNullException @p counter was null.
	 */
void IncrementCounterAes192Ctr(byte_t* counter, uint64_t incr);

}} // namespace tc::crypto
