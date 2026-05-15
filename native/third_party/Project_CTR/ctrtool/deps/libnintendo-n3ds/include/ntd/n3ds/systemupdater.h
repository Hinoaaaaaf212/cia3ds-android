#pragma once
#include <tc/types.h>

namespace ntd { namespace n3ds {

#pragma pack(push,1)

struct ContentAddr
{
	tc::bn::le32<uint32_t> begin;
	tc::bn::le32<uint32_t> end;
};
static_assert(sizeof(ContentAddr) == 0x8, "ContentAddr had invalid size");

struct ContHeader
{
	static const uint32_t kStructMagic = tc::bn::make_struct_magic_uint32("CONT");
	static const uint32_t kFormatVersion = 4;
	
	// 0x000
	tc::bn::le32<uint32_t> struct_magic; // CONT
	tc::bn::le32<uint32_t> format_version; // 0x4
	tc::bn::pad<0xBF8>     padding_08;
	// 0xC00
	ContentAddr            content_addr[];
};
static_assert(sizeof(ContHeader) == 0xC00, "ContHeader had invalid size");

struct LegacyUpdaterConfig
{
	ContentAddr            unk_addr;
	tc::bn::le64<uint64_t> update_kernel_id;
};
static_assert(sizeof(LegacyUpdaterConfig) == 0x10, "LegacyUpdaterConfig had invalid size");

struct UpdaterConfig
{
	tc::bn::le64<uint64_t> update_kernel_id;
	ContentAddr            updater_info_addr;
	ContentAddr            twl_font_addr;
	ContentAddr            cup_list_addr;
};
static_assert(sizeof(UpdaterConfig) == 0x20, "UpdaterConfig had invalid size");

struct UpdaterInfo
{
	tc::bn::le32<uint32_t> firm_ver_major;
	tc::bn::le32<uint32_t> firm_ver_minor;
	tc::bn::le32<uint32_t> firm_ver_build;
	tc::bn::le32<uint32_t> rel_step;
	tc::bn::le32<uint32_t> firm_revision;
	tc::bn::pad<4>         reserved;
	tc::bn::string<0x20>   patch_info;
	tc::bn::le32<uint32_t> kernel_ver_major;
	tc::bn::le32<uint32_t> kernel_ver_minor;
};
static_assert(sizeof(UpdaterInfo) == 0x40, "UpdaterInfo had invalid size");

struct UpdaterSpec
{
	static const uint32_t kStructMagic = tc::bn::make_struct_magic_uint32("SPEC");
	static const uint32_t kFormatVersion = 0;
	
	enum Region
	{
		Region_JP = 0,
		Region_US = 1,
		Region_EU = 2,
		Region_CN = 4,
		Region_KR = 5,
		Region_TW = 6
	};

	enum SystemMode
	{
		SystemMode_prod = 0,
		SystemMode_dev1 = 2,
		SystemMode_dev2 = 3,
		SystemMode_dev3 = 4
	};

	enum Flag
	{
		Flag_AutoRun = 0,
		Flag_SkipImport = 1,
		Flag_TmpUpdater = 2,
		Flag_ForceChangeSystemMode = 3,

		// SystemUpdater sets this flag at run-time if "rom:/contents/ClearProgram" is present
		Flag_ClearProgramBeforeImport = 27,
		// SystemUpdater sets this flag at run-time if "rom:/contents/ClearTicket" is present
		Flag_ClearTickets = 28,
		// SystemUpdater sets this flag at run-time
		Flag_RebootAfterUpdate = 29,

		// SystemUpdater sets these flags at run-time from key-combos
		Flag_NotIncrementSeed = 30,
		Flag_SdmcLog = 31
	};

	// 0x000
	tc::bn::le32<uint32_t> struct_magic; // SPEC
	tc::bn::le32<uint32_t> format_version; // 0x0
	byte_t                 region;
	byte_t                 system_mode;
	tc::bn::pad<6>         padding;
	tc::bn::bitarray<4>    flags;	
};
static_assert(sizeof(UpdaterSpec) == 0x14, "UpdaterSpec had invalid size");

struct DeleteSpec
{
	enum Flag
	{
		Flag_DeleteNonSystemTitles = 0, // this deletes all titles from NAND without the system app bit, excluding "Self", see below
		Flag_DeleteSelf = 1 // This refers to when the systemupdater was installed to NAND (old install method), the titleID was 0x000400000fd00200
	};

	tc::bn::le32<uint32_t> id_list_length;
	tc::bn::bitarray<4>    flags;
	tc::bn::pad<8>         padding;
	tc::bn::le64<uint64_t> id_list[];
};
static_assert(sizeof(DeleteSpec) == 0x10, "DeleteSpec had invalid size");


#pragma pack(pop)

}} // namespace ntd::n3ds