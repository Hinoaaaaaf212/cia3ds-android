	/**
	 * @file Pbkdf2Impl.h
	 * @brief Declaration of tc::crypto::detail::Pbkdf2Impl
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/06/06
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/HmacGenerator.h>

#include <tc/crypto/CryptoException.h>

namespace tc { namespace crypto { namespace detail {

	/**
	 * @class Pbkdf2Impl
	 * @brief This class implements Password-Based Key Derivation Function 2 (PBKDF2)
	 * 
	 * @tparam HashFunction The class that implements the hash function used for key derivation.
	 * 
	 * @details
	 * PBKDF2 is a hmac based key derivation function, as defined in RFC 8018.
	 * Applicable hash functions to use with PBKDF2 include.
	 * -# SHA-1
	 * -# SHA-224
	 * -# SHA-256
	 * -# SHA-384
	 * -# SHA-512
	 */
template <typename HashFunction>
class Pbkdf2Impl
{
public:
	static const uint64_t kMaxDerivableSize = uint64_t(0xffffffff) * uint64_t(HashFunction::kHashSize); /**< Maximum total data that can be derived */

	Pbkdf2Impl() :
		mState(State::None),
		mPassword(),
		mSalt(),
		mRoundCount(0),
		mHmac(),
		mAvailableData(0),
		mTotalDataDerived(0),
		mBlockIndex(0)
	{
		std::memset(mDerivedData.data(), 0, mDerivedData.size());
	}

	~Pbkdf2Impl()
	{
		mState = State::None;
		std::memset(mPassword.data(), 0, mPassword.size());
		std::memset(mSalt.data(), 0, mSalt.size());
		std::memset(mDerivedData.data(), 0, mDerivedData.size());
		mRoundCount = 0;
		mBlockIndex = 0;
		mAvailableData = 0;
	}

	void initialize(const byte_t* password, size_t password_size, const byte_t* salt, size_t salt_size, size_t n_rounds)
	{
		if (n_rounds < 1) { throw tc::crypto::CryptoException("tc::crypto::detail::Pbkdf2Impl", "Round count must be >= 1."); }

		mPassword = tc::ByteData(password, password_size);
		mSalt = tc::ByteData(salt, salt_size);
		mRoundCount = n_rounds;
		mBlockIndex = 1;

		mAvailableData = 0;
		mTotalDataDerived = 0;
		
		mState = State::Initialized;
	}

	void getBytes(byte_t* key, size_t key_size)
	{
		if (mState != State::Initialized) return;

		// determine data remaining
		uint64_t derivable_data = kMaxDerivableSize - mTotalDataDerived + mAvailableData;

		if (key_size > derivable_data) { throw tc::crypto::CryptoException("tc::crypto::detail::Pbkdf1Impl", "Request too large."); }

		while (key_size != 0)
		{
			// if there is no availble data then we generate more
			if (mAvailableData == 0)
			{
				deriveBytes();

				// incrementing the block index ensures the next block is (predictably) unique
				mBlockIndex++;

				// update the available digest to maximum
				mAvailableData = mDerivedData.size();

				mTotalDataDerived += mDerivedData.size();
			}

			// determine how much to copy in this loop 
			size_t copy_size = std::min<size_t>(key_size, size_t(std::min<uint64_t>(mAvailableData, std::numeric_limits<size_t>::max())));

			// copy available data into key
			memcpy(key, mDerivedData.data() + mDerivedData.size() - mAvailableData, copy_size);

			// increment key pointer so next loop will copy to the right position
			key += copy_size;

			// decrement key_size so the next loop can track how much data is needed
			key_size -= copy_size;

			// decrement available digest so the next loop can determine where to copy from and generate more digest if needed
			mAvailableData -= copy_size;
		}
	}
private:
	static const size_t kMacSize = HmacGenerator<HashFunction>::kMacSize;

	enum State
	{
		None,
		Initialized
	};

	State mState;

	tc::ByteData mPassword;
	tc::ByteData mSalt;
	size_t mRoundCount;

	HmacGenerator<HashFunction> mHmac;
	std::array<byte_t, kMacSize> mDerivedData;
	uint64_t mAvailableData;
	uint64_t mTotalDataDerived;
	uint32_t mBlockIndex;

	void deriveBytes()
	{
		// Init HMAC with password
		mHmac.initialize(mPassword.data(), mPassword.size());

		// Update HMAC with Salt
		mHmac.update(mSalt.data(), mSalt.size());

		// Update HMAC with BigEndian block index
		tc::bn::be32<uint32_t> be_block_index;
		be_block_index.wrap(mBlockIndex);
		mHmac.update((const byte_t*)&be_block_index, sizeof(tc::bn::be32<uint32_t>));

		// Save MAC to temporary value
		std::array<byte_t, kMacSize> mac;
		mHmac.getMac(mac.data());

		// Also save MAC to derived data
		mHmac.getMac(mDerivedData.data());

		// do HMAC rounds
		for (size_t round = 1; round < mRoundCount; round++)
		{
			// initialize HMAC again from password
			mHmac.initialize(mPassword.data(), mPassword.size());

			// update hmac with old hmac digest
			mHmac.update(mac.data(), mac.size());

			// overwrite old hmac digest with new hmac digest
			mHmac.getMac(mac.data());

			// XOR temp digest with derived data
			for (size_t i = 0; i < kMacSize; i++)
			{
				mDerivedData[i] ^= mac[i];
			}
		}
	}
};

}}} // namespace tc::crypto::detail