#pragma once

typedef enum
{
	lic_Permanent = 0,
	lic_Demo = 1,
	lic_Trial = 2,
	lic_Rental = 3,
	lic_Subscription = 4,
	lic_Service = 5,
	lic_Mask = 15
} tik_license_type;

typedef enum
{
	right_Permanent = 1,
	right_Subscription = 2,
	right_Content = 3,
	right_ContentConsumption = 4,
	right_AccessTitle = 5
} tik_item_rights;

typedef struct
{
	u8 unk0[4];
	u8 totalSize[4];
	u8 unk1[4];
	u8 unk2[4];
	u8 unk3[4];
	u8 hdrSize[4];
	u8 segNum[4];
	u8 segSize[4];
	u8 segTotalSize[4];
	u8 unk4[4];
} tik_content_index_hdr;

typedef struct
{
    u8 level[4];
	u8 index[0x80];
} tik_content_index_struct;

typedef struct
{
	u8 sigType[4];
	u8 data[0x100];
	u8 padding[0x3C];
} tik_signature;

typedef struct
{
	u8 issuer[0x40];
	u8 eccPubKey[0x3c];
	u8 formatVersion;
	u8 caCrlVersion;
	u8 signerCrlVersion;
	u8 encryptedTitleKey[0x10];
	u8 padding0;
	u8 ticketId[8];
	u8 deviceId[4];
	u8 titleId[8];
	u8 padding1[2];
	u8 ticketVersion[2];
	u8 padding2[8];
	u8 licenceType;
	u8 keyId;
	u8 propertyMask[2];
	u8 customData[0x14];
	u8 padding3[0x14];
	u8 eshopAccId[4];
	u8 padding4;
	u8 audit;
	u8 padding5[0x42];
	u8 limits[0x40];
} tik_hdr;



