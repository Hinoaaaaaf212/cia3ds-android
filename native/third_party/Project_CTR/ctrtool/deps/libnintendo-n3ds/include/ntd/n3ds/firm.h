#pragma once
#include <tc/types.h>

namespace ntd { namespace n3ds {

#pragma pack(push,1)

struct FirmwareHeader
{
	static const uint32_t kStructMagic = tc::bn::make_struct_magic_uint32("FIRM");

	struct SectionHeader
	{
		enum CopyMethod
		{
			CopyMethod_NDMA = 0,
			CopyMethod_XDMA = 1,
			CopyMethod_MEMCPY = 2
		};

		tc::bn::le32<uint32_t>   offset;
		tc::bn::le32<uint32_t>   address;
		tc::bn::le32<uint32_t>   size;
		tc::bn::le32<uint32_t>   copy_method;
		std::array<byte_t, 0x20> hash; // SHA2-256 over 0x000+offset - 0x000+offset+size-1
	};

	tc::bn::le32<uint32_t>       struct_magic; // FIRM
	tc::bn::le32<uint32_t>       priority;
	tc::bn::le32<uint32_t>       entrypoint_arm11;
	tc::bn::le32<uint32_t>       entrypoint_arm9;
	tc::bn::pad<0x30>            reserved1;
	std::array<SectionHeader, 4> section;
	std::array<byte_t, 0x100>    signature; // RSA2048-PKCS1-SHA2-256 over 0x000-0x0ff
};
static_assert(sizeof(FirmwareHeader) == 0x200, "FirmwareHeader had invalid size");

#pragma pack(pop)

}} // namespace ntd::n3ds