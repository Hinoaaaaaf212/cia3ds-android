#include "RomFsProcess.h"
#include <tc/io.h>
#include <tc/cli.h>
#include <tc/ArgumentNullException.h>

#include <ntd/n3ds/RomFsSnapshotGenerator.h>

#include "CrrProcess.h"

ctrtool::RomFsProcess::RomFsProcess() :
	mModuleLabel("ctrtool::RomFsProcess"),
	mInputStream(),
	mKeyBag(),
	mShowHeaderInfo(false),
	mShowFs(false),
	mVerbose(false),
	mVerify(false),
	mExtractPath(),
	mFsReader(),
	mStaticCrr()
{
	memset((byte_t*)&mHeader, 0, sizeof(ntd::n3ds::RomFsHeader));
}

void ctrtool::RomFsProcess::setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream)
{
	mInputStream = input_stream;
}

void ctrtool::RomFsProcess::setKeyBag(const ctrtool::KeyBag& key_bag)
{
	mKeyBag = key_bag;
}

void ctrtool::RomFsProcess::setCliOutputMode(bool show_header_info, bool show_fs)
{
	mShowHeaderInfo = show_header_info;
	mShowFs = show_fs;
}

void ctrtool::RomFsProcess::setVerboseMode(bool verbose)
{
	mVerbose = verbose;
}

void ctrtool::RomFsProcess::setVerifyMode(bool verify)
{
	mVerify = verify;
}


void ctrtool::RomFsProcess::setExtractPath(const tc::io::Path& extract_path)
{
	mExtractPath = extract_path;
}

void ctrtool::RomFsProcess::process()
{
	// begin processing
	processHeader();
	if (mShowHeaderInfo)
		printHeader();
	if (mStaticCrr != nullptr)
		processCrr();
	if (mShowFs)
		printFs();
	if (mExtractPath.isSet())
		extractFs();
}

void ctrtool::RomFsProcess::processHeader()
{
	if (mInputStream == nullptr)
	{
		throw tc::ArgumentNullException(mModuleLabel, "Input stream was null.");
	}
	if (mInputStream->canRead() == false || mInputStream->canSeek() == false)
	{
		throw tc::InvalidOperationException(mModuleLabel, "Input stream requires read/seek permissions.");
	}

	if (mInputStream->length() < sizeof(ntd::n3ds::RomFsHeader))
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "Input stream is too small.");
	}

	// import header
	mInputStream->seek(0, tc::io::SeekOrigin::Begin);
	mInputStream->read((byte_t*)&mHeader, sizeof(ntd::n3ds::RomFsHeader));

	/*
	std::cout << "mHeader.header_size             : " << mHeader.header_size.unwrap() << std::endl;
	std::cout << "sizeof(ntd::n3ds::RomFsHeader)      : " << sizeof(ntd::n3ds::RomFsHeader) << std::endl;
	std::cout << "mHeader.dir_hash_bucket.offset : " << mHeader.dir_hash_bucket.offset.unwrap() << std::endl;
	std::cout << "mHeader.data_offset             : " << mHeader.data_offset.unwrap() << std::endl;
	std::cout << "expected data offset            : " << align<uint32_t>(mHeader.file_entry.offset.unwrap() + mHeader.file_entry.size.unwrap(), ntd::n3ds::RomFsHeader::kRomFsDataAlignSize) << std::endl;
	*/

	// do some simple checks to verify if this is an ROMFS header
	if (mHeader.header_size.unwrap() != sizeof(ntd::n3ds::RomFsHeader) ||
	    mHeader.dir_hash_bucket.offset.unwrap() != sizeof(ntd::n3ds::RomFsHeader) ||
	    mHeader.data_offset.unwrap() != align<uint32_t>(mHeader.file_entry.offset.unwrap() + mHeader.file_entry.size.unwrap(), ntd::n3ds::RomFsHeader::kRomFsDataAlignSize))
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "RomFsHeader is corrupted.");
	}

	// create FileSystem reader
	mFsReader = std::shared_ptr<tc::io::VirtualFileSystem>(new tc::io::VirtualFileSystem(ntd::n3ds::RomFsSnapshotGenerator(mInputStream)));

	// Open romfs:/.crr/static.crr
	if (mFsReader != nullptr)
	{
		try {
			mFsReader->openFile(tc::io::Path("/.crr/static.crr"), tc::io::FileMode::Open, tc::io::FileAccess::Read, mStaticCrr);
		} catch (const tc::io::FileNotFoundException&) {
			// do nothing
		}
	}
	
}

