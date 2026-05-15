#pragma once

typedef struct
{
	u8 magic[4];
	u8 reserved0[4];
	u8 node0[4];
	u8 node1[4];
	u8 debugInfoOffset[4]; //s32
	u8 debugInfoSize[4]; //s32
	u8 reserved1[8];
	u8 uniqueIdMask[4];
	u8 uniqueIdPattern[4];
	u8 reserved2[0x18];
	u8 signPublicKey[0x100];
	u8 signPublicKeySign[0x100];
	u8 sign[0x100];
	u8 uniqueId[4];
	u8 size[4];
	u8 reserved3[8];
	u8 hashOffset[4];
	u8 numHash[4];
	u8 moduleIdOffset[4];
	u8 moduleIdSize[4];
} crr_hdr;