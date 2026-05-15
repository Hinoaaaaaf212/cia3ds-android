#include <tc/crypto/Aes128CbcEncryptedStream.h>
#include <tc/io/IOUtil.h>
#include <tc/io/StreamUtil.h>

/*
#include <fmt/core.h>
#include <tc/cli/FormatUtil.h>
*/

// inline utils
inline uint64_t castInt64ToUint64(int64_t val) { return val < 0 ? 0 : uint64_t(val); }
inline int64_t castUint64ToInt64(uint64_t val) { return (int64_t)std::min<uint64_t>(val, uint64_t(std::numeric_limits<int64_t>::max())); }

inline uint64_t offsetToBlockIndex(int64_t offset) { return castInt64ToUint64(offset / tc::io::IOUtil::castSizeToInt64(tc::crypto::Aes128CbcEncryptor::kBlockSize)); };
inline int64_t blockIndexToOffset(uint64_t block_index) { return castUint64ToInt64(block_index) * tc::io::IOUtil::castSizeToInt64(tc::crypto::Aes128CbcEncryptor::kBlockSize); };

inline size_t lengthToBlockNum(int64_t length) { return tc::io::IOUtil::castInt64ToSize(length / tc::io::IOUtil::castSizeToInt64(tc::crypto::Aes128CbcEncryptor::kBlockSize)); };
inline size_t offsetInBlock(int64_t offset) { return tc::io::IOUtil::castInt64ToSize(offset % tc::io::IOUtil::castSizeToInt64(tc::crypto::Aes128CbcEncryptor::kBlockSize)); };


const std::string tc::crypto::Aes128CbcEncryptedStream::kClassName = "tc::crypto::Aes128CbcEncryptedStream";

tc::crypto::Aes128CbcEncryptedStream::Aes128CbcEncryptedStream() :
	mBaseStream(),
	mCryptor(std::shared_ptr<tc::crypto::Aes128CbcEncryptor>(new tc::crypto::Aes128CbcEncryptor()))
{
	memset(mBaseIv.data(), 0, mBaseIv.size());
}

tc::crypto::Aes128CbcEncryptedStream::Aes128CbcEncryptedStream(const std::shared_ptr<tc::io::IStream>& stream, const key_t& key, const iv_t& iv) :
	Aes128CbcEncryptedStream()
{
	mBaseStream = stream;

	// validate stream properties
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName, "stream is null.");
	}
	if (mBaseStream->canRead() == false)
	{
		throw tc::NotSupportedException(kClassName, "stream does not support reading.");
	}
	if (mBaseStream->canSeek() == false)
	{
		throw tc::NotSupportedException(kClassName, "stream does not support seeking.");
	}
	if ((mBaseStream->length() % sizeof(block_t)) != 0)
	{
		throw tc::NotSupportedException(kClassName, "stream does is not block aligned.");
	}

	// initialize cryptor
	mCryptor->initialize(key.data(), key.size(), iv.data(), iv.size());
	mBaseIv = iv;
}

bool tc::crypto::Aes128CbcEncryptedStream::canRead() const
{
	return mBaseStream == nullptr ? false : mBaseStream->canRead();
}

bool tc::crypto::Aes128CbcEncryptedStream::canWrite() const
{
	return false; // always false this is a read-only stream
}
bool tc::crypto::Aes128CbcEncryptedStream::canSeek() const
{
	return mBaseStream == nullptr ? false : mBaseStream->canSeek();
}

int64_t tc::crypto::Aes128CbcEncryptedStream::length()
{
	return mBaseStream == nullptr ? 0 : mBaseStream->length();
}

int64_t tc::crypto::Aes128CbcEncryptedStream::position()
{
	return mBaseStream == nullptr ? 0 : mBaseStream->position();
}

