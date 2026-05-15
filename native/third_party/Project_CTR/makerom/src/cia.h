#pragma once

// Structs
typedef struct
{
	u8 hdrSize[4];
	u8 type[2];
	u8 version[2];
	u8 certChainSize[4];
	u8 tikSize[4];
	u8 tmdSize[4];
	u8 metaSize[4];
	u8 contentSize[8];
	u8 contentIndex[0x2000];
} cia_hdr;

typedef struct
{
	u8 dependencyList[0x30*0x8];
	u8 padding0[0x180];
	u8 coreVersion[4];
	u8 padding1[0xfc];
} cia_metadata;

int CryptContent(u8 *input, u8 *output, u64 size, u8 *title_key, u16 index, u8 mode);

