#include "IvfcProcess.h"
#include <tc/io.h>
#include <tc/cli.h>
#include <tc/crypto.h>
#include <tc/ArgumentNullException.h>

ctrtool::IvfcProcess::IvfcProcess() :
	mModuleLabel("ctrtool::IvfcProcess"),
	mInputStream(),
	mKeyBag(),
	mShowHeaderInfo(false),
	mShowFs(false),
	mVerbose(false),
	mVerify(),
	mExtractPath(),
	mRomFsProcess()
{
	memset((byte_t*)&mHeader, 0, sizeof(ntd::n3ds::IvfcCtrRomfsHeader));
	mMasterHashOffset = 0;
	for (size_t i = 0; i < ntd::n3ds::IvfcCtrRomfsHeader::kLevelNum; i++)
	{
		mActualLevelOffsets[i] = 0;
		mLevelValidation[i] = ValidState::Unchecked;
	}
}

void ctrtool::IvfcProcess::setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream)
{
	mInputStream = input_stream;
}

void ctrtool::IvfcProcess::setKeyBag(const ctrtool::KeyBag& key_bag)
{
	mKeyBag = key_bag;
}

void ctrtool::IvfcProcess::setCliOutputMode(bool show_header_info, bool show_fs)
{
	mShowHeaderInfo = show_header_info;
	mShowFs = show_fs;
}

void ctrtool::IvfcProcess::setVerboseMode(bool verbose)
{
	mVerbose = verbose;
}

void ctrtool::IvfcProcess::setVerifyMode(bool verify)
{
	mVerify = verify;
}

void ctrtool::IvfcProcess::setExtractPath(const tc::io::Path& extract_path)
{
	mExtractPath = extract_path;
}

void ctrtool::IvfcProcess::process()
{
	// begin processing
	processHeader();
	if (mVerify)
		verifyLevels();
	if (mShowHeaderInfo)
		printHeader();
	processRomFs();
}

void ctrtool::IvfcProcess::processHeader()
{
	if (mInputStream == nullptr)
	{
		throw tc::ArgumentNullException(mModuleLabel, "Input stream was null.");
	}
	if (mInputStream->canRead() == false || mInputStream->canSeek() == false)
	{
		throw tc::InvalidOperationException(mModuleLabel, "Input stream requires read/seek permissions.");
	}

	// import header
	mInputStream->seek(0, tc::io::SeekOrigin::Begin);
	mInputStream->read((byte_t*)&mHeader, sizeof(ntd::n3ds::IvfcCtrRomfsHeader));

	// do some simple checks to verify if this is an IVFC header
	if (mHeader.head.struct_magic.unwrap() != mHeader.head.kStructMagic ||
		mHeader.head.type_id.unwrap() != mHeader.head.TypeId_A ||
	    mHeader.header_size.unwrap() != sizeof(ntd::n3ds::IvfcCtrRomfsHeader))
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "IvfcCtrRomfsHeader is corrupted.");
	}

	mMasterHashOffset = align<int64_t>(sizeof(ntd::n3ds::IvfcCtrRomfsHeader), ntd::n3ds::IvfcCtrRomfsHeader::kHeaderAlign);
	mActualLevelOffsets[2] = align<int64_t>(mMasterHashOffset + static_cast<int64_t>(mHeader.master_hash_size.unwrap()), static_cast<int64_t>(1) << static_cast<int64_t>(mHeader.level[1].block_size_log2.unwrap()));
	mActualLevelOffsets[0] = align<int64_t>(mActualLevelOffsets[2] + static_cast<int64_t>(mHeader.level[2].size.unwrap()), static_cast<int64_t>(1) << static_cast<int64_t>(mHeader.level[2].block_size_log2.unwrap()));
	mActualLevelOffsets[1] = align<int64_t>(mActualLevelOffsets[0] + static_cast<int64_t>(mHeader.level[0].size.unwrap()), static_cast<int64_t>(1) << static_cast<int64_t>(mHeader.level[0].block_size_log2.unwrap()));
}

