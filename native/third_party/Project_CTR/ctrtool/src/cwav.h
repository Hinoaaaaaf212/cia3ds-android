#pragma once

#include "types.h"

enum CWAV_ENCODING
{
	CWAV_ENCODING_PCM8			= 0,
	CWAV_ENCODING_PCM16			= 1,
	CWAV_ENCODING_DSPADPCM		= 2,
	CWAV_ENCODING_IMAADPCM		= 3,
};

enum CWAV_REFTYPE
{
	CWAV_REFTYPE_DSP_ADPCM_INFO = 0x0300,
	CWAV_REFTYPE_IMA_ADPCM_INFO = 0x0301,
	CWAV_REFTYPE_SAMPLE_DATA    = 0x1F00,
	CWAV_REFTYPE_INFO_BLOCK     = 0x7000,
	CWAV_REFTYPE_DATA_BLOCK     = 0x7001,
	CWAV_REFTYPE_CHANNEL_INFO  = 0x7100,
};

struct cwav_reference
{
	// 0x00
	tc::bn::le16<uint16_t> idtype; // See CWAV_REFTYPE
	// 0x02
	tc::bn::pad<2> padding;
	// 0x04
	tc::bn::le32<uint32_t> offset;
	// 0x08
};

struct cwav_sizedreference
{
	// 0x00
	tc::bn::le16<uint16_t> idtype; // See CWAV_REFTYPE
	// 0x02
	tc::bn::pad<2> padding;
	// 0x04
	tc::bn::le32<uint32_t> offset;
	// 0x08
	tc::bn::le32<uint32_t> size;
	// 0x0C
};

struct cwav_header
{
	static const uint32_t kStructMagic = tc::bn::make_struct_magic_uint32("CWAV");

	// 0x00
	tc::bn::le32<uint32_t> magic;
	// 0x04
	tc::bn::le16<uint16_t> byteordermark; // byte_t[2]{0xFF,0xFE} == little endian, byte_t[2]{0xFE,0xFF} == big endian
	// 0x06
	tc::bn::le16<uint16_t> headersize;
	// 0x08
	tc::bn::le32<uint32_t> version;
	// 0x0C
	tc::bn::le32<uint32_t> totalsize;
	// 0x10
	tc::bn::le16<uint16_t> datablocks;
	// 0x12
	tc::bn::pad<2> reserved;
	// 0x14
	cwav_sizedreference infoblockref;
	// 0x20
	cwav_sizedreference datablockref;
	// 0x2C
};

struct cwav_infoheader
{
	static const uint32_t kStructMagic = tc::bn::make_struct_magic_uint32("INFO");

	// 0x00
	tc::bn::le32<uint32_t> magic;
	// 0x04
	tc::bn::le32<uint32_t> size;
	// 0x08
	byte_t encoding; // See CWAV_ENCODING
	// 0x09
	byte_t looped; // 0 = no loop, 1 = loop
	// 0x0A
	tc::bn::pad<2> padding;
	// 0x0C
	tc::bn::le32<uint32_t> samplerate;
	// 0x10
	tc::bn::le32<uint32_t> loopstart;
	// 0x14
	tc::bn::le32<uint32_t> loopend;
	// 0x18
	tc::bn::pad<4>         reserved;
	// 0x1C
	//tc::bn::le32<uint32_t> channelcount;
	// 0x20
};

struct cwav_referencetable
{
	// 0x00
	tc::bn::le32<uint32_t> ref_count;
	// 0x04
	cwav_reference[] ref_entry;
};

struct cwav_channelinfo
{
	// 0x00
	cwav_reference sampleref;
	// 0x08
	cwav_reference codecref;
	// 0x10
	tc::bn::pad<4> reserved;
	// 0x14
};

struct cwav_dspadpcminfo
{
	// 0x00
	std::array<tc::bn::le16<uint16_t>, 16> coef;
	// 0x20
	tc::bn::le16<uint16_t> scale;
	// 0x22
	tc::bn::le16<uint16_t> yn1;
	// 0x24
	tc::bn::le16<uint16_t> yn2;
	// 0x26
	tc::bn::le16<uint16_t> loopscale;
	// 0x28
	tc::bn::le16<uint16_t> loopyn1;
	// 0x2A
	tc::bn::le16<uint16_t> loopyn2;
	// 0x2C
};

struct cwav_imaadpcminfo
{
	// 0x00
	tc::bn::le16<uint16_t> data;
	// 0x02
	byte_t tableindex;
	// 0x03
	byte_t padding;
	// 0x04
	tc::bn::le16<uint16_t> loopdata;
	// 0x06
	byte_t looptableindex;
	// 0x07
	byte_t looppadding;
	// 0x08
};