	/**
	 * @file XtsModeImpl.h
	 * @brief Declaration of tc::crypto::detail::XtsModeImpl
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/07/04
	 **/
#pragma once
#include <tc/types.h>

#include <tc/ArgumentOutOfRangeException.h>
#include <tc/ArgumentNullException.h>

namespace tc { namespace crypto { namespace detail {

	/**
	 * @class XtsModeImpl
	 * @brief This class implements the XTS (<b>X</b>EX mode with cipher<b>t</b>ext <b>s</b>tealing) mode cipher as a template class.
	 * 
	 * @tparam BlockCipher The class that implements the block cipher used for XTS mode encryption/decryption.
	 * 
	 * @details
	 * The implementation of <var>BlockCipher</var> must satisfies the following conditions.
	 * 
	 * -# Has a <tt>kBlockSize</tt> constant that defines the size of the block to process.
	 * -# Has a <tt>kKeySize</tt> constant that defines the required key size to initialize the block cipher.
	 * -# Has an <tt>initialize</tt> method that initializes the state of the block cipher.
	 * -# Has an <tt>encrypt</tt> method that encrypts a block of input data.
	 * -# Has a <tt>decrypt</tt> method that decrypts a block of input data.
	 */
template <class BlockCipher>
class XtsModeImpl
{
public:
	static_assert(BlockCipher::kBlockSize == 16, "XtsModeImpl only supports BlockCiphers with block size 16.");

	static const size_t kKeySize = BlockCipher::kKeySize;
	static const size_t kBlockSize = BlockCipher::kBlockSize;

	size_t sector_size() const { return mSectorSize; }

	XtsModeImpl() :
		mState(None),
		mCryptCipher(),
		mTweakCipher(),
		mSectorSize(0),
		mTweakIsLittleEndian(true)
	{
	}

	void initialize(const byte_t* key1, size_t key1_size, const byte_t* key2, size_t key2_size, size_t sector_size, bool tweak_little_endian = true) 
	{
		if (key1 == nullptr) { throw tc::ArgumentNullException("XtsModeImpl::initialize()", "key1 was null."); }
		if (key1_size != kKeySize) { throw tc::ArgumentOutOfRangeException("XtsModeImpl::initialize()", "key1_size did not equal kKeySize."); }
		if (key2 == nullptr) { throw tc::ArgumentNullException("XtsModeImpl::initialize()", "key2 was null."); }
		if (key2_size != kKeySize) { throw tc::ArgumentOutOfRangeException("XtsModeImpl::initialize()", "key2_size did not equal kKeySize."); }
		if (sector_size < kBlockSize) { throw tc::ArgumentOutOfRangeException("XtsModeImpl::initialize()", "sector_size was less than kBlockSize."); }

		mCryptCipher.initialize(key1, key1_size);
		mTweakCipher.initialize(key2, key2_size);
		mSectorSize = sector_size;
		mTweakIsLittleEndian = tweak_little_endian;
		mState = State::Initialized;
	}

	void encrypt(byte_t* dst, const byte_t* src, size_t size, uint64_t sector_number)
	{
		if (mState != State::Initialized) { return ; }
		if (dst == nullptr) { throw tc::ArgumentNullException("XtsModeImpl::encrypt()", "dst was null."); }
		if (src == nullptr) { throw tc::ArgumentNullException("XtsModeImpl::encrypt()", "src was null."); }
		if (size == 0 || size % mSectorSize) { throw tc::ArgumentOutOfRangeException("XtsModeImpl::encrypt()", "size was not a multiple of the sector size."); }
		
		auto block = std::array<byte_t, kBlockSize>();
		auto dec_tweak = std::array<byte_t, kBlockSize>();
		auto enc_tweak = std::array<byte_t, kBlockSize>();

		// for ciphertext stealing
		size_t sector_leftover = mSectorSize % kBlockSize;
		
		// initialize tweak
		set_tweak(dec_tweak.data(), sector_number);

		// iterate through sectors
		for (size_t sector_idx = 0, sector_num = (size / mSectorSize); sector_idx < sector_num; sector_idx++)
		{
			// encrypt tweak
			mTweakCipher.encrypt(enc_tweak.data(), dec_tweak.data());
			
			// process each block within a sector
			for (size_t block_idx = 0, block_num = (mSectorSize / kBlockSize); block_idx < block_num; block_idx++)
			{
				const byte_t* src_block = src + (sector_idx * mSectorSize) + (block_idx * kBlockSize);
				byte_t* dst_block = dst + (sector_idx * mSectorSize) + (block_idx * kBlockSize);

				// block = src_block XOR enc_tweak
				xor_block(block.data(), enc_tweak.data(), src_block);

				// encrypt block
				mCryptCipher.encrypt(block.data(), block.data());

				// dst_block = enc_block XOR enc_tweak
				xor_block(dst_block, block.data(), enc_tweak.data());

				// Update encrypted tweak
				galois_func(enc_tweak.data());
			}

			// cipher text stealing
			if (sector_leftover > 0)
			{
				size_t block_idx = (mSectorSize / kBlockSize);
				const byte_t* src_block = src + (sector_idx * mSectorSize) + (block_idx * kBlockSize);
				byte_t* prev_dst_block = dst + (sector_idx * mSectorSize) + ((block_idx - 1) * kBlockSize);
				byte_t* dst_block = dst + (sector_idx * mSectorSize) + (block_idx * kBlockSize);

				for (size_t j = 0; j < sector_leftover; j++)
				{
					// block [0, sector_leftover) = src_block [0, sector_leftover) ^ enc_tweak[0, sector_leftover)
					block[j] = src_block[j] ^ enc_tweak[j];

					// dst_block [0, sector_leftover) = prev_dst_block [0, sector_leftover)
					dst_block[j] = prev_dst_block[j];
				}

				for (size_t j = sector_leftover; j < kBlockSize; j++)
				{
					// block [sector_leftover, kBlockSize) = prev_dst_block[sector_leftover, kBlockSize) ^ enc_tweak[sector_leftover, kBlockSize)
					block[j] = prev_dst_block[j] ^ enc_tweak[j];
				}

				// encrypt block
				mCryptCipher.encrypt(block.data(), block.data());

				// prev_dst_block = enc_block XOR enc_tweak
				xor_block(prev_dst_block, block.data(), enc_tweak.data());
			}
			
			// increment tweak
			incr_tweak(dec_tweak.data(), 1);
		}
	}