void ctrtool::IvfcProcess::verifyLevels()
{
	size_t blk_num;
	size_t blk_sz;
	int64_t hash_base_offset;
	size_t hash_sz;
	tc::crypto::Sha256Generator hashgen;
	std::array<byte_t, tc::crypto::Sha256Generator::kHashSize> calc_hash;
	for (size_t i = 0; i < mActualLevelOffsets.size(); i++)
	{
		blk_sz = size_t(1) << size_t(mHeader.level[i].block_size_log2.unwrap());
		blk_num = (mHeader.level[i].size.unwrap() / blk_sz) + ((mHeader.level[i].size.unwrap() % blk_sz) ? 1 : 0);
		hash_base_offset = (i == 0) ? mMasterHashOffset : mActualLevelOffsets[i-1];
		hash_sz = blk_num * tc::crypto::Sha256Generator::kHashSize;

		tc::ByteData test_hash = tc::ByteData(hash_sz);
		mInputStream->seek(hash_base_offset, tc::io::SeekOrigin::Begin);
		mInputStream->read(test_hash.data(), test_hash.size());

		tc::ByteData block = tc::ByteData(blk_sz);
		mInputStream->seek(mActualLevelOffsets[i], tc::io::SeekOrigin::Begin);
		size_t bad_blocks = blk_num;
		for (size_t j = 0; j < blk_num; j++)
		{
			mInputStream->read(block.data(), block.size());
			hashgen.initialize();
			hashgen.update(block.data(), block.size());
			hashgen.getHash(calc_hash.data());
			if (memcmp(calc_hash.data(), test_hash.data() + j*tc::crypto::Sha256Generator::kHashSize, tc::crypto::Sha256Generator::kHashSize) != 0)
			{
				if (mVerbose)
				{
					fmt::print(stderr, "[{} ERROR] IVFC Layer {:d}, Block {:d} failed validation.\n", mModuleLabel, i, j);
				}
					
			}
			else
			{
				bad_blocks -= 1;
			}
		}

		mLevelValidation[i] = bad_blocks == 0? ValidState::Good : ValidState::Fail;
		
		if (mLevelValidation[i] != ValidState::Good)
		{
			fmt::print(stderr, "[{} ERROR] IVFC Layer {:d} failed validation.\n", mModuleLabel, i);
		}
	}
}

void ctrtool::IvfcProcess::printHeader()
{
	fmt::print("\n");
	fmt::print("IVFC:\n");
	fmt::print("Header:                 {}\n", "IVFC");
	fmt::print("Id:                     {:08x}\n", mHeader.head.type_id.unwrap());
	fmt::print("Master hash size:       0x{:08x}\n", mHeader.master_hash_size.unwrap());
	fmt::print("Header size:            0x{:08x}\n", mHeader.header_size.unwrap());

	for (size_t i = 0; i < ntd::n3ds::IvfcCtrRomfsHeader::kLevelNum; i++)
	{
		fmt::print("\n");
		fmt::print("Level {:d}: {}\n", i, getValidString(mLevelValidation[i]));
		fmt::print(" Offset:            0x{:08x} (Actual: 0x{:08x})\n", mHeader.level[i].offset.unwrap(), mActualLevelOffsets[i]);
		fmt::print(" Size:              0x{:08x}\n", mHeader.level[i].size.unwrap());
		fmt::print(" BlockSizeLog2:     0x{:08x} (BlockSize: 0x{:08x})\n", mHeader.level[i].block_size_log2.unwrap(), 1 << mHeader.level[i].block_size_log2.unwrap());
	}
}

void ctrtool::IvfcProcess::processRomFs()
{
	std::shared_ptr<ntd::n3ds::IvfcStream> data_layer = std::shared_ptr<ntd::n3ds::IvfcStream>(new ntd::n3ds::IvfcStream(mInputStream));

	mRomFsProcess.setInputStream(data_layer);
	mRomFsProcess.setKeyBag(mKeyBag);
	mRomFsProcess.setCliOutputMode(mShowHeaderInfo, mShowFs);
	mRomFsProcess.setVerboseMode(mVerbose);
	mRomFsProcess.setVerifyMode(mVerify);
	if (mExtractPath.isSet())
		mRomFsProcess.setExtractPath(mExtractPath.get());
	mRomFsProcess.process();
}

std::string ctrtool::IvfcProcess::getValidString(byte_t validstate)
{
	std::string ret_str;
	switch (validstate)
	{
		case ValidState::Unchecked:
			ret_str =  "";
			break;
		case ValidState::Good:
			ret_str =  "(GOOD)";
			break;
		case ValidState::Fail:
		default:
			ret_str =  "(FAIL)";
			break;
	}

	return ret_str;
}