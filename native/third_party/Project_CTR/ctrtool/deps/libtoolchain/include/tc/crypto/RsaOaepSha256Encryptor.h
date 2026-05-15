	/**
	 * @file RsaOaepSha256Encryptor.h
	 * @brief Declarations for API resources for RSA-OAEP-SHA2-256 calculations.
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/09/28
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/Sha256Generator.h>
#include <tc/crypto/RsaOaepEncryptor.h>

namespace tc { namespace crypto {

	/**
	 * @typedef Rsa1024OaepSha256Encryptor
	 * @brief Class for RSA1024-OAEP-SHA2-256 encryption/decryption.
	 * 
	 * @details This class encrypts/decrypts data using RSA1024-OAEP-SHA2-256.
	 * For more information refer to @ref RsaOaepEncryptor.
	 */
using Rsa1024OaepSha256Encryptor = RsaOaepEncryptor<1024,Sha256Generator>;

	/**
	 * @typedef Rsa2048OaepSha256Encryptor
	 * @brief Class for RSA2048-OAEP-SHA2-256 encryption/decryption.
	 * 
	 * @details This class encrypts/decrypts data using RSA2048-OAEP-SHA2-256.
	 * For more information refer to @ref RsaOaepEncryptor.
	 */
using Rsa2048OaepSha256Encryptor = RsaOaepEncryptor<2048,Sha256Generator>;

	/**
	 * @typedef Rsa4096OaepSha256Encryptor
	 * @brief Class for RSA4096-OAEP-SHA2-256 encryption/decryption.
	 * 
	 * @details This class encrypts/decrypts data using RSA4096-OAEP-SHA2-256.
	 * For more information refer to @ref RsaOaepEncryptor.
	 */
using Rsa4096OaepSha256Encryptor = RsaOaepEncryptor<4096,Sha256Generator>;

	/**
	 * @brief Utility function for encrypting a message using RSA1024-OAEP-SHA2-256.
	 * 
	 * @param[out] block Pointer to the buffer storing the encrypted RSA block.
	 * @param[in]  message Pointer to message.
	 * @param[in]  message_size Size of message.
	 * @param[in]  key RSA key data.
	 * @param[in]  label OAEP label data.
	 * @param[in]  label_size Size in bytes of OAEP label data.
	 * @param[in]  isLabelDigested Boolean indicating if label data has already been digested. False is the default (label is in raw form).
	 * @return true if encryption was successful.
	 * 
	 * @pre
	 * - Size of the @p block buffer must >= <tt>Rsa1024OaepSha256Encryptor::kBlockSize</tt>.
	 * - The maximum size for @p message_size is <tt>Rsa1024OaepSha256Encryptor::kBlockSize</tt> - (2 * <tt>Sha256Generator::kHashSize</tt>) - 2
	 * 
	 * @post
	 * - The encrypted block is written to <tt><var>block</var></tt>.
	 * 
	 * @details
	 * This function encrypts a message using RSA-OAEP, using an RSA public key.
	 * OAEP encoding uses a label (which is digested using a hash algorithm) as part of the MGF1 masking process. It is possible to specify a pre-digested label instead, in which case set @p isLabelDigested to true.
	 * OAEP encoding uses a random seed, this function generates the seed internally. To manually specify the seed, please use the  @ref Rsa1024OaepSha256Encryptor class directly.
	 */
