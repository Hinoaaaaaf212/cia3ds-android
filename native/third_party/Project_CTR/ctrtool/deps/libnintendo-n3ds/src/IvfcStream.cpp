#include <ntd/n3ds/IvfcStream.h>
#include <tc/io/SubStream.h>
#include <tc/io/IOUtil.h>
#include <tc/io/StreamUtil.h>

#include <ntd/n3ds/ivfc.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <tc/cli/FormatUtil.h>

ntd::n3ds::IvfcStream::IvfcStream() :
	mModuleLabel("ntd::n3ds::IvfcStream"),
	mBaseStream(),
	mDataStreamBlockSize(0),
	mDataStreamLogicalLength(0),
	mDataStream(),
	mHashCache()
{
}

ntd::n3ds::IvfcStream::IvfcStream(const std::shared_ptr<tc::io::IStream>& stream) :
	IvfcStream()
{
	mBaseStream = stream;

	// validate stream properties
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException("ntd::n3ds::IvfcStream", "stream is null.");
	}
	if (mBaseStream->canRead() == false)
	{
		throw tc::InvalidOperationException("ntd::n3ds::IvfcStream", "stream does not support reading.");
	}
	if (mBaseStream->canSeek() == false)
	{
		throw tc::InvalidOperationException("ntd::n3ds::IvfcStream", "stream does not support seeking.");
	}

	// validate and read IVFC header
	if (mBaseStream->length() < sizeof(ntd::n3ds::IvfcCtrRomfsHeader))
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::IvfcStream", "stream is too small.");
	}
	ntd::n3ds::IvfcCtrRomfsHeader hdr;
	mBaseStream->seek(0, tc::io::SeekOrigin::Begin);
	mBaseStream->read((byte_t*)&hdr, sizeof(ntd::n3ds::IvfcCtrRomfsHeader));

	if (hdr.head.struct_magic.unwrap() != ntd::n3ds::IvfcHeader::kStructMagic)
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::IvfcStream", "IVFC header had invalid struct magic.");
	}
	if (hdr.head.type_id.unwrap() != ntd::n3ds::IvfcHeader::TypeId_A)
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::IvfcStream", "IVFC header had unexpected type id.");
	}

	enum LevelIndex
	{
		MasterHash,
		HashLevel0,
		HashLevel1,
		DataLevel2
	};

	// parse level layout
	struct LevelInfo
	{
		int64_t offset;
		int64_t size;
		size_t block_size;
		size_t block_num;
	};
	
	std::array<LevelInfo, 4> section;

	for (size_t i = 0; i < section.size(); i++)
	{
		if (i == MasterHash)
		{
			section[i].offset = int64_t(align<size_t>(sizeof(ntd::n3ds::IvfcCtrRomfsHeader), ntd::n3ds::IvfcCtrRomfsHeader::kHeaderAlign));
			section[i].size = int64_t(hdr.master_hash_size.unwrap());
			section[i].block_size = 0;
			section[i].block_num = 0;
		}
		else
		{
			if (tc::is_uint64_t_too_large_for_int64_t(hdr.level[i-1].offset.unwrap()))
			{
				throw tc::OutOfMemoryException("ntd::n3ds::IvfcStream", "IVFC layer offset too large.");
			}
			if (tc::is_uint64_t_too_large_for_int64_t(hdr.level[i-1].size.unwrap()))
			{
				throw tc::OutOfMemoryException("ntd::n3ds::IvfcStream", "IVFC layer size too large.");
			}
			section[i].offset = int64_t(hdr.level[i-1].offset.unwrap());
			section[i].size = int64_t(hdr.level[i-1].size.unwrap());
			section[i].block_size = size_t(size_t(1) << size_t(hdr.level[i-1].block_size_log2.unwrap()));
			section[i].block_num = size_t(section[i].size / int64_t(section[i].block_size)) + (size_t(section[i].size % int64_t(section[i].block_size)) ? 1 : 0);
		
			/*
			std::cout << "i: " << std::dec << i << std::endl;
			std::cout << "size :           " << std::dec << section[i].size << std::endl;
			std::cout << "block_size_log2: " << std::dec << hdr.level[i-1].block_size_log2.unwrap() << std::endl;
			std::cout << "block_size     : " << std::dec << section[i].block_size << std::endl;
			std::cout << "block_num      : " << std::dec << section[i].block_num << std::endl;
			*/
		}
	}

	// set actual offsets
	section[DataLevel2].offset = align<int64_t>(section[MasterHash].offset + section[MasterHash].size, section[DataLevel2].block_size);
	section[HashLevel0].offset = align<int64_t>(section[DataLevel2].offset + section[DataLevel2].size, section[HashLevel0].block_size);
	section[HashLevel1].offset = align<int64_t>(section[HashLevel0].offset + section[HashLevel0].size, section[HashLevel1].block_size);

	// verify that the hash tables can be read into memory
	if (tc::is_int64_t_too_large_for_size_t(section[MasterHash].size))
	{
		throw tc::OutOfMemoryException("ntd::n3ds::IvfcStream", "IVFC master hash table too large.");
	}
	if (tc::is_int64_t_too_large_for_size_t(section[HashLevel0].size))
	{
		throw tc::OutOfMemoryException("ntd::n3ds::IvfcStream", "IVFC level0 hash table too large.");
	}
	if (tc::is_int64_t_too_large_for_size_t(section[HashLevel1].size))
	{
		throw tc::OutOfMemoryException("ntd::n3ds::IvfcStream", "IVFC level1 hash table too large.");
	}

	// validate hash tree
	if ((section[DataLevel2].block_num * tc::crypto::Sha256Generator::kHashSize) != section[HashLevel1].size)
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::IvfcStream", "IVFC level1 hash table had unexpected size.");
	}
	if ((section[HashLevel1].block_num * tc::crypto::Sha256Generator::kHashSize) != section[HashLevel0].size)
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::IvfcStream", "IVFC level0 hash table had unexpected size.");
	}
	if ((section[HashLevel0].block_num * tc::crypto::Sha256Generator::kHashSize) != section[MasterHash].size)
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::IvfcStream", "IVFC master hash table had unexpected size.");
	}

	auto master_hash_data = tc::ByteData(static_cast<size_t>(section[MasterHash].size));
	auto hash_level0_data = tc::ByteData(align<size_t>(static_cast<size_t>(section[HashLevel0].size), section[HashLevel0].block_size));
	auto hash_level1_data = tc::ByteData(align<size_t>(static_cast<size_t>(section[HashLevel1].size), section[HashLevel1].block_size));

	mBaseStream->seek(section[MasterHash].offset, tc::io::SeekOrigin::Begin);
	mBaseStream->read(master_hash_data.data(), master_hash_data.size());

	mBaseStream->seek(section[HashLevel0].offset, tc::io::SeekOrigin::Begin);
	mBaseStream->read(hash_level0_data.data(), hash_level0_data.size());

	mBaseStream->seek(section[HashLevel1].offset, tc::io::SeekOrigin::Begin);
	mBaseStream->read(hash_level1_data.data(), hash_level1_data.size());

	/*
	std::cout << "Master Hash:" << std::endl;
	std::cout << tc::cli::FormatUtil::formatBytesAsHxdHexString(master_hash_data);

	std::cout << "Level 0:" << std::endl;
	std::cout << tc::cli::FormatUtil::formatBytesAsHxdHexString(hash_level0_data);

	std::cout << "Level 1:" << std::endl;
	std::cout << tc::cli::FormatUtil::formatBytesAsHxdHexString(hash_level1_data);
	*/
	

	// validate level0
	if (validateLayerBlocksWithHashLayer(hash_level0_data.data(), section[HashLevel0].block_size, section[HashLevel0].block_num, master_hash_data.data()) == false)
	{
		throw tc::crypto::CryptoException("ntd::n3ds::IvfcStream", "Hash layer0 failed hash validation.");
	}

	// validate level1
	if (validateLayerBlocksWithHashLayer(hash_level1_data.data(), section[HashLevel1].block_size, section[HashLevel1].block_num, hash_level0_data.data()) == false)
	{
		throw tc::crypto::CryptoException("ntd::n3ds::IvfcStream", "Hash layer0 failed hash validation.");
	}

	// save hash level1 data for data layer
	mHashCache = hash_level1_data;

	// create data layer
	mDataStreamBlockSize = section[DataLevel2].block_size;
	mDataStreamLogicalLength = section[DataLevel2].size;
	mDataStream = std::shared_ptr<tc::io::SubStream>(new tc::io::SubStream(mBaseStream, section[DataLevel2].offset, tc::io::IOUtil::castSizeToInt64(section[DataLevel2].block_num) * tc::io::IOUtil::castSizeToInt64(section[DataLevel2].block_size)));
}