	void decrypt(byte_t* dst, const byte_t* src, size_t size, uint64_t sector_number)
	{
		if (mState != State::Initialized) { return ; }
		if (dst == nullptr) { throw tc::ArgumentNullException("XtsModeImpl::decrypt()", "dst was null."); }
		if (src == nullptr) { throw tc::ArgumentNullException("XtsModeImpl::decrypt()", "src was null."); }
		if (size == 0 || size % mSectorSize) { throw tc::ArgumentOutOfRangeException("XtsModeImpl::decrypt()", "size was not a multiple of sector_size."); }
	
		auto block = std::array<byte_t, kBlockSize>();
		auto dec_tweak = std::array<byte_t, kBlockSize>();
		auto enc_tweak = std::array<byte_t, kBlockSize>();

		// for ciphertext stealing
		auto prev_tweak = std::array<byte_t, kBlockSize>();
		size_t sector_leftover = mSectorSize % kBlockSize;

		// initialize tweak
		set_tweak(dec_tweak.data(), sector_number);

		// iterate through sectors
		for (size_t sector_idx = 0, sector_num = (size / mSectorSize); sector_idx < sector_num; sector_idx++)
		{
			// encrypt tweak
			mTweakCipher.encrypt(enc_tweak.data(), dec_tweak.data());
			
			// process each block within a sector
			for (size_t block_idx = 0, block_num = (mSectorSize / kBlockSize); block_idx < block_num; block_idx++)
			{
				const byte_t* src_block = src + (sector_idx * mSectorSize) + (block_idx * kBlockSize);
				byte_t* dst_block = dst + (sector_idx * mSectorSize) + (block_idx * kBlockSize);

				// if this is the last block && there is left-over data
				if ((block_idx + 1) == block_num && sector_leftover > 0)
				{
					// save tweak for the cipher text stealing decryption
					memcpy(prev_tweak.data(), enc_tweak.data(), kBlockSize);

					// Update encrypted tweak since this block uses the next tweak due to encryption mode cipher text stealing
					galois_func(enc_tweak.data());
				}

				// block = src_block XOR enc_tweak
				xor_block(block.data(), enc_tweak.data(), src_block);

				// decrypt block
				mCryptCipher.decrypt(block.data(), block.data());

				// dst_block = dec_block XOR enc_tweak
				xor_block(dst_block, block.data(), enc_tweak.data());

				// Update encrypted tweak
				galois_func(enc_tweak.data());
			}

			// cipher text stealing
			if (sector_leftover > 0)
			{
				size_t block_idx = (mSectorSize / kBlockSize);
				const byte_t* src_block = src + (sector_idx * mSectorSize) + (block_idx * kBlockSize);
				byte_t* prev_dst_block = dst + (sector_idx * mSectorSize) + ((block_idx - 1) * kBlockSize);
				byte_t* dst_block = dst + (sector_idx * mSectorSize) + (block_idx * kBlockSize);

				for (size_t j = 0; j < sector_leftover; j++)
				{
					// block [0, sector_leftover) = src_block [0, sector_leftover) ^ prev_tweak[0, sector_leftover)
					block[j] = src_block[j] ^ prev_tweak[j];

					// dst_block [0, sector_leftover) = prev_dst_block [0, sector_leftover)
					dst_block[j] = prev_dst_block[j];
				}

				for (size_t j = sector_leftover; j < kBlockSize; j++)
				{
					// block [sector_leftover, kBlockSize) = prev_dst_block[sector_leftover, kBlockSize) ^ prev_tweak[sector_leftover, kBlockSize)
					block[j] = prev_dst_block[j] ^ prev_tweak[j];
				}

				// encrypt block
				mCryptCipher.decrypt(block.data(), block.data());

				// prev_dst_block = enc_block XOR prev_tweak
				xor_block(prev_dst_block, block.data(), prev_tweak.data());
			}

			// increment tweak
			incr_tweak(dec_tweak.data(), 1);
		}
	}
private:
	enum State
	{
		None,
		Initialized
	};

