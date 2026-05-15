	/**
	 * @file Pbkdf1Sha1KeyDeriver.h
	 * @brief Declarations for API resources for PBKDF1-SHA1 key derivation.
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/06/06
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/Sha1Generator.h>
#include <tc/crypto/Pbkdf1KeyDeriver.h>

namespace tc { namespace crypto {

	/**
	 * @typedef Pbkdf1Sha1KeyDeriver
	 * @brief Class for deriving a key using PBKDF1-SHA1.
	 * 
	 * @details This class derives a key using PBKDF1-SHA1.
	 * For more information refer to @ref Pbkdf1KeyDeriver.
	 */
using Pbkdf1Sha1KeyDeriver = Pbkdf1KeyDeriver<Sha1Generator>;

	/**
	 * @brief Utility function for deriving a key using PBKDF1-SHA1.
	 * 
	 * @param[out] key Pointer to the buffer storing the derived key.
	 * @param[in]  key_size Size of key to derive.
	 * @param[in]  password Pointer to password.
	 * @param[in]  password_size Size in bytes of password.
	 * @param[in]  salt Pointer to salt.
	 * @param[in]  salt_size Size in bytes of salt.
	 * @param[in]  n_rounds Number of PBKDF1 rounds.
	 * 
	 * @pre
	 * - @p n_round >= 1 
	 * - @p salt is optional however the strength of the derived key is reduced if the salt is not sufficently random.
	 * 
	 * @post
	 * - The derived key is written to <tt><var>key</var></tt>.
	 * 
	 * @throw tc::crypto::CryptoException @p n_round was < 1
	 * @throw tc::crypto::CryptoException @p key_size was too large.
	 */
void DeriveKeyPbkdf1Sha1(byte_t* key, size_t key_size, const byte_t* password, size_t password_size, const byte_t* salt, size_t salt_size, size_t n_rounds);

}} // namespace tc::crypto