bool ntd::n3ds::IvfcStream::canRead() const
{
	return mDataStream == nullptr ? false : mDataStream->canRead();
}

bool ntd::n3ds::IvfcStream::canWrite() const
{
	return false; // always false this is a read-only stream
}
bool ntd::n3ds::IvfcStream::canSeek() const
{
	return mDataStream == nullptr ? false : mDataStream->canSeek();
}

int64_t ntd::n3ds::IvfcStream::length()
{
	return mDataStream == nullptr ? 0 : mDataStreamLogicalLength;
}

int64_t ntd::n3ds::IvfcStream::position()
{
	return mDataStream == nullptr ? 0 : mDataStream->position();
}

size_t ntd::n3ds::IvfcStream::read(byte_t* ptr, size_t count)
{
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException(mModuleLabel+"::read()", "Failed to read from stream (stream is disposed)");
	}
	
	// track read_count
	size_t data_read_count = 0;

	// get predicted read count
	count = tc::io::IOUtil::getReadableCount(this->length(), this->position(), count);
	
	// if count is 0 just return
	if (count == 0) return data_read_count;

	// determine begin & end offsets
	int64_t begin_read_offset = this->position();
	int64_t end_read_offset   = begin_read_offset + tc::io::IOUtil::castSizeToInt64(count);
	int64_t begin_aligned_offset = begin_read_offset - offsetInBlock(begin_read_offset);
	int64_t end_aligned_offset   = end_read_offset - offsetInBlock(end_read_offset) + (offsetInBlock(end_read_offset) ? mDataStreamBlockSize : 0x0);
	size_t block_num = offsetToBlock(end_aligned_offset - begin_aligned_offset);

	bool read_partial_begin_block     = false;
	size_t partial_begin_block        = offsetToBlock(begin_read_offset);
	size_t partial_begin_block_offset = 0;
	size_t partial_begin_block_size   = mDataStreamBlockSize;

	bool read_partial_end_block     = false;
	size_t partial_end_block        = offsetToBlock(end_read_offset);
	size_t partial_end_block_offset = 0;
	size_t partial_end_block_size   = mDataStreamBlockSize;

	if (offsetInBlock(begin_read_offset) != 0)
	{
		read_partial_begin_block   = true;
		partial_begin_block_offset += offsetInBlock(begin_read_offset);
		partial_begin_block_size   -= partial_begin_block_offset;
	}
	if (offsetInBlock(end_read_offset) != 0)
	{
		if (partial_begin_block == partial_end_block)
		{
			read_partial_begin_block = true;
			partial_begin_block_size -= (mDataStreamBlockSize - offsetInBlock(end_read_offset));
		}
		else
		{
			read_partial_end_block = true;
			partial_end_block_size = offsetInBlock(end_read_offset);
		}
	}

	size_t continuous_block_num   = block_num - (size_t)read_partial_begin_block - (size_t)read_partial_end_block;
	size_t continuous_begin_block = (continuous_block_num == 0) ? 0 : (offsetToBlock(begin_aligned_offset) + (size_t)read_partial_begin_block);

	/*
	std::cout << "##############################################" << std::endl;
	std::cout << "count:                  0x" << std::hex << count << std::endl;
	std::cout << "begin_read_offset:      0x" << std::hex << begin_read_offset << std::endl;
	std::cout << "end_read_offset:        0x" << std::hex << end_read_offset << std::endl;
	std::cout << "begin_aligned_offset:   0x" << std::hex << begin_aligned_offset << std::endl;
	std::cout << "end_aligned_offset:     0x" << std::hex << end_aligned_offset << std::endl;
	std::cout << "block_num:              0x" << std::hex << block_num << std::endl;
	
	std::cout << "partial_begin:" << std::endl;
	std::cout << "  read_block:           " << std::boolalpha << read_partial_begin_block << std::endl;
	std::cout << "  block:                0x" << std::hex << partial_begin_block << std::endl;
	std::cout << "  offset:               0x" << std::hex << partial_begin_block_offset << std::endl;
	std::cout << "  size:                 0x" << std::hex << partial_begin_block_size << std::endl;

	std::cout << "partial_end:" << std::endl;
	std::cout << "  read_block:           " << std::boolalpha << read_partial_end_block << std::endl;
	std::cout << "  block:                0x" << std::hex << partial_end_block << std::endl;
	std::cout << "  offset:               0x" << std::hex << partial_end_block_offset << std::endl;
	std::cout << "  size:                 0x" << std::hex << partial_end_block_size << std::endl;

	std::cout << "continuous:" << std::endl;
	std::cout << "  block:                0x" << std::hex << continuous_begin_block << std::endl;
	std::cout << "  block_num:            0x" << std::hex << continuous_block_num << std::endl;
	*/

	if (block_num == 0)
	{
		tc::InvalidOperationException("ntd::n3ds::IvfcStream", "Invalid block number (0 blocks, would have returned before now if count==0)");
	}

	if (block_num < continuous_block_num)
	{
		tc::InvalidOperationException("ntd::n3ds::IvfcStream", "Invalid block number (underflow error)");
	}

	// allocate memory for partial block
	tc::ByteData partial_block = tc::ByteData(mDataStreamBlockSize);

	// read un-aligned begin block
	if (read_partial_begin_block)
	{	
		// read block
		this->seek(blockToOffset(partial_begin_block), tc::io::SeekOrigin::Begin);
		mDataStream->read(partial_block.data(), partial_block.size());
		
		// verify block
		if (validateLayerBlocksWithHashLayer(partial_block.data(), mDataStreamBlockSize, 1, getBlockHash(partial_begin_block)) == false)
		{
			throw tc::crypto::CryptoException("ntd::n3ds::IvfcStream", "Data layer block(s) failed hash validation.");
		}

		// copy out block carving
		memcpy(ptr + data_read_count, partial_block.data() + partial_begin_block_offset, partial_begin_block_size);

		// increment data read count
		data_read_count += partial_begin_block_size;
	}

	// read continous blocks
	if (continuous_block_num > 0)
	{
		// read blocks
		this->seek(blockToOffset(continuous_begin_block), tc::io::SeekOrigin::Begin);
		mDataStream->read(ptr + data_read_count, continuous_block_num * mDataStreamBlockSize);
		
		// verify blocks
		if (validateLayerBlocksWithHashLayer(ptr + data_read_count, mDataStreamBlockSize, continuous_block_num, getBlockHash(continuous_begin_block)) == false)
		{
			throw tc::crypto::CryptoException("ntd::n3ds::IvfcStream", "Data layer block(s) failed hash validation.");
		}

		// increment data read count
		data_read_count += continuous_block_num * mDataStreamBlockSize;
	}
	
	// read un-aligned end block
	if (read_partial_end_block)
	{
		// read block
		this->seek(blockToOffset(partial_end_block), tc::io::SeekOrigin::Begin);
		mDataStream->read(partial_block.data(), partial_block.size());
		
		// verify block
		if (validateLayerBlocksWithHashLayer(partial_block.data(), mDataStreamBlockSize, 1, getBlockHash(partial_end_block)) == false)
		{
			throw tc::crypto::CryptoException("ntd::n3ds::IvfcStream", "Data layer block(s) failed hash validation.");
		}

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

size_t ntd::n3ds::IvfcStream::write(const byte_t* ptr, size_t count)
{
	throw tc::NotImplementedException(mModuleLabel+"::write()", "write is not supported for IvfcStream");
}

int64_t ntd::n3ds::IvfcStream::seek(int64_t offset, tc::io::SeekOrigin origin)
{
	if (mDataStream == nullptr)
	{
		throw tc::ObjectDisposedException(mModuleLabel+"::seek()", "Failed to set stream position (stream is disposed)");
	}
	
	return mDataStream->seek(offset, origin);
}

void ntd::n3ds::IvfcStream::setLength(int64_t length)
{
	if (mDataStream == nullptr)
	{
		throw tc::ObjectDisposedException(mModuleLabel+"::setLength()", "Failed to set stream length (stream is disposed)");
	}

	throw tc::NotSupportedException(mModuleLabel+"::setLength()", "setLength is not supported for IvfcStream");
}

void ntd::n3ds::IvfcStream::flush()
{
	if (mDataStream == nullptr)
	{
		throw tc::ObjectDisposedException(mModuleLabel+"::seek()", "Failed to flush stream (stream is disposed)");
	}

	mDataStream->flush();
	mBaseStream->flush();
}

void ntd::n3ds::IvfcStream::dispose()
{
	if (mDataStream.get() != nullptr)
	{
		// dispose data stream
		mDataStream->dispose();

		// release ptr
		mDataStream.reset();
	}

	if (mBaseStream.get() != nullptr)
	{
		// dispose base stream
		mBaseStream->dispose();

		// release ptr
		mBaseStream.reset();
	}
	
	// clear hash cache
	mHashCache = tc::ByteData();
}

bool ntd::n3ds::IvfcStream::validateLayerBlocksWithHashLayer(const byte_t* layer, size_t block_size, size_t block_num, const byte_t* hash_layer)
{
	size_t bad_block = block_num;
	for (size_t i = 0; i < block_num; i++)
	{
		const byte_t* blk_ptr = layer + (block_size * i);
		size_t blk_size = block_size;

		const byte_t* blk_hash_ptr = hash_layer + (mHashCalc.kHashSize * i);
		//std::cout << tc::cli::FormatUtil::formatBytesAsHxdHexString(blk_hash_ptr, block_size);

		mHashCalc.initialize();
		mHashCalc.update(blk_ptr, blk_size);
		mHashCalc.getHash(mHash.data());

		//std::cout << "test hash: " << tc::cli::FormatUtil::formatBytesAsString(blk_hash_ptr, 32, true, ":") << std::endl;
		//std::cout << "calc hash: " << tc::cli::FormatUtil::formatBytesAsString(mHash.data(), 32, true, ":") << std::endl;

		// if good hash, reduce bad block count
		if (memcmp(mHash.data(), blk_hash_ptr, mHashCalc.kHashSize) == 0)
		{
			bad_block -= 1;
		}
		else
		{
			//std::cout << "BadBlock:" << std::endl;
			//std::cout << tc::cli::FormatUtil::formatBytesAsHxdHexString(blk_ptr, block_size);
		}
		
	}

	return bad_block == 0;
}