	State mState;
	BlockCipher mCryptCipher;
	BlockCipher mTweakCipher;
	size_t mSectorSize;
	bool mTweakIsLittleEndian;

	inline void xor_block(byte_t* dst, const byte_t* src_a, const byte_t* src_b)
	{
		((uint64_t*)dst)[0] = ((uint64_t*)src_a)[0] ^ ((uint64_t*)src_b)[0];
		((uint64_t*)dst)[1] = ((uint64_t*)src_a)[1] ^ ((uint64_t*)src_b)[1];
		//for (size_t i = 0; i < kBlockSize; i++) { dst[i] = src_a[i] ^ src_b[i];}
	}

	inline void set_tweak_le(byte_t* tweak, uint64_t sector_number)
	{
		((tc::bn::le64<uint64_t>*)tweak)[0].wrap(sector_number);
		((tc::bn::le64<uint64_t>*)tweak)[1].wrap(0x0);
	}

	inline void set_tweak_be(byte_t* tweak, uint64_t sector_number)
	{
		((tc::bn::be64<uint64_t>*)tweak)[1].wrap(sector_number);
		((tc::bn::be64<uint64_t>*)tweak)[0].wrap(0x0);
	}

	inline void set_tweak(byte_t* tweak, uint64_t sector_number)
	{
		mTweakIsLittleEndian ? set_tweak_le(tweak, sector_number) : set_tweak_be(tweak, sector_number);
	}

	inline void incr_tweak_be(byte_t* tweak, uint64_t incr)
	{
		tc::bn::be64<uint64_t>* tweak_words = (tc::bn::be64<uint64_t>*)tweak;

		uint64_t carry = incr;
		for (size_t i = 0; carry != 0 ; i = ((i + 1) % 2))
		{
			uint64_t word = tweak_words[1 - i].unwrap();
			uint64_t remaining = std::numeric_limits<uint64_t>::max() - word;

			if (remaining > carry)
			{
				tweak_words[1 - i].wrap(word + carry);
				carry = 0;
			}
			else
			{
				tweak_words[1 - i].wrap(carry - remaining - 1);
				carry = 1;
			}
		}
	}

	inline void incr_tweak_le(byte_t* tweak, uint64_t incr)
	{
		tc::bn::le64<uint64_t>* tweak_words = (tc::bn::le64<uint64_t>*)tweak;

		uint64_t carry = incr;
		for (size_t i = 0; carry != 0 ; i = ((i + 1) % 2))
		{
			uint64_t word = tweak_words[i].unwrap();
			uint64_t remaining = std::numeric_limits<uint64_t>::max() - word;

			if (remaining > carry)
			{
				tweak_words[i].wrap(word + carry);
				carry = 0;
			}
			else
			{
				tweak_words[i].wrap(carry - remaining - 1);
				carry = 1;
			}
		}
	}

	inline void incr_tweak(byte_t* tweak, uint64_t incr)
	{
		mTweakIsLittleEndian ? incr_tweak_le(tweak, incr) : incr_tweak_be(tweak, incr);
	}

	inline void galois_func(byte_t* tweak)
	{
		tc::bn::le64<uint64_t>* tweak_u64 = (tc::bn::le64<uint64_t>*)tweak; 

		uint64_t ra = ( tweak_u64[0].unwrap() << 1 )  ^ 0x0087 >> ( 8 - ( ( tweak_u64[1].unwrap() >> 63 ) << 3 ) );
		uint64_t rb = ( tweak_u64[0].unwrap() >> 63 ) | ( tweak_u64[1].unwrap() << 1 );

		tweak_u64[0].wrap(ra);
		tweak_u64[1].wrap(rb);
	}
};

}}} // namespace tc::crypto::detail