#include <ntd/n3ds/CciFsSnapshotGenerator.h>
#include <tc/io/SubStream.h>
#include <tc/crypto/Sha256Generator.h>

#include <ntd/n3ds/cci.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <tc/cli/FormatUtil.h>


ntd::n3ds::CciFsShapshotGenerator::CciFsShapshotGenerator(const std::shared_ptr<tc::io::IStream>& stream) :
	FileSystemSnapshot(),
	mBaseStream(stream),
	mCurDir(0)
{
	// validate stream properties
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException("ntd::n3ds::CciFsShapshotGenerator", "Failed to open input stream.");
	}
	if (mBaseStream->canRead() == false || mBaseStream->canSeek() == false)
	{
		throw tc::NotSupportedException("ntd::n3ds::CciFsShapshotGenerator", "Input stream requires read/seek permissions.");
	}

	// validate and read CCI header
	ntd::n3ds::CciHeader hdr;
	if (mBaseStream->length() < sizeof(ntd::n3ds::CciHeader))
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::CciFsShapshotGenerator", "Input stream is too small.");
	}
	mBaseStream->seek(0, tc::io::SeekOrigin::Begin);
	mBaseStream->read((byte_t*)(&hdr), sizeof(ntd::n3ds::CciHeader));

	if (hdr.ncsd_header.struct_magic.unwrap() != hdr.ncsd_header.kStructMagic)
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::CciFsShapshotGenerator", "CCI header had invalid struct magic.");
	}
	if (hdr.ncsd_header.flags.media_type != hdr.ncsd_header.MediaType_Card1 && hdr.ncsd_header.flags.media_type != hdr.ncsd_header.MediaType_Card2)
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::CciFsShapshotGenerator", "CCI header had unexpected media type.");
	}
	if (hdr.ncsd_header.flags.block_size_log != 0)
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::CciFsShapshotGenerator", "CCI header had unexpected block size.");
	}
	if (hdr.ncsd_header.flags.media_platform.test(hdr.ncsd_header.MediaPlatform_CTR) == false)
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::CciFsShapshotGenerator", "CCI header had unexpected supported platforms.");
	}

	// parse header partitions
	struct PartitionInformation
	{
		int64_t offset;
		int64_t size;
		uint64_t title_id;
	};
	
	std::array<PartitionInformation, ntd::n3ds::NcsdCommonHeader::kPartitionNum> partition;

	int64_t used_image_size = 0;
	for (size_t i = 0; i < partition.size(); i++)
	{
		if (hdr.ncsd_header.partition_fs_type[i] != hdr.ncsd_header.PartitionFsType_None)
		{
			throw tc::ArgumentOutOfRangeException("ntd::n3ds::CciFsShapshotGenerator", "CCI partition had unexpected fs type.");
		}
		if (hdr.ncsd_header.partition_crypto_type[i] != hdr.ncsd_header.PartitionCryptoType_None)
		{
			throw tc::ArgumentOutOfRangeException("ntd::n3ds::CciFsShapshotGenerator", "CCI partition had unexpected crypto type.");
		}

		partition[i].offset = int64_t(hdr.ncsd_header.partition_offsetsize[i].blk_offset.unwrap()) << 9;
		partition[i].size = int64_t(hdr.ncsd_header.partition_offsetsize[i].blk_size.unwrap()) << 9;
		partition[i].title_id = hdr.ncsd_header.card_ext.partition_id[i].unwrap();

		used_image_size = std::max<int64_t>((partition[i].offset + partition[i].size), used_image_size);
	}

	if (stream->length() < used_image_size)
	{
		throw tc::ArgumentOutOfRangeException("ntd::n3ds::CciFsShapshotGenerator", "Input stream is too small, given calculated CCI partition geometry.");
	}

	// Add root directory
	dir_entries.push_back(DirEntry());
	mCurDir = dir_entries.size() - 1;
	dir_entries[mCurDir].dir_listing.abs_path = tc::io::Path("/");
	dir_entry_path_map[tc::io::Path("/")] = mCurDir;

	// populate virtual filesystem
	// initial data
	//addFile("initial_data.bin", sizeof(ntd::n3ds::NcsdCommonHeader) + 0xe00, sizeof(ntd::n3ds::CardInfoHeader::InitialData));

	// NCCH partitions
	for (size_t i = 0; i < partition.size(); i++)
	{
		if (partition[i].size != 0)
		{
			std::stringstream ss;
			ss << std::hex << std::setfill('0') << std::setw(2) << i;
			ss << "_";
			ss << std::hex << std::setfill('0') << std::setw(16) << partition[i].title_id;
			ss << ".app";

			addFile(ss.str(), partition[i].offset, partition[i].size);
		}
	}
}

void ntd::n3ds::CciFsShapshotGenerator::addFile(const std::string& name, int64_t offset, int64_t size)
{
	FileEntry tmp;

	tmp.stream = std::make_shared<tc::io::SubStream>(tc::io::SubStream(mBaseStream, offset, size));

	// create file path
	tc::io::Path file_path = dir_entries[mCurDir].dir_listing.abs_path + std::string(name);

	// add file entry to list
	file_entries.push_back(std::move(tmp));

	// add file entry to map
	file_entry_path_map[file_path] = file_entries.size()-1;

	// add name to parent directory listing
	dir_entries[mCurDir].dir_listing.file_list.push_back(name);
}