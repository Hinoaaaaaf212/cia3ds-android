	/**
	 * @file RsaPssSha256Signer.h
	 * @brief Declarations for API resources for RSA-PSS-SHA2-256 calculations.
	 * @author Jack (jakcron)
	 * @version 0.3
	 * @date 2020/09/28
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/Sha256Generator.h>
#include <tc/crypto/RsaPssSigner.h>

namespace tc { namespace crypto {

	/**
	 * @typedef Rsa1024PssSha256Signer
	 * @brief Class for generating and verifying RSA1024-PSS-SHA2-256 signatures.
	 * 
	 * @details This class uses RSA1024-PSS to sign/validate SHA2-256 message digests.
	 * For more information refer to @ref RsaPssSigner.
	 */
using Rsa1024PssSha256Signer = RsaPssSigner<1024,Sha256Generator>;

	/**
	 * @typedef Rsa2048PssSha256Signer
	 * @brief Class for generating and verifying RSA2048-PSS-SHA2-256 signatures.
	 * 
	 * @details This class uses RSA2048-PSS to sign/validate SHA2-256 message digests.
	 * For more information refer to @ref RsaPssSigner.
	 */
using Rsa2048PssSha256Signer = RsaPssSigner<2048,Sha256Generator>;

	/**
	 * @typedef Rsa4096PssSha256Signer
	 * @brief Class for generating and verifying RSA4096-PSS-SHA2-256 signatures.
	 * 
	 * @details This class uses RSA4096-PSS to sign/validate SHA2-256 message digests.
	 * For more information refer to @ref RsaPssSigner.
	 */
using Rsa4096PssSha256Signer = RsaPssSigner<4096,Sha256Generator>;

	/**
	 * @brief Utility function for calculating a RSA1024-PSS-SHA2-256 signature.
	 * 
	 * @param[out] signature Pointer to the buffer storing the signature.
	 * @param[in]  message_digest Pointer to message digest.
	 * @param[in]  key Reference to RSA private key.
	 * @return true if signature calculation was successful.
	 * 
	 * @pre
	 * - Size of the signature buffer must >= <tt>Rsa1024PssSha256Signer::kSignatureSize</tt>.
	 * 
	 * @post
	 * - The signature is written to <tt><var>signature</var></tt>.
	 * 
	 * @details
	 * This function calculates a signature for a message digest.
	 * To calculate a message digest use the @ref Sha256Generator class.
	 */
bool SignRsa1024PssSha256(byte_t* signature, const byte_t* message_digest, const RsaKey& key);

	/**
	 * @brief Utility function for verfifying a RSA1024-PSS-SHA2-256 signature.
	 * 
	 * @param[in] signature Pointer to signature.
	 * @param[in] message_digest Pointer to message digest.
	 * @param[in] key Reference to RSA public key.
	 * @return true if the signature is valid, otherwise false.
	 * 
	 * @details
	 * This function verifies a signature for a message digest.
	 * To calculate a message digest use the @ref Sha256Generator class.
	 */
bool VerifyRsa1024PssSha256(const byte_t* signature, const byte_t* message_digest, const RsaKey& key);

	/**
	 * @brief Utility function for calculating a RSA2048-PSS-SHA2-256 signature.
	 * 
	 * @param[out] signature Pointer to the buffer storing the signature.
	 * @param[in]  message_digest Pointer to message digest.
	 * @param[in]  key Reference to RSA private key.
	 * @return true if signature calculation was successful.
	 * 
	 * @pre
	 * - Size of the signature buffer must >= <tt>Rsa2048PssSha256Signer::kSignatureSize</tt>.
	 * 
	 * @post
	 * - The signature is written to <tt><var>signature</var></tt>.
	 * 
	 * @details
	 * This function calculates a signature for a message digest.
	 * To calculate a message digest use the @ref Sha256Generator class.
	 */
bool SignRsa2048PssSha256(byte_t* signature, const byte_t* message_digest, const RsaKey& key);

	/**
	 * @brief Utility function for verfifying a RSA2048-PSS-SHA2-256 signature.
	 * 
	 * @param[in] signature Pointer to signature.
	 * @param[in] message_digest Pointer to message digest.
	 * @param[in] key Reference to RSA public key.
	 * @return true if the signature is valid, otherwise false.
	 * 
	 * @details
	 * This function verifies a signature for a message digest.
	 * To calculate a message digest use the @ref Sha256Generator class.
	 */
bool VerifyRsa2048PssSha256(const byte_t* signature, const byte_t* message_digest, const RsaKey& key);

	/**
	 * @brief Utility function for calculating a RSA4096-PSS-SHA2-256 signature.
	 * 
	 * @param[out] signature Pointer to the buffer storing the signature.
	 * @param[in]  message_digest Pointer to message digest.
	 * @param[in]  key Reference to RSA private key.
	 * @return true if signature calculation was successful.
	 * 
	 * @pre
	 * - Size of the signature buffer must >= <tt>Rsa4096PssSha256Signer::kSignatureSize</tt>.
	 * 
	 * @post
	 * - The signature is written to <tt><var>signature</var></tt>.
	 * 
	 * @details
	 * This function calculates a signature for a message digest.
	 * To calculate a message digest use the @ref Sha256Generator class.
	 */
bool SignRsa4096PssSha256(byte_t* signature, const byte_t* message_digest, const RsaKey& key);

	/**
	 * @brief Utility function for verfifying a RSA4096-PSS-SHA2-256 signature.
	 * 
	 * @param[in] signature Pointer to signature.
	 * @param[in] message_digest Pointer to message digest.
	 * @param[in] key Reference to RSA public key.
	 * @return true if the signature is valid, otherwise false.
	 * 
	 * @details
	 * This function verifies a signature for a message digest.
	 * To calculate a message digest use the @ref Sha256Generator class.
	 */
bool VerifyRsa4096PssSha256(const byte_t* signature, const byte_t* message_digest, const RsaKey& key);

}} // namespace tc::crypto
