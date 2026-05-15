#pragma once
#include <tc/types.h>

namespace ntd { namespace n3ds {

#pragma pack(push,1)

struct CrrHeader
{
	static const uint32_t kStructMagic = tc::bn::make_struct_magic_uint32("CRR0");

	struct Certificate
	{
		tc::bn::le32<uint32_t> unique_id_mask;
		tc::bn::le32<uint32_t> unique_id_pattern;
		tc::bn::pad<0x18> reserved0;
		std::array<byte_t, 0x100> crr_body_public_key;
		std::array<byte_t, 0x100> signature; // PKCS1-RSA2048-SHA2-256 over 0x000-0x11f
	};
	static_assert(sizeof(Certificate) == 0x220, "Certificate had invalid size");

	tc::bn::le32<uint32_t> struct_magic;
	tc::bn::pad<4> reserved0;
	tc::bn::le32<uint32_t> node0; // prev CRR (0 in file, set by RO module when loaded in memory)
	tc::bn::le32<uint32_t> node1; // next CRR (0 in file, set by RO module when loaded in memory)
	tc::bn::le32<int32_t> debug_info_offset;
	tc::bn::le32<int32_t> debug_info_size;
	tc::bn::pad<8> reserved1;
	Certificate body_certificate;
};
static_assert(sizeof(CrrHeader) == 0x240, "CrrHeader had invalid size");

struct CrrBodyHeader
{
	std::array<byte_t, 0x100> signature; // PKCS1-RSA2048-SHA2-256 over 0x000 - end of hashes (according to 3dbrew a fixed size of 0x358 bytes). CRR0 files must be stored under "romfs:/.crr/". The end of the file is aligned to a 0x1000-byte boundary with 0xCC bytes.
	tc::bn::le32<uint32_t> unique_id;
	tc::bn::le32<uint32_t> size;
	tc::bn::pad<8> reserved0;
	tc::bn::le32<uint32_t> hash_offset;
	tc::bn::le32<uint32_t> num_hash;
	tc::bn::le32<uint32_t> module_id_offset;
	tc::bn::le32<uint32_t> module_id_size;
};
static_assert(sizeof(CrrBodyHeader) == 0x120, "CrrBodyHeader had invalid size");

#pragma pack(pop)

}} // namespace ntd::n3ds