	/**
	 * @file RsaPkcs1Signer.h
	 * @brief Declaration of tc::crypto::RsaPkcs1Signer
	 * @author Jack (jakcron)
	 * @version 0.3
	 * @date 2020/09/28
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/detail/RsaImpl.h>
#include <tc/crypto/detail/RsaPkcs1Padding.h>
#include <tc/crypto/RsaKey.h>


namespace tc { namespace crypto {

	/**
	 * @class RsaPkcs1Signer
	 * @brief Class for calculating an RSA-PKCS1 signature.
	 * 
	 * @tparam KeyBitSize RSA key size in bits.
	 * @tparam HashFunction The class that implements the hash function used with RSA-PKCS1.
	 * 
	 * @details
	 * This class is a template class that takes a hash function implementation class as template parameter.
	 * See @ref Rsa2048Pkcs1Sha256Signer or similar for supplied realizations of this template class.
	 * 
	 * The <var>KeyBitSize</var> is the size in bits of the RSA key, this only supports key sizes aligned to 8 bits.
	 * 
	 * The implementation of <var>HashFunction</var> must satisfies the following conditions.
	 * See @ref Sha256Generator or similar class, for more information including parameters to each function.
	 * 
	 * -# Has a <tt>kAsn1OidDataSize</tt> constant that defines the size of the ASN.1 encoded OID for the hash algorithm
	 * -# Has a <tt>kAsn1OidData</tt> constant that defines the ASN.1 encoded OID for the hash algorithm
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
	 * - Initialize RSA Signer state with @ref initialize().
	 * - Sign/Verify message digest with @ref sign() / @ref verify().
	 */
template <size_t KeyBitSize, class HashFunction>
class RsaPkcs1Signer
{
public:
	static_assert((KeyBitSize % 8) == 0, "KeyBitSize must be 8 bit aligned.");

	static const size_t kSignatureSize = KeyBitSize >> 3; /**< RSA-PKCS1 signature size */

		/**
		 * @brief Default constructor.
		 * 
		 * @post
		 * - State is None. @ref initialize() must be called before use.
		 */
	RsaPkcs1Signer() :
		mState(State::None),
		mRsaImpl(),
		mPadImpl()
	{}

		/**
		 * @brief Initializes the signature calculation.
		 * 
		 * @param[in] key RSA key data.
		 * 
		 * @post
		 * - Instance is now in a Initialized state
		 * 
		 * @details
		 * Resets the RSA calculation state with an RSA key.
		 */
	void initialize(const RsaKey& key)
	{
		if (key.n.size() == 0 || (key.d.size() == 0 && key.e.size() == 0))
		{
			throw tc::ArgumentNullException("RsaPkcs1Signer::initialize()", "key does not have minimal required key-data.");
		}

		mRsaImpl.initialize(KeyBitSize, key.n.data(), key.n.size(), nullptr, 0, nullptr, 0, key.d.data(), key.d.size(), key.e.data(), key.e.size());
		
		mState = State::Initialized;
	}
	
		/**
		 * @brief Calculate RSA-PKCS1 signature.
		 * 
		 * @param[out] signature Pointer to the buffer storing the signature.
		 * @param[in]  message_digest Pointer to message digest.
		 * @return true if signature calculation was successful.
		 * 
		 * @pre
		 * - Size of the signature buffer must >= <tt>Rsa1024Pkcs1Sha256Signer::kSignatureSize</tt>.
		 * 
		 * @post
		 * - The signature is written to <tt><var>signature</var></tt>.
		 * 
		 * @details
		 * This function calculates a signature for a message digest.
		 */
	bool sign(byte_t* signature, const byte_t* message_digest)
	{
		if (mState != State::Initialized) { return false; }
		if (signature == nullptr) { return false; }
		if (message_digest == nullptr) { return false; }

		std::array<byte_t, kSignatureSize> block;
		memset(block.data(), 0, block.size());

		if (mPadImpl.BuildPad(block.data(), block.size(), message_digest, HashFunction::kHashSize) != detail::RsaPkcs1Padding<HashFunction>::Result::kSuccess)
		{
			return false;
		} 

		try{
			mRsaImpl.privateTransform(signature, block.data());
		} 
		catch (...)	{
			return false;
		}

		return true;
	}

		/**
		 * @brief Verify RSA-PKCS1 signature.
		 * 
		 * @param[in] signature Pointer to signature.
		 * @param[in] message_digest Pointer to message digest.
		 * @return true if the signature is valid, otherwise false.
		 * 
		 * @details
		 * This function verifies a signature for a message digest.
		 */
	bool verify(const byte_t* signature, const byte_t* message_digest)
	{
		if (mState != State::Initialized) { return false; }
		if (signature == nullptr) { return false; }
		if (message_digest == nullptr) { return false; }

		std::array<byte_t, kSignatureSize> block;
		memcpy(block.data(), signature, block.size());

		try {
			mRsaImpl.publicTransform(block.data(), signature);
		} catch (...) {
			return false;
		}

		return (mPadImpl.CheckPad(message_digest, HashFunction::kHashSize, block.data(), block.size()) == detail::RsaPkcs1Padding<HashFunction>::Result::kSuccess);
	}

private:
	enum class State
	{
		None,
		Initialized
	};

	State mState;
	detail::RsaImpl mRsaImpl;
	detail::RsaPkcs1Padding<HashFunction> mPadImpl;
};

}} // namespace tc::crypto