bool EncryptRsa1024OaepSha256(byte_t* block, const byte_t* message, size_t message_size, const RsaKey& key, const byte_t* label, size_t label_size, bool isLabelDigested = false);

	/**
	 * @brief Utility for decrypting a RSA1024-OAEP-SHA2-256 encrypted message.
	 * 
	 * @param[out] message Pointer to the buffer storing the decrypted message.
	 * @param[out] message_size Size of decrypted @p message.
	 * @param[in]  message_capacity Capacity of @p message buffer.
	 * @param[in]  block Pointer to encrypted RSA-OAEP block.
	 * @param[in]  key RSA key data.
	 * @param[in]  label OAEP label data.
	 * @param[in]  label_size Size in bytes of OAEP label data.
	 * @param[in]  isLabelDigested Boolean indicating if label data has already been digested. False is the default (label is in raw form).
	 * @return true if decryption was successful.
	 * 
	 * @pre
	 * - Size of the @p block buffer must >= <tt>Rsa1024OaepSha256Encryptor::kBlockSize</tt>.
	 * - @p message_capacity >= (<tt>Rsa1024OaepSha256Encryptor::kBlockSize</tt> - (2 * <tt>Sha256Generator::kHashSize</tt>) - 2)
	 * 
	 * @post
	 * - The decrypted message is written to <tt><var>message</var></tt>.
	 * - The size of the decrypted message is written to <tt><var>message_size</var></tt>.
	 * 
	 * @details
	 * This function decrypts a RSA-OAEP encrypted message, using an RSA private key.
	 * OAEP encoding uses a label (which is digested using a hash algorithm) as part of the MGF1 masking process. It is possible to specify a pre-digested label instead, in which case set @p isLabelDigested to true.
	 */
bool DecryptRsa1024OaepSha256(byte_t* message, size_t& message_size, size_t message_capacity, const byte_t* block, const RsaKey& key, const byte_t* label, size_t label_size, bool isLabelDigested = false);

	/**
	 * @brief Utility function for encrypting a message using RSA2048-OAEP-SHA2-256.
	 * 
	 * @param[out] block Pointer to the buffer storing the encrypted RSA block.
	 * @param[in]  message Pointer to message.
	 * @param[in]  message_size Size of message.
	 * @param[in]  key RSA key data.
	 * @param[in]  label OAEP label data.
	 * @param[in]  label_size Size in bytes of OAEP label data.
	 * @param[in]  isLabelDigested Boolean indicating if label data has already been digested. False is the default (label is in raw form).
	 * @return true if encryption was successful.
	 * 
	 * @pre
	 * - Size of the @p block buffer must >= <tt>Rsa2048OaepSha256Encryptor::kBlockSize</tt>.
	 * - The maximum size for @p message_size is <tt>Rsa2048OaepSha256Encryptor::kBlockSize</tt> - (2 * <tt>Sha256Generator::kHashSize</tt>) - 2
	 * 
	 * @post
	 * - The encrypted block is written to <tt><var>block</var></tt>.
	 * 
	 * @details
	 * This function encrypts a message using RSA-OAEP, using an RSA public key.
	 * OAEP encoding uses a label (which is digested using a hash algorithm) as part of the MGF1 masking process. It is possible to specify a pre-digested label instead, in which case set @p isLabelDigested to true.
	 * OAEP encoding uses a random seed, this function generates the seed internally. To manually specify the seed, please use the  @ref Rsa2048OaepSha256Encryptor class directly.
	 */
bool EncryptRsa2048OaepSha256(byte_t* block, const byte_t* message, size_t message_size, const RsaKey& key, const byte_t* label, size_t label_size, bool isLabelDigested = false);

	/**
	 * @brief Utility for decrypting a RSA2048-OAEP-SHA2-256 encrypted message.
	 * 
	 * @param[out] message Pointer to the buffer storing the decrypted message.
	 * @param[out] message_size Size of decrypted @p message.
	 * @param[in]  message_capacity Capacity of @p message buffer.
	 * @param[in]  block Pointer to encrypted RSA-OAEP block.
	 * @param[in]  key RSA key data.
	 * @param[in]  label OAEP label data.
	 * @param[in]  label_size Size in bytes of OAEP label data.
	 * @param[in]  isLabelDigested Boolean indicating if label data has already been digested. False is the default (label is in raw form).
	 * @return true if decryption was successful.
	 * 
	 * @pre
	 * - Size of the @p block buffer must >= <tt>Rsa2048OaepSha256Encryptor::kBlockSize</tt>.
	 * - @p message_capacity >= (<tt>Rsa2048OaepSha256Encryptor::kBlockSize</tt> - (2 * <tt>Sha256Generator::kHashSize</tt>) - 2)
	 * 
	 * @post
	 * - The decrypted message is written to <tt><var>message</var></tt>.
	 * - The size of the decrypted message is written to <tt><var>message_size</var></tt>.
	 * 
	 * @details
	 * This function decrypts a RSA-OAEP encrypted message, using an RSA private key.
	 * OAEP encoding uses a label (which is digested using a hash algorithm) as part of the MGF1 masking process. It is possible to specify a pre-digested label instead, in which case set @p isLabelDigested to true.
	 */
