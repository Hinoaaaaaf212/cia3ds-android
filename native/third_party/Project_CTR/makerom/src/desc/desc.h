#pragma once
#include <stdint.h>

enum SdkAppTypes {
	desc_NotSpecified,
	desc_Application,
	desc_DlpChild,
	desc_Demo,
	desc_EcApplication, // intergrated in (Ext)Application since SDK 7
	desc_ExtApplication, // Snake equivalent of desc_Application (128MB/804MHz/L2 Cache)
	desc_ExtDlpChild, // Snake equivalent of desc_DlpChild (128MB/804MHz/L2 Cache)
	desc_ExtDemo // Snake equivalent of desc_Demo (128MB/804MHz/L2 Cache)
};

typedef struct CtrSdkDepList {
	uint32_t fw_minor;
	uint8_t dependency[0x180];
} CtrSdkDepList;

typedef struct CtrSdkDesc {
	uint32_t type;
	uint32_t fw_minor;
	uint8_t exheader_desc[0x200];
	uint8_t signed_desc[0x200];
} CtrSdkDesc;

typedef struct CtrSdkDescSignData {
	uint32_t type;
	uint32_t fw_minor;
	uint8_t modulus[0x100];
	uint8_t priv_exponent[0x100];
	uint8_t access_desc_signature[0x100];
} CtrSdkDescSignData;