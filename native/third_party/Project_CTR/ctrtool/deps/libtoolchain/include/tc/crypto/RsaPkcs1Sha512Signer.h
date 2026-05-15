	/**
	 * @file RsaPkcs1Sha512Signer.h
	 * @brief Declarations for API resources for RSA-PKCS1-SHA2-512 calculations.
	 * @author Jack (jakcron)
	 * @version 0.3
	 * @date 2020/09/28
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/Sha512Generator.h>
#include <tc/crypto/RsaPkcs1Signer.h>

namespace tc { namespace crypto {

	/**
	 * @typedef Rsa1024Pkcs1Sha512Signer
	 * @brief Class for generating and verifying RSA1024-PKCS1-SHA2-512 signatures.
	 * 
	 * @details This class uses RSA1024-PKCS1 to sign/validate SHA2-512 message digests.
	 * For more information refer to @ref RsaPkcs1Signer.
	 */
using Rsa1024Pkcs1Sha512Signer = RsaPkcs1Signer<1024,Sha512Generator>;

	/**
	 * @typedef Rsa2048Pkcs1Sha512Signer
	 * @brief Class for generating and verifying RSA2048-PKCS1-SHA2-512 signatures.
	 * 
	 * @details This class uses RSA2048-PKCS1 to sign/validate SHA2-512 message digests.
	 * For more information refer to @ref RsaPkcs1Signer.
	 */
using Rsa2048Pkcs1Sha512Signer = RsaPkcs1Signer<2048,Sha512Generator>;

	/**
	 * @typedef Rsa4096Pkcs1Sha512Signer
	 * @brief Class for generating and verifying RSA4096-PKCS1-SHA2-512 signatures.
	 * 
	 * @details This class uses RSA4096-PKCS1 to sign/validate SHA2-512 message digests.
	 * For more information refer to @ref RsaPkcs1Signer.
	 */
using Rsa4096Pkcs1Sha512Signer = RsaPkcs1Signer<4096,Sha512Generator>;

	/**
	 * @brief Utility function for calculating a RSA1024-PKCS1-SHA2-512 signature.
	 * 
	 * @param[out] signature Pointer to the buffer storing the signature.
	 * @param[in]  message_digest Pointer to message digest.
	 * @param[in]  key Reference to RSA private key.
	 * @return true if signature calculation was successful.
	 * 
	 * @pre
	 * - Size of the signature buffer must >= <tt>Rsa1024Pkcs1Sha512Signer::kSignatureSize</tt>.
	 * 
	 * @post
	 * - The signature is written to <tt><var>signature</var></tt>.
	 * 
	 * @details
	 * This function calculates a signature for a message digest.
	 * To calculate a message digest use the @ref Sha512Generator class.
	 */
bool SignRsa1024Pkcs1Sha512(byte_t* signature, const byte_t* message_digest, const RsaKey& key);

	/**
	 * @brief Utility function for verfifying a RSA1024-PKCS1-SHA2-512 signature.
	 * 
	 * @param[in] signature Pointer to signature.
	 * @param[in] message_digest Pointer to message digest.
	 * @param[in] key Reference to RSA public key.
	 * @return true if the signature is valid, otherwise false.
	 * 
	 * @details
	 * This function verifies a signature for a message digest.
	 * To calculate a message digest use the @ref Sha512Generator class.
	 */
bool VerifyRsa1024Pkcs1Sha512(const byte_t* signature, const byte_t* message_digest, const RsaKey& key);

	/**
	 * @brief Utility function for calculating a RSA2048-PKCS1-SHA2-512 signature.
	 * 
	 * @param[out] signature Pointer to the buffer storing the signature.
	 * @param[in]  message_digest Pointer to message digest.
	 * @param[in]  key Reference to RSA private key.
	 * @return true if signature calculation was successful.
	 * 
	 * @pre
	 * - Size of the signature buffer must >= <tt>Rsa2048Pkcs1Sha512Signer::kSignatureSize</tt>.
	 * 
	 * @post
	 * - The signature is written to <tt><var>signature</var></tt>.
	 * 
	 * @details
	 * This function calculates a signature for a message digest.
	 * To calculate a message digest use the @ref Sha512Generator class.
	 */
bool SignRsa2048Pkcs1Sha512(byte_t* signature, const byte_t* message_digest, const RsaKey& key);

	/**
	 * @brief Utility function for verfifying a RSA2048-PKCS1-SHA2-512 signature.
	 * 
	 * @param[in] signature Pointer to signature.
	 * @param[in] message_digest Pointer to message digest.
	 * @param[in] key Reference to RSA public key.
	 * @return true if the signature is valid, otherwise false.
	 * 
	 * @details
	 * This function verifies a signature for a message digest.
	 * To calculate a message digest use the @ref Sha512Generator class.
	 */
bool VerifyRsa2048Pkcs1Sha512(const byte_t* signature, const byte_t* message_digest, const RsaKey& key);

	/**
	 * @brief Utility function for calculating a RSA4096-PKCS1-SHA2-512 signature.
	 * 
	 * @param[out] signature Pointer to the buffer storing the signature.
	 * @param[in]  message_digest Pointer to message digest.
	 * @param[in]  key Reference to RSA private key.
	 * @return true if signature calculation was successful.
	 * 
	 * @pre
	 * - Size of the signature buffer must >= <tt>Rsa4096Pkcs1Sha512Signer::kSignatureSize</tt>.
	 * 
	 * @post
	 * - The signature is written to <tt><var>signature</var></tt>.
	 * 
	 * @details
	 * This function calculates a signature for a message digest.
	 * To calculate a message digest use the @ref Sha512Generator class.
	 */
bool SignRsa4096Pkcs1Sha512(byte_t* signature, const byte_t* message_digest, const RsaKey& key);

	/**
	 * @brief Utility function for verfifying a RSA4096-PKCS1-SHA2-512 signature.
	 * 
	 * @param[in] signature Pointer to signature.
	 * @param[in] message_digest Pointer to message digest.
	 * @param[in] key Reference to RSA public key.
	 * @return true if the signature is valid, otherwise false.
	 * 
	 * @details
	 * This function verifies a signature for a message digest.
	 * To calculate a message digest use the @ref Sha512Generator class.
	 */
bool VerifyRsa4096Pkcs1Sha512(const byte_t* signature, const byte_t* message_digest, const RsaKey& key);

}} // namespace tc::crypto