bool DecryptRsa2048OaepSha256(byte_t* message, size_t& message_size, size_t message_capacity, const byte_t* block, const RsaKey& key, const byte_t* label, size_t label_size, bool isLabelDigested = false);

	/**
	 * @brief Utility function for encrypting a message using RSA4096-OAEP-SHA2-256.
	 * 
	 * @param[out] block Pointer to the buffer storing the encrypted RSA block.
	 * @param[in]  message Pointer to message.
	 * @param[in]  message_size Size of message.
	 * @param[in]  key RSA key data.
	 * @param[in]  label OAEP label data.
	 * @param[in]  label_size Size in bytes of OAEP label data.
	 * @param[in]  isLabelDigested Boolean indicating if label data has already been digested. False is the default (label is in raw form).
	 * @return true if encryption was successful.
	 * 
	 * @pre
	 * - Size of the @p block buffer must >= <tt>Rsa4096OaepSha256Encryptor::kBlockSize</tt>.
	 * - The maximum size for @p message_size is <tt>Rsa4096OaepSha256Encryptor::kBlockSize</tt> - (2 * <tt>Sha256Generator::kHashSize</tt>) - 2
	 * 
	 * @post
	 * - The encrypted block is written to <tt><var>block</var></tt>.
	 * 
	 * @details
	 * This function encrypts a message using RSA-OAEP, using an RSA public key.
	 * OAEP encoding uses a label (which is digested using a hash algorithm) as part of the MGF1 masking process. It is possible to specify a pre-digested label instead, in which case set @p isLabelDigested to true.
	 * OAEP encoding uses a random seed, this function generates the seed internally. To manually specify the seed, please use the  @ref Rsa4096OaepSha256Encryptor class directly.
	 */
bool EncryptRsa4096OaepSha256(byte_t* block, const byte_t* message, size_t message_size, const RsaKey& key, const byte_t* label, size_t label_size, bool isLabelDigested = false);

	/**
	 * @brief Utility for decrypting a RSA4096-OAEP-SHA2-256 encrypted message.
	 * 
	 * @param[out] message Pointer to the buffer storing the decrypted message.
	 * @param[out] message_size Size of decrypted @p message.
	 * @param[in]  message_capacity Capacity of @p message buffer.
	 * @param[in]  block Pointer to encrypted RSA-OAEP block.
	 * @param[in]  key RSA key data.
	 * @param[in]  label OAEP label data.
	 * @param[in]  label_size Size in bytes of OAEP label data.
	 * @param[in]  isLabelDigested Boolean indicating if label data has already been digested. False is the default (label is in raw form).
	 * @return true if decryption was successful.
	 * 
	 * @pre
	 * - Size of the @p block buffer must >= <tt>Rsa4096OaepSha256Encryptor::kBlockSize</tt>.
	 * - @p message_capacity >= (<tt>Rsa4096OaepSha256Encryptor::kBlockSize</tt> - (2 * <tt>Sha256Generator::kHashSize</tt>) - 2)
	 * 
	 * @post
	 * - The decrypted message is written to <tt><var>message</var></tt>.
	 * - The size of the decrypted message is written to <tt><var>message_size</var></tt>.
	 * 
	 * @details
	 * This function decrypts a RSA-OAEP encrypted message, using an RSA private key.
	 * OAEP encoding uses a label (which is digested using a hash algorithm) as part of the MGF1 masking process. It is possible to specify a pre-digested label instead, in which case set @p isLabelDigested to true.
	 */
bool DecryptRsa4096OaepSha256(byte_t* message, size_t& message_size, size_t message_capacity, const byte_t* block, const RsaKey& key, const byte_t* label, size_t label_size, bool isLabelDigested = false);

}} // namespace tc::crypto
