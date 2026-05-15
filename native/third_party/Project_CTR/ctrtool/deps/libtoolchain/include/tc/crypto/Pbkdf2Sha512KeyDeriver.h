	/**
	 * @file Pbkdf2Sha512KeyDeriver.h
	 * @brief Declarations for API resources for PBKDF2-SHA2-512 key derivation.
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/06/06
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/Sha512Generator.h>
#include <tc/crypto/Pbkdf2KeyDeriver.h>

namespace tc { namespace crypto {

	/**
	 * @typedef Pbkdf2Sha512KeyDeriver
	 * @brief Class for deriving a key using PBKDF2-SHA2-512.
	 * 
	 * @details This class derives a key using PBKDF2-SHA2-512.
	 * For more information refer to @ref Pbkdf2KeyDeriver.
	 */
using Pbkdf2Sha512KeyDeriver = Pbkdf2KeyDeriver<Sha512Generator>;

	/**
	 * @brief Utility function for deriving a key using PBKDF2-SHA2-512.
	 * 
	 * @param[out] key Pointer to the buffer storing the derived key.
	 * @param[in]  key_size Size of key to derive.
	 * @param[in]  password Pointer to password.
	 * @param[in]  password_size Size in bytes of password.
	 * @param[in]  salt Pointer to salt.
	 * @param[in]  salt_size Size in bytes of salt.
	 * @param[in]  n_rounds Number of PBKDF2 rounds.
	 * 
	 * @pre
	 * - @p n_rounds >= 1.
	 * - @p salt is optional however the strength of the derived key is reduced if the salt is not sufficently random.
	 * 
	 * @post
	 * - The derived key is written to <tt><var>key</var></tt>.
	 * 
	 * @throw tc::crypto::CryptoException @p n_rounds was < 1.
	 * @throw tc::crypto::CryptoException @p key_size was too large.
	 */
void DeriveKeyPbkdf2Sha512(byte_t* key, size_t key_size, const byte_t* password, size_t password_size, const byte_t* salt, size_t salt_size, size_t n_rounds);

}} // namespace tc::crypto