void ctrtool::RomFsProcess::printHeader()
{
	fmt::print("\n");
	fmt::print("RomFS:\n");
	fmt::print("Header size:            0x{:08x}\n", mHeader.header_size.unwrap());
	fmt::print("DirHashBucket offset:   0x{:08x}\n", mHeader.dir_hash_bucket.offset.unwrap());
	fmt::print("DirHashBucket size:     0x{:08x}\n", mHeader.dir_hash_bucket.size.unwrap());
	fmt::print("DirEntryTable offset:   0x{:08x}\n", mHeader.dir_entry.offset.unwrap());
	fmt::print("DirEntryTable size:     0x{:08x}\n", mHeader.dir_entry.size.unwrap());
	fmt::print("FileHashBucket offset:  0x{:08x}\n", mHeader.file_hash_bucket.offset.unwrap());
	fmt::print("FileHashBucket size:    0x{:08x}\n", mHeader.file_hash_bucket.size.unwrap());
	fmt::print("FileEntryTable offset:  0x{:08x}\n", mHeader.file_entry.offset.unwrap());
	fmt::print("FileEntryTable size:    0x{:08x}\n", mHeader.file_entry.size.unwrap());
	fmt::print("Data offset:            0x{:08x}\n", mHeader.data_offset.unwrap());
}

void ctrtool::RomFsProcess::processCrr()
{
	CrrProcess crr_proc;

	crr_proc.setInputStream(mStaticCrr);
	crr_proc.setKeyBag(mKeyBag);
	crr_proc.setCliOutputMode(mShowHeaderInfo);
	crr_proc.setVerboseMode(mVerbose);
	crr_proc.setVerifyMode(mVerify);

	crr_proc.process();
}

void ctrtool::RomFsProcess::printFs()
{
	fmt::print("[RomFs Filesystem]\n");
	visitDir(tc::io::Path("/"),tc::io::Path("/"), false, true);
}

void ctrtool::RomFsProcess::extractFs()
{
	visitDir(tc::io::Path("/"), mExtractPath.get(), true, false);
}

void ctrtool::RomFsProcess::visitDir(const tc::io::Path& v_path, const tc::io::Path& l_path, bool extract_fs, bool print_fs)
{
	tc::io::LocalFileSystem local_fs;

	// get listing for directory
	tc::io::sDirectoryListing info;
	mFsReader->getDirectoryListing(v_path, info);

	if (print_fs)
	{
		for (size_t i = 0; i < v_path.size(); i++)
			fmt::print(" ");

		fmt::print("{}/\n", (v_path.size() == 1) ? "RomFs:" : v_path.back());
	}
	if (extract_fs)
	{
		// create local dir
		local_fs.createDirectory(l_path);
	}

	// iterate thru child files
	tc::ByteData cache = tc::ByteData(0x10000);
	size_t cache_read_len;
	tc::io::Path out_path;
	std::shared_ptr<tc::io::IStream> in_stream;
	std::shared_ptr<tc::io::IStream> out_stream;
	for (auto itr = info.file_list.begin(); itr != info.file_list.end(); itr++)
	{
		if (print_fs)
		{
			for (size_t i = 0; i < v_path.size(); i++)
				fmt::print(" ");

			fmt::print(" {}\n", *itr);
		}
		if (extract_fs)
		{
			// build out path
			out_path = l_path + *itr;

			if (mVerbose)
			{
				fmt::print(stderr, "[{} LOG] Saving {}...\n", mModuleLabel, out_path.to_string());
			}

			// begin export
			mFsReader->openFile(v_path + *itr, tc::io::FileMode::Open, tc::io::FileAccess::Read, in_stream);
			local_fs.openFile(out_path, tc::io::FileMode::OpenOrCreate, tc::io::FileAccess::Write, out_stream);

			in_stream->seek(0, tc::io::SeekOrigin::Begin);
			out_stream->seek(0, tc::io::SeekOrigin::Begin);
			for (int64_t remaining_data = in_stream->length(); remaining_data > 0;)
			{
				cache_read_len = in_stream->read(cache.data(), cache.size());
				if (cache_read_len == 0)
				{
					throw tc::io::IOException(mModuleLabel, "Failed to read from RomFs file.");
				}

				out_stream->write(cache.data(), cache_read_len);

				remaining_data -= int64_t(cache_read_len);
			}
		}
	}

	// iterate thru child dirs
	for (auto itr = info.dir_list.begin(); itr != info.dir_list.end(); itr++)
	{
		visitDir(v_path + *itr, l_path + *itr, extract_fs, print_fs);
	}
}