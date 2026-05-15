	/**
	 * @file RsaOaepEncryptor.h
	 * @brief Declaration of tc::crypto::RsaOaepEncryptor
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/09/28
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/detail/RsaImpl.h>
#include <tc/crypto/detail/PrbgImpl.h>
#include <tc/crypto/detail/RsaOaepPadding.h>
#include <tc/crypto/RsaKey.h>


namespace tc { namespace crypto {

	/**
	 * @class RsaOaepEncryptor
	 * @brief Class for RSA-OAEP encryption/decryption.
	 * 
	 * @tparam KeyBitSize RSA key size in bits.
	 * @tparam HashFunction The class that implements the hash function used with RSA-OAEP.
	 * 
	 * @details
	 * This class is a template class that takes a key size and a hash function implementation class as template parameter.
	 * See @ref Rsa2048OaepSha256Encryptor or similar for supplied realizations of this template class.
	 * 
	 * The <var>KeyBitSize</var> is the size in bits of the RSA key, this only supports key sizes aligned to 8 bits.
	 * 
	 * The implementation of <var>HashFunction</var> must satisfies the following conditions.
	 * See @ref Sha256Generator or similar class, for more information including parameters to each function.
	 * 
	 * -# Has a <tt>kBlockSize</tt> constant that defines the size of the block to process.
	 * -# Has a <tt>kHashSize</tt> constant that defines the output size of the hash value.
	 * -# Has an <tt>initialize</tt> method that begins processing.
	 * -# Has an <tt>update</tt> method that updates the hash value on input.
	 * -# Has a <tt>getHash</tt> method that gets the final hash value.
	 * 
	 * This class has two states:
	 * - None : Not ready
	 * - Initialized : Ready to process input data
	 * 
	 * General usage of this class is as follows:
	 * - Initialize RSA-OAEP Encryptor state with @ref initialize().
	 * - Encrypt/Decrypt message with @ref encrypt() / @ref decrypt().
	 */
template <size_t KeyBitSize, class HashFunction>
class RsaOaepEncryptor
{
public:
	static_assert((KeyBitSize % 8) == 0, "KeyBitSize must be 8 bit aligned.");

	static const size_t kBlockSize = KeyBitSize >> 3; /**< RSA-OAEP block size */

		/**
		 * @brief Default constructor.
		 * 
		 * @post
		 * - State is None. @ref initialize() must be called before use.
		 */
	RsaOaepEncryptor() :
		mState(State::None),
		mLabelDigest(HashFunction::kHashSize),
		mRsaImpl(),
		mPrbgImpl(),
		mPadImpl()
	{}

		/**
		 * @brief Initializes the encryption state.
		 * 
		 * @param[in] key RSA key data.
		 * @param[in] label OAEP label data.
		 * @param[in] label_size Size in bytes of OAEP label data.
		 * @param[in] isLabelDigested Boolean indicating if label data has already been digested. False is the default (label is in raw form).
		 * 
		 * @post
		 * - Instance is now in a Initialized state
		 * 
		 * @details
		 * Resets the RSA calculation state with an RSA key.
		 * OAEP encoding uses a label (which is digested using a hash algorithm) as part of the MGF1 masking process. It is possible to specify a pre-digested label instead, in which case set @p isLabelDigested to true.
		 */
	void initialize(const RsaKey& key, const byte_t* label, size_t label_size, bool isLabelDigested = false)
	{
		if (key.n.size() == 0 || (key.d.size() == 0 && key.e.size() == 0))
		{
			throw tc::ArgumentNullException("RsaOaepEncryptor::initialize()", "key does not have minimal required key-data.");
		}

		mRsaImpl.initialize(KeyBitSize, key.n.data(), key.n.size(), nullptr, 0, nullptr, 0, key.d.data(), key.d.size(), key.e.data(), key.e.size());

		if (((label == nullptr) ^ (label_size == 0)))
		{
			throw tc::ArgumentNullException("RsaOaepEncryptor::initialize()", "label was null when label_size was non-zero or vice-versa.");
		}

		if (isLabelDigested == true)
		{
			if (label_size != HashFunction::kHashSize)
				throw tc::ArgumentOutOfRangeException("RsaOaepEncryptor::initialize()", "predigested label must be the size of HashFunction::kHashSize.");

			memcpy(mLabelDigest.data(), label, mLabelDigest.size());
		}
		else
		{
			HashFunction hash_impl;
			hash_impl.initialize();
			hash_impl.update(label, label_size);
			hash_impl.getHash(mLabelDigest.data());
		}
		

		mState = State::Initialized;
	}
	
