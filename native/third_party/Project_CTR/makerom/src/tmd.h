#pragma once

typedef enum
{
	TYPE_CTR = 0x40,
	TYPE_DATA = 0x8
} tmd_title_type;

typedef enum
{
	content_Encrypted = 0x0001,
	content_Optional = 0x4000,
	content_Shared = 0x8000
} tmd_content_types;

typedef struct
{
	u8 id[4];
	u8 index[2];
	u8 flags[2];
	u8 size[8];
	u8 hash[0x20]; // SHA 256
} tmd_content_chunk;

typedef struct
{
	u8 contentIndexOffset[2];
	u8 contentCommandCount[2];
	u8 contentChunkHash[0x20]; // SHA 256
} tmd_content_info_record;

typedef struct
{
	u8 sigType[4];
	u8 data[0x100];
	u8 padding[0x3C];
} tmd_signature;

typedef struct
{
	u8 issuer[0x40];
	u8 formatVersion;
	u8 caCrlVersion;
	u8 signerCrlVersion;
	u8 padding0;
	u8 systemVersion[8];
	u8 titleID[8];
	u8 titleType[4];
	u8 groupID[2];
	u8 savedataSize[4];
	u8 privSavedataSize[4]; // Zero for CXI Content0
	u8 padding1[4];
	u8 twlFlag; // Zero for CXI Content0
	u8 padding2[0x31];
	u8 accessRights[4];
	u8 titleVersion[2];
	u8 contentCount[2];
	u8 bootContent[2];
	u8 padding3[2];
	u8 infoRecordHash[0x20]; // SHA-256
} tmd_hdr;