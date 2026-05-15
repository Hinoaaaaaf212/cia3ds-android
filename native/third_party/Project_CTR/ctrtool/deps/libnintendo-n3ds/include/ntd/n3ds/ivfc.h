#pragma once
#include <tc/types.h>

namespace ntd { namespace n3ds {

#pragma pack(push,1)

struct IvfcHeader
{
	static const uint32_t kStructMagic = tc::bn::make_struct_magic_uint32("IVFC");

	enum TypeId : uint32_t
	{
		TypeId_A = 0x10000, // CTR RomFS
		TypeId_B = 0x20000, // CTR SaveData
	};

	tc::bn::le32<uint32_t> struct_magic;
	tc::bn::le32<TypeId>   type_id;
};
static_assert(sizeof(IvfcHeader) == 0x8, "IvfcHeader had invalid size");

struct IvfcLevelEntry
{
	tc::bn::le64<uint64_t>    offset;
	tc::bn::le64<uint64_t>    size;
	tc::bn::le32<uint32_t>    block_size_log2;
	tc::bn::pad<4>            reserved;
};
static_assert(sizeof(IvfcLevelEntry) == 0x18, "IvfcLevelEntry had invalid size");

	/**
	 * @struct IvfcCtrRomfsHeader
	 * @details
	 * IVFC for CTR uses 4 levels (1 master, 3 auxillary)
	 * 
	 * * The master level follows the IvfcCtrRomfsHeader aligned to 0x10 bytes (0x60 bytes total), for head.master_hash_size bytes, validating aux level 0
	 * * Aux level 0 validates aux level 1
	 * * Aux level 1 validates aux level 2
	 * * Aux level 2 is RomFS data
	 */ 
struct IvfcCtrRomfsHeader
{
	static const size_t kLevelNum = 3;
	static const size_t kHeaderAlign = 0x10;
	static const size_t kDefaultRomFsBlockSize = 0x1000;

	IvfcHeader                            head;
	tc::bn::le32<uint32_t>                master_hash_size;
	std::array<IvfcLevelEntry, kLevelNum> level;
	tc::bn::le32<uint32_t>                header_size; // header_size == 0x5c
	tc::bn::pad<4>                        reserved;
};
static_assert(sizeof(IvfcCtrRomfsHeader) == 0x5C, "IvfcCtrRomfsHeader had invalid size");

struct IvfcCtrSavedataHeader
{
	static const size_t kLevelNum = 4;

	IvfcHeader                            head;
	tc::bn::le64<uint64_t>                master_hash_size;
	std::array<IvfcLevelEntry, kLevelNum> level;
	tc::bn::le32<uint32_t>                header_size; // header_size == 0x78
	std::array<byte_t, 4>                 reserved;
};
static_assert(sizeof(IvfcCtrSavedataHeader) == 0x78, "IvfcCtrSavedataHeader had invalid size");

#pragma pack(pop)

}} // namespace ntd::n3ds