size_t tc::crypto::Aes128CbcEncryptedStream::read(byte_t* ptr, size_t count)
{
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::read()", "Failed to read from stream (stream is disposed)");
	}

	// track read_count
	size_t data_read_count = 0;

	// get predicted read count
	count = tc::io::IOUtil::getReadableCount(this->length(), this->position(), count);
	
	// if count is 0 just return
	if (count == 0) return data_read_count;

	// get current position
	int64_t current_pos = mBaseStream->position();
	if (current_pos < 0)
	{
		throw tc::InvalidOperationException(kClassName+"::read()", "Current stream position is negative.");
	}

	// determine begin & end offsets
	int64_t begin_read_offset    = current_pos;
	int64_t end_read_offset      = begin_read_offset + tc::io::IOUtil::castSizeToInt64(count);
	int64_t begin_aligned_offset = begin_read_offset - tc::io::IOUtil::castSizeToInt64(offsetInBlock(begin_read_offset));
	int64_t end_aligned_offset   = end_read_offset - tc::io::IOUtil::castSizeToInt64(offsetInBlock(end_read_offset)) + tc::io::IOUtil::castSizeToInt64(offsetInBlock(end_read_offset) ? sizeof(block_t) : 0x0);
	size_t block_num             = lengthToBlockNum(end_aligned_offset - begin_aligned_offset);

	bool read_partial_begin_block       = false;
	uint64_t partial_begin_block_index  = offsetToBlockIndex(begin_read_offset);
	size_t partial_begin_block_offset   = 0;
	size_t partial_begin_block_size     = sizeof(block_t);

	bool read_partial_end_block         = false;
	uint64_t partial_end_block_index    = offsetToBlockIndex(end_read_offset);
	size_t partial_end_block_offset     = 0;
	size_t partial_end_block_size       = sizeof(block_t);

	if (offsetInBlock(begin_read_offset) != 0)
	{
		read_partial_begin_block   = true;
		partial_begin_block_offset += offsetInBlock(begin_read_offset);
		partial_begin_block_size   -= partial_begin_block_offset;
	}
	if (offsetInBlock(end_read_offset) != 0)
	{
		if (partial_begin_block_index == partial_end_block_index)
		{
			read_partial_begin_block = true;
			partial_begin_block_size -= (sizeof(block_t) - offsetInBlock(end_read_offset));
		}
		else
		{
			read_partial_end_block = true;
			partial_end_block_size = offsetInBlock(end_read_offset);
		}
	}

	size_t continuous_block_num           = block_num - (size_t)read_partial_begin_block - (size_t)read_partial_end_block;
	uint64_t continuous_begin_block_index = (continuous_block_num == 0) ? 0 : (offsetToBlockIndex(begin_aligned_offset) + (uint64_t)read_partial_begin_block);

	/*
	fmt::print("##############################################\n");
	fmt::print("count:                  0x{:x}\n", count);
	fmt::print("begin_read_offset:      0x{:x}\n", begin_read_offset);
	fmt::print("end_read_offset:        0x{:x}\n", end_read_offset);
	fmt::print("begin_aligned_offset:   0x{:x}\n", begin_aligned_offset);
	fmt::print("end_aligned_offset:     0x{:x}\n", end_aligned_offset);
	fmt::print("block_num:              0x{:x}\n", block_num);
	
	fmt::print("partial_begin:\n");
	fmt::print("  read_block:           {}\n", read_partial_begin_block);
	fmt::print("  block_index:          0x{:x}\n", partial_begin_block_index);
	fmt::print("  offset:               0x{:x}\n", partial_begin_block_offset);
	fmt::print("  size:                 0x{:x}\n", partial_begin_block_size);
	
	fmt::print("partial_end:\n");
	fmt::print("  read_block:           {}\n", read_partial_end_block);
	fmt::print("  block_index:          0x{:x}\n", partial_end_block_index);
	fmt::print("  offset:               0x{:x}\n", partial_end_block_offset);
	fmt::print("  size:                 0x{:x}\n", partial_end_block_size);

	fmt::print("continuous:\n");
	fmt::print("  block_index:          0x{:x}\n", continuous_begin_block_index);
	fmt::print("  block_num:            0x{:x}\n", continuous_block_num);
	*/

	if (block_num == 0)
	{
		tc::InvalidOperationException(kClassName+"::read()", "Invalid block number (0 blocks, would have returned before now if count==0)");
	}

	if (block_num < continuous_block_num)
	{
		tc::InvalidOperationException(kClassName+"::read()", "Invalid block number (underflow error)");
	}

	// allocate memory for partial block
	tc::ByteData partial_block = tc::ByteData(sizeof(block_t));

	// read un-aligned begin block
	if (read_partial_begin_block)
	{
		// read iv	
		iv_t iv;
		if (partial_begin_block_index == 0)
		{
			iv = mBaseIv;
		}
		else
		{
			this->seek(blockIndexToOffset(partial_begin_block_index-1), tc::io::SeekOrigin::Begin);
			mBaseStream->read(iv.data(), iv.size());
		}
		mCryptor->update_iv(iv.data(), iv.size());
		
		// read block
		this->seek(blockIndexToOffset(partial_begin_block_index), tc::io::SeekOrigin::Begin);
		mBaseStream->read(partial_block.data(), partial_block.size());
		
		// decrypt block
		mCryptor->decrypt(partial_block.data(), partial_block.data(), partial_block.size());

		// copy out block carving
		memcpy(ptr + data_read_count, partial_block.data() + partial_begin_block_offset, partial_begin_block_size);

		// increment data read count
		data_read_count += partial_begin_block_size;
	}

	// read continous blocks
	if (continuous_block_num > 0)
	{
		// read iv	
		iv_t iv;
		if (continuous_begin_block_index == 0)
		{
			iv = mBaseIv;
		}
		else
		{
			this->seek(blockIndexToOffset(continuous_begin_block_index-1), tc::io::SeekOrigin::Begin);
			mBaseStream->read(iv.data(), iv.size());
		}
		mCryptor->update_iv(iv.data(), iv.size());

		// read blocks
		this->seek(blockIndexToOffset(continuous_begin_block_index), tc::io::SeekOrigin::Begin);
		mBaseStream->read(ptr + data_read_count, continuous_block_num * sizeof(block_t));
		
		// decrypt blocks
		mCryptor->decrypt(ptr + data_read_count, ptr + data_read_count, continuous_block_num * sizeof(block_t));

		// increment data read count
		data_read_count += continuous_block_num * sizeof(block_t);
	}
	
	// read un-aligned end block
	if (read_partial_end_block)
	{
		// read iv	
		iv_t iv;
		if (partial_end_block_index == 0)
		{
			iv = mBaseIv;
		}
		else
		{
			this->seek(blockIndexToOffset(partial_end_block_index-1), tc::io::SeekOrigin::Begin);
			mBaseStream->read(iv.data(), iv.size());
		}
		mCryptor->update_iv(iv.data(), iv.size());

		// read block
		this->seek(blockIndexToOffset(partial_end_block_index), tc::io::SeekOrigin::Begin);
		mBaseStream->read(partial_block.data(), partial_block.size());

		// decrypt block
		mCryptor->decrypt(partial_block.data(), partial_block.data(), partial_block.size());

		// copy out block carving
		memcpy(ptr + data_read_count, partial_block.data() + partial_end_block_offset, partial_end_block_size);

		// increment
		data_read_count += partial_end_block_size;
	}

	// restore expected logical position
	this->seek(begin_read_offset + tc::io::IOUtil::castSizeToInt64(data_read_count), tc::io::SeekOrigin::Begin);

	// return data read count
	return data_read_count;
}

