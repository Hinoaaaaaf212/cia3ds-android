	/**
	 * @file CbcModeImpl.h
	 * @brief Declaration of tc::crypto::detail::CbcModeImpl
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2020/10/04
	 **/
#pragma once
#include <tc/types.h>

#include <tc/crypto/detail/BlockUtilImpl.h>

#include <tc/ArgumentOutOfRangeException.h>
#include <tc/ArgumentNullException.h>

namespace tc { namespace crypto { namespace detail {

	/**
	 * @class CbcModeImpl
	 * @brief This class implements the CBC (<b>c</b>ipher <b>b</b>lock <b>c</b>haining) mode cipher as a template class.
	 * 
	 * @tparam BlockCipher The class that implements the block cipher used for CBC mode encryption/decryption.
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
class CbcModeImpl
{
public:
	static const size_t kKeySize = BlockCipher::kKeySize;
	static const size_t kBlockSize = BlockCipher::kBlockSize;

	CbcModeImpl() :
		mState(None),
		mCipher()
	{

	}

	void initialize(const byte_t* key, size_t key_size, const byte_t* iv, size_t iv_size) 
	{
		if (key == nullptr) { throw tc::ArgumentNullException("CbcModeImpl::initialize()", "key was null."); }
		if (key_size != kKeySize) { throw tc::ArgumentOutOfRangeException("CbcModeImpl::initialize()", "key_size did not equal kKeySize."); }
		if (iv == nullptr) { throw tc::ArgumentNullException("CbcModeImpl::initialize()", "iv was null."); }
		if (iv_size != kBlockSize) { throw tc::ArgumentOutOfRangeException("CbcModeImpl::initialize()", "iv_size did not equal kBlockSize."); }

		mCipher.initialize(key, key_size);
		memcpy(mIv.data(), iv, mIv.size());
		mState = State::Initialized;
	}

	void update_iv(const byte_t* iv, size_t iv_size)
	{
		if (mState != State::Initialized) { return ; }
		if (iv == nullptr) { throw tc::ArgumentNullException("CbcModeImpl::update_iv()", "iv was null."); }
		if (iv_size != kBlockSize) { throw tc::ArgumentOutOfRangeException("CbcModeImpl::update_iv()", "iv_size did not equal kBlockSize."); }

		memcpy(mIv.data(), iv, mIv.size());
	}

	void encrypt(byte_t* dst, const byte_t* src, size_t size)
	{
		if (mState != State::Initialized) { return ; }
		if (dst == nullptr) { throw tc::ArgumentNullException("CbcModeImpl::encrypt()", "dst was null."); }
		if (src == nullptr) { throw tc::ArgumentNullException("CbcModeImpl::encrypt()", "src was null."); }
		if (size == 0 || size % kBlockSize) { throw tc::ArgumentOutOfRangeException("CbcModeImpl::encrypt()", "size was not a multiple of kBlockSize."); }

		auto block = std::array<byte_t, kBlockSize>();

		// iterate through blocks
		for (size_t block_idx = 0, block_num = (size / kBlockSize); block_idx < block_num; block_idx++)
		{
			// block = src_block ^ iv
			xor_block<kBlockSize>(block.data(), src + (block_idx * kBlockSize), mIv.data());

			// dst_block = encrypt(block)
			mCipher.encrypt(dst + (block_idx * kBlockSize), block.data());

			// iv = dst_block
			memcpy(mIv.data(), dst + (block_idx * kBlockSize), kBlockSize);
		}
	}

	void decrypt(byte_t* dst, const byte_t* src, size_t size)
	{
		if (mState != State::Initialized) { return ; }
		if (dst == nullptr) { throw tc::ArgumentNullException("CbcModeImpl::decrypt()", "dst was null."); }
		if (src == nullptr) { throw tc::ArgumentNullException("CbcModeImpl::decrypt()", "src was null."); }
		if (size == 0 || size % kBlockSize) { throw tc::ArgumentOutOfRangeException("CbcModeImpl::decrypt()", "size was not a multiple of kBlockSize."); }

		auto block = std::array<byte_t, kBlockSize>();
		auto next_iv = std::array<byte_t, kBlockSize>();

		// iterate through blocks
		for (size_t block_idx = 0, block_num = (size / kBlockSize); block_idx < block_num; block_idx++)
		{
			// next_iv = src_block
			memcpy(next_iv.data(), src + (block_idx * kBlockSize), kBlockSize);

			// block = decrypt(src_block)
			mCipher.decrypt(block.data(), src + (block_idx * kBlockSize));

			// dst_block = block ^ iv
			xor_block<kBlockSize>(dst + (block_idx * kBlockSize), block.data(), mIv.data());

			// iv = next_iv
			memcpy(mIv.data(), next_iv.data(), kBlockSize);
		}
	}
private:
	enum State
	{
		None,
		Initialized
	};

	State mState;
	
	BlockCipher mCipher;
	std::array<byte_t, kBlockSize> mIv;
};

}}} // namespace tc::crypto::detail