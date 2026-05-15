#pragma once

typedef enum
{
	cciflag_BACKUP_WRITE_WAIT_TIME = 0,
	cciflag_FW6_SAVE_CRYPTO = 1,
	cciflag_CARD_DEVICE = 3,
	cciflag_MEDIA_PLATFORM = 4,
	cciflag_MEDIA_TYPE = 5,
	cciflag_MEDIA_BLOCK_SIZE = 6,
	cciflag_CARD_DEVICE_OLD = 7
} cci_flagindex;

typedef enum
{
	carddevice_NOR_FLASH = 1,
	carddevice_NONE = 2,
	carddevice_BT = 3
} cci_carddevice;

typedef enum
{
	cciplatform_CTR = 1,
} cci_platform;

typedef enum
{
	mediatype_INNER_DEVICE, // NAND
	mediatype_CARD1,
	mediatype_CARD2,
	mediatype_EXTENDED_DEVICE
} cci_mediatype;

// Structs
typedef struct
{
	u8 offset[4];
	u8 size[4];
} ncch_offsetsize;

typedef struct
{
	u8 signature[0x100];
	u8 magic[4];
	u8 mediaSize[4];
	u8 titleId[8];
	u8 padding0[0x10];
	ncch_offsetsize offset_sizeTable[8];
	u8 padding1[0x28];
	u8 flags[8];
	u8 ncchIdTable[8][8];
	u8 padding2[0x30];
} cci_hdr;

