	/**
	 * @file EcbModeImpl.h
	 * @brief Declaration of tc::crypto::detail::EcbModeImpl
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
	 * @class EcbModeImpl
	 * @brief This class implements the ECB (<b>e</b>lectronic <b>c</b>ode<b>b</b>ook) mode cipher as a template class.
	 * 
	 * @tparam BlockCipher The class that implements the block cipher used for ECB mode encryption/decryption.
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
class EcbModeImpl
{
public:
	static const size_t kKeySize = BlockCipher::kKeySize;
	static const size_t kBlockSize = BlockCipher::kBlockSize;

	EcbModeImpl() :
		mState(None),
		mCipher()
	{

	}

	void initialize(const byte_t* key, size_t key_size) 
	{
		if (key == nullptr) { throw tc::ArgumentNullException("EcbModeImpl::initialize()", "key was null."); }
		if (key_size != kKeySize) { throw tc::ArgumentOutOfRangeException("EcbModeImpl::initialize()", "key_size did not equal kKeySize."); }

		mCipher.initialize(key, key_size);
		mState = State::Initialized;
	}

	void encrypt(byte_t* dst, const byte_t* src, size_t size)
	{
		if (mState != State::Initialized) { return ; }
		if (dst == nullptr) { throw tc::ArgumentNullException("EcbModeImpl::encrypt()", "dst was null."); }
		if (src == nullptr) { throw tc::ArgumentNullException("EcbModeImpl::encrypt()", "src was null."); }
		if (size < kBlockSize) { throw tc::ArgumentOutOfRangeException("EcbModeImpl::encrypt()", "size was less than kBlockSize."); }

		// for ciphertext stealing
		size_t block_leftover = size % kBlockSize;

		// iterate through blocks
		for (size_t block_idx = 0, block_num = (size / kBlockSize); block_idx < block_num; block_idx++)
		{
			mCipher.encrypt(dst + (block_idx * kBlockSize), src + (block_idx * kBlockSize));
		}
		
		// cipher text stealing
		if (block_leftover)
		{
			size_t block_idx = (size / kBlockSize);
			const byte_t* src_block = src + (block_idx * kBlockSize);
			byte_t* prev_dst_block = dst + ((block_idx - 1) * kBlockSize);
			byte_t* dst_block = dst + (block_idx * kBlockSize);

			// part 1 : prep encryption thru cipher text stealing
			std::array<byte_t, kBlockSize> block;

			// block [0, block_leftover) = src_block [0, block_leftover)
			memcpy(block.data(), src_block, block_leftover);

			// block [block_leftover-kBlockSize with previous) = prev_dst_block [block_leftover-kBlockSize)
			memcpy(block.data() + block_leftover, prev_dst_block + block_leftover, kBlockSize - block_leftover);

			// dst_block [0-block_leftover) = prev_dst_block [0-block_leftover)
			memcpy(dst_block, prev_dst_block, block_leftover);

			// part 2 : encrypt block
			mCipher.encrypt(prev_dst_block, block.data());
		}
	}

	void decrypt(byte_t* dst, const byte_t* src, size_t size)
	{
		if (mState != State::Initialized) { return ; }
		if (dst == nullptr) { throw tc::ArgumentNullException("EcbModeImpl::decrypt()", "dst was null."); }
		if (src == nullptr) { throw tc::ArgumentNullException("EcbModeImpl::decrypt()", "src was null."); }
		if (size < kBlockSize) { throw tc::ArgumentOutOfRangeException("EcbModeImpl::decrypt()", "size less than kBlockSize."); }

		// for ciphertext stealing
		size_t block_leftover = size % kBlockSize;

		// iterate through blocks
		for (size_t block_idx = 0, block_num = (size / kBlockSize); block_idx < block_num; block_idx++)
		{
			mCipher.decrypt(dst + (block_idx * kBlockSize), src + (block_idx * kBlockSize));
		}

		// cipher text stealing
		if (block_leftover)
		{
			size_t block_idx = (size / kBlockSize);
			const byte_t* src_block = src + (block_idx * kBlockSize);
			byte_t* prev_dst_block = dst + ((block_idx - 1) * kBlockSize);
			byte_t* dst_block = dst + (block_idx * kBlockSize);

			// part 1 : prep encryption thru cipher text stealing
			std::array<byte_t, kBlockSize> block;

			// block [0, block_leftover) = src_block [0, block_leftover)
			memcpy(block.data(), src_block, block_leftover);

			// block [block_leftover-kBlockSize with previous) = prev_dst_block [block_leftover-kBlockSize)
			memcpy(block.data() + block_leftover, prev_dst_block + block_leftover, kBlockSize - block_leftover);

			// dst_block [0-block_leftover) = prev_dst_block [0-block_leftover)
			memcpy(dst_block, prev_dst_block, block_leftover);

			// part 2 : encrypt block
			mCipher.decrypt(prev_dst_block, block.data());
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
};

}}} // namespace tc::crypto::detail