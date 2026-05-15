#pragma once
#include <tc/types.h>

namespace ntd { namespace n3ds {

#pragma pack(push,1)

struct CiaHeader
{
	static const size_t kCiaMaxContentNum = 0x10000;
	static const size_t kCiaSectionAlignment = 64;
	static const size_t kCiaContentAlignment = 16;

	enum Type : uint16_t
	{
		Type_Normal = 0x0
	};

	enum FormatVersion : uint16_t
	{
		FormatVersion_Default = 0x0,
		FormatVersion_SimpleCia = 0xFF,
	};

	tc::bn::le32<uint32_t> header_size;
	tc::bn::le16<uint16_t> type;
	tc::bn::le16<uint16_t> format_version;
	tc::bn::le32<uint32_t> certificate_size;
	tc::bn::le32<uint32_t> ticket_size;
	tc::bn::le32<uint32_t> tmd_size;
	tc::bn::le32<uint32_t> footer_size;
	tc::bn::le64<uint64_t> content_size;
	tc::bn::bitarray<kCiaMaxContentNum/8, true, false> content_bitarray;
	//std::array<byte_t, kCiaMaxContentNum/8> content_bitarray;
};
static_assert(sizeof(CiaHeader) == 0x2020, "CiaHeader had invalid size");

	/**
	 * @struct CiaFooter
	 * @brief This is an optional section of a CIA file that includes "Lot Check" metadata for CTR Titles (Not TWL titles).
	 * 
	 * @details
	 * The CIA footer contains the following metadata
	 * * List of title dependencies (titleids)
	 * * The CoreVersion (titleid lower of the target firmware)
	 * * (Optionally) SystemMenuData blob
	 */
struct CiaFooter
{
	static const size_t kMaxDependencyNum = 48;

	std::array<tc::bn::le64<uint64_t>, kMaxDependencyNum> dependency_list;
	tc::bn::pad<0x180>                                    padding0;
	tc::bn::le32<uint32_t>                                firmware_title_id_lower; // aka "core version"
	tc::bn::pad<0xfc>                                     padding1;
};
static_assert(sizeof(CiaFooter) == 0x400, "CiaFooter had invalid size");

#pragma pack(pop)

}} // namespace ntd::n3ds