		/**
		 * @brief Encode and encrypt a message into an RSA-OAEP block.
		 * 
		 * @param[out] block Pointer to the buffer storing the encrypted RSA block.
		 * @param[in]  message Pointer to message.
		 * @param[in]  message_size Size of message.
		 * @return true if encryption was successful.
		 * 
		 * @pre
		 * - Size of the @p block buffer must >= <tt>RsaOaepEncryptor::kBlockSize</tt>.
		 * - The maximum size for @p message_size is <tt>RsaOaepEncryptor::kBlockSize</tt> - (2 * <tt>HashFunction::kHashSize</tt>) - 2.
		 * 
		 * @post
		 * - The encrypted block is written to <tt><var>block</var></tt>.
		 * 
		 * @details
		 * This method encrypts a message using RSA-OAEP, using an RSA public key.
		 * OAEP encoding uses a random seed, this overload of @ref encrypt() generates the seed internally. To manually specify the seed, please use the alternative @ref encrypt() method.
		 */
	bool encrypt(byte_t* block, const byte_t* message, size_t message_size)
	{
		if (mState != State::Initialized) { return false; }

		std::array<byte_t, HashFunction::kHashSize> seed;
		mPrbgImpl.getBytes(seed.data(), seed.size());
		
		return encrypt(block, message, message_size, seed.data(), seed.size());
	}

		/**
		 * @brief Encode and encrypt a message into an RSA-OAEP block.
		 * 
		 * @param[out] block Pointer to the buffer storing the encrypted RSA block.
		 * @param[in]  message Pointer to message.
		 * @param[in]  message_size Size of message.
		 * @param[in]  seed Pointer to random seed.
		 * @param[in]  seed_size Size of random seed.
		 * @return true if encryption was successful.
		 * 
		 * @pre
		 * - Size of the @p block buffer must >= <tt>RsaOaepEncryptor::kBlockSize</tt>.
		 * - The maximum size for @p message_size is <tt>RsaOaepEncryptor::kBlockSize</tt> - (2 * <tt>HashFunction::kHashSize</tt>) - 2.
		 * - Size of the @p seed buffer must be == <tt>HashFunction::kHashSize</tt>.
		 * - The seed should be random or the security of the encryption is reduced.
		 * 
		 * @post
		 * - The encrypted block is written to <tt><var>block</var></tt>.
		 * 
		 * @details
		 * This method encrypts a message using RSA-OAEP, using an RSA public key.
		 */
	bool encrypt(byte_t* block, const byte_t* message, size_t message_size, const byte_t* seed, size_t seed_size)
	{
		if (mState != State::Initialized) { return false; }
		if (message_size > (kBlockSize - (2 * HashFunction::kBlockSize) - 2)) { return false;  }
		if (block == nullptr) { return false; }
		if (message == nullptr || message_size == 0) { return false; }
		if (seed == nullptr || seed_size == 0) { return false; }

		std::array<byte_t, kBlockSize> encoded_message;
		//memset(encoded_message.data(), 0, encoded_message.size());

		if (mPadImpl.BuildPad(encoded_message.data(), encoded_message.size(), mLabelDigest.data(), mLabelDigest.size(), message, message_size, seed, seed_size) != detail::RsaOaepPadding<HashFunction>::Result::kSuccess)
		{
			return false;
		} 

		try {
			mRsaImpl.publicTransform(block, encoded_message.data());
		} 
		catch (...)	{
			return false;
		}

		return true;
	}

		/**
		 * @brief Decrypt & decode message from an RSA-OAEP block.
		 * 
		 * @param[out] message Pointer to the buffer storing the decrypted message.
		 * @param[out] message_size Size of decrypted @p message.
		 * @param[in]  message_capacity Capacity of @p message buffer.
		 * @param[in]  block Pointer to encrypted RSA-OAEP block.
		 * @return true if decryption was successful.
		 * 
		 * @pre
		 * - Size of the @p block buffer must >= <tt>RsaOaepEncryptor::kBlockSize</tt>.
		 * - @p message_capacity >= (<tt>RsaOaepEncryptor::kBlockSize</tt> - (2 * <tt>HashFunction::kHashSize</tt>) - 2)
		 * 
		 * @post
		 * - The decrypted message is written to <tt><var>message</var></tt>.
		 * - The size of the decrypted message is written to <tt><var>message_size</var></tt>.
		 * 
		 * @details
		 * This method decrypts a RSA-OAEP encrypted message, using an RSA private key.
		 */
	bool decrypt(byte_t* message, size_t& message_size, size_t message_capacity, const byte_t* block)
	{
		if (mState != State::Initialized) { return false; }
		if (block == nullptr) { return false; }
		if (message == nullptr || message_capacity == 0) { return false; }

		std::array<byte_t, kBlockSize> decrypted_block;

		try {
			mRsaImpl.privateTransform(decrypted_block.data(), block);
		} catch (...) {
			return false;
		}
				
		return (mPadImpl.RecoverFromPad(message, message_capacity, message_size, mLabelDigest.data(), mLabelDigest.size(), decrypted_block.data(), decrypted_block.size()) == detail::RsaOaepPadding<HashFunction>::Result::kSuccess);
	}

private:
	enum class State
	{
		None,
		Initialized
	};

	State mState;
	tc::ByteData mLabelDigest;
	detail::RsaImpl mRsaImpl;
	detail::PrbgImpl mPrbgImpl;
	detail::RsaOaepPadding<HashFunction> mPadImpl;
};

}} // namespace tc::crypto