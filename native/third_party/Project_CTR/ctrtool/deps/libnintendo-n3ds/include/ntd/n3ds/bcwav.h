#pragma once
#include <tc/types.h>

namespace ntd { namespace n3ds {

namespace bcwav {

static const uint32_t kCwavMagic = tc::bn::make_struct_magic_uint32("CWAV");
static const uint32_t kCwavVersion = 0x02010000;
static const uint32_t kInfoMagic = tc::bn::make_struct_magic_uint32("INFO");
static const uint32_t kDataMagic = tc::bn::make_struct_magic_uint32("DATA");

enum TypeId
{
	TYPE_ID_DSP_ADPCM = 0x300,
	TYPE_ID_IMA_ADPCM = 0x301,
	TYPE_ID_INFO_BLOCK = 0x7000,
	TYPE_ID_DATA_BLOCK = 0x7001,
	TYPE_ID_CHANNEL_REF = 0x7100,
};

enum Encoding
{
	ENCODING_PCM8 = 0,
	ENCODING_PCM16 = 1,
	ENCODING_DSP_ADPCM = 2,
	ENCODING_IMA_ADPCM = 3
};

enum ChannelIndex
{
	CHANNEL_INDEX_L = 0,
	CHANNEL_INDEX_R = 1,
};

#pragma pack(push,1)

struct Reference
{
	// 0x00
	tc::bn::le16<uint16_t> type_id;
	// 0x02
	tc::bn::pad<2>         padding;
	// 0x04
	tc::bn::le32<uint32_t> offset;
	// 0x08
};

struct ReferenceWithSize : public Reference
{
	// 0x08
	tc::bn::le32<uint32_t> size;
	// 0x0C
};

template <typename T>
struct Table
{
	// 0x00
	tc::bn::le32<uint32_t> count;
	// 0x04
	T                      item[];
};

struct FileHeader
{
	// 0x00
	tc::bn::le32<uint32_t> signature; // 'CWAV'
	// 0x04
	tc::bn::le16<uint16_t> byte_order_mark; // 0xfeff (always little endian)
	// 0x06
	tc::bn::le16<uint16_t> header_size;
	// 0x08
	tc::bn::le32<uint32_t> version; // 2.1.0.0 (0x02010000)
	// 0x0C
	tc::bn::le32<uint32_t> file_size;
	// 0x10
	tc::bn::le16<uint16_t> data_blocks;
	// 0x12
	tc::bn::pad<2>         reserved;
	// 0x14
};

struct BlockInfo
{
	// 0x00
	ReferenceWithSize info_block_reference;
	// 0x0C
	ReferenceWithSize data_block_reference;
	// 0x18
};

struct FileInfo
{
	// 0x00
	FileHeader header;
	// 0x14
	BlockInfo  block_info;
	// 0x2C
};

struct BlockHeader
{
	// 0x00
	tc::bn::le32<uint32_t> kind; // 'INFO' or 'DATA'
	// 0x04
	tc::bn::le32<uint32_t> size;
	// 0x08
};

struct WaveInfo
{
	// 0x00
	byte_t                 encoding; // see Encoding
	// 0x01
	byte_t                 is_loop; // 0: noloop, 1: loop
	// 0x02
	tc::bn::pad<2>         padding;
	// 0x04
	tc::bn::le32<uint32_t> sample_rate;
	// 0x08
	tc::bn::le32<uint32_t> loop_start_frame;
	// 0x0C
	tc::bn::le32<uint32_t> loop_end_frame;
	// 0x10
};

struct InfoBlockBody
{
	// 0x00
	WaveInfo         wave_info;
	// 0x10
	tc::bn::pad<4>   reserved;
	// 0x14
	Table<Reference> channel_info_reference_table;
	// 0x18
};

struct ChannelInfo
{
	// 0x00
	Reference      to_samples;
	// 0x08
	Reference      to_adpcm_info;
	// 0x10
	tc::bn::pad<4> reserved;
	// 0x14
};

struct AdpcmParam
{
	// 0x00
	std::array<tc::bn::le16<uint16_t>, 16> coef;
	// 0x20
};

struct AdpcmContext
{
	// 0x00
	tc::bn::le16<uint16_t> pred_scale; // Stores the predicted value (4bit) and the scale value (4bit) of Adpcm. The upper 8 bits are not referenced
	// 0x02
	tc::bn::le16<int16_t> yn1; // Historical data (1st sample value)
	// 0x04
	tc::bn::le16<int16_t> yn2; // Historical data (2nd sample value)
	// 0x06
};

struct DspAdpcmInfo
{
	// 0x00
	AdpcmParam param;
	// 0x20
	AdpcmContext context;
	// 0x26
	AdpcmContext loop_context;
	// 0x2C
};

struct ImaAdpcmContext
{
	// 0x00
	tc::bn::le16<uint16_t> data;
	// 0x02
	byte_t                 table_index;
	// 0x03
	byte_t                 padding;
	// 0x04
};

struct ImaAdpcmInfo
{
	// 0x00
	ImaAdpcmContext context;
	// 0x04
	ImaAdpcmContext loop_context;
	// 0x08
};

struct InfoBlock
{
	// 0x00
	BlockHeader   header;
	// 0x08
	InfoBlockBody body;
	// 0x20
};

#pragma pack(pop)

} // namespace ntd::n3ds::bcwav

}} // namespace ntd::n3ds