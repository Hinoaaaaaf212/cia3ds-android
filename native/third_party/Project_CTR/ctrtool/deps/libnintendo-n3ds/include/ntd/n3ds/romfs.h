#pragma once
#include <tc/types.h>

namespace ntd { namespace n3ds {

#pragma pack(push,1)

struct RomFsHeader
{
	static const size_t kSectionNum = 4;
	static const size_t kRomFsDataAlignSize = 0x10;

	struct SectionGeometry
	{
		tc::bn::le32<uint32_t> offset;
		tc::bn::le32<uint32_t> size;
	};

	tc::bn::le32<uint32_t> header_size;
	SectionGeometry        dir_hash_bucket;
	SectionGeometry        dir_entry;
	SectionGeometry        file_hash_bucket;
	SectionGeometry        file_entry;
	tc::bn::le32<uint32_t> data_offset;
};
static_assert(sizeof(RomFsHeader) == 0x28, "RomFsHeader had invalid size");

#ifdef _WIN32
#pragma warning(disable : 4200) // silence warnings for usage of empty arrays in stucts (for name[])
#endif

struct RomFsDirectoryEntry
{
	tc::bn::le32<uint32_t> parent_offset; // parent directory
	tc::bn::le32<uint32_t> sibling_offset; // next sibling directory
	tc::bn::le32<uint32_t> child_offset; // directory child
	tc::bn::le32<uint32_t> file_offset; // file child
	tc::bn::le32<uint32_t> hash_sibling_offset; // hashtable sibling directory
	tc::bn::le32<uint32_t> name_size; // size of name field
	tc::bn::le16<char16_t> name[]; // variable length le-16bit unicode name
};
static_assert(sizeof(RomFsDirectoryEntry) == 0x18, "RomFsDirectoryEntry had invalid size");

struct RomFsFileEntry
{
	tc::bn::le32<uint32_t> parent_offset; // parent directory
	tc::bn::le32<uint32_t> sibling_offset; // next sibling file
	tc::bn::le64<uint64_t> data_offset; // file data offset
	tc::bn::le64<uint64_t> data_size; // file data size
	tc::bn::le32<uint32_t> hash_sibling_offset; // hashtable sibling file
	tc::bn::le32<uint32_t> name_size; // size of name field
	tc::bn::le16<char16_t> name[]; // variable length le-16bit unicode name
};
static_assert(sizeof(RomFsFileEntry) == 0x20, "RomFsFileEntry had invalid size");

#ifdef _WIN32
#pragma warning(default : 4200)
#endif

#pragma pack(pop)

}} // namespace ntd::n3ds