size_t tc::crypto::Aes128CbcEncryptedStream::write(const byte_t* ptr, size_t count)
{
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::write()", "Failed to set stream position (stream is disposed)");
	}

	throw tc::NotImplementedException(kClassName+"::write()", "write is not implemented for Aes128CbcEncryptedStream");
}

int64_t tc::crypto::Aes128CbcEncryptedStream::seek(int64_t offset, tc::io::SeekOrigin origin)
{
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::seek()", "Failed to set stream position (stream is disposed)");
	}

	return mBaseStream->seek(offset, origin);
}

void tc::crypto::Aes128CbcEncryptedStream::setLength(int64_t length)
{
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::setLength()", "Failed to set stream length (stream is disposed)");
	}

	throw tc::NotImplementedException(kClassName+"::setLength()", "setLength is not implemented for Aes128CbcEncryptedStream");
}

void tc::crypto::Aes128CbcEncryptedStream::flush()
{
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::seek()", "Failed to flush stream (stream is disposed)");
	}

	mBaseStream->flush();
}

void tc::crypto::Aes128CbcEncryptedStream::dispose()
{
	if (mBaseStream.get() != nullptr)
	{
		// dispose base stream
		mBaseStream->dispose();

		// release ptr
		mBaseStream.reset();
	}	
}