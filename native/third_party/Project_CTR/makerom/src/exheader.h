#pragma once

typedef enum
{
	infoflag_COMPRESS_EXEFS_0 = 1,
	infoflag_SD_APPLICATION = 2,
} system_info_flags;

typedef enum
{
	sysmode_64MB, // prod
	sysmode_UNK, // null
	sysmode_96MB, // dev1
	sysmode_80MB, // dev2
	sysmode_72MB, // dev3
	sysmode_32MB, // dev4
} system_mode;

typedef enum
{
	sysmode_ext_LEGACY,
	sysmode_ext_124MB, // snake Prod
	sysmode_ext_178MB, // snake Dev1
} system_mode_ext;

typedef enum
{
	memtype_APPLICATION = 1,
    memtype_SYSTEM = 2,
    memtype_BASE = 3
} memory_type;

typedef enum
{
	processtype_DEFAULT = -1,
	processtype_SYSTEM = 0,
	processtype_APPLICATION = 1
} process_type;

typedef enum
{
	resrc_limit_APPLICATION,
	resrc_limit_SYS_APPLET,
	resrc_limit_LIB_APPLET,
	resrc_limit_OTHER
} resource_limit_category;

typedef enum
{
	cpuspeed_268MHz,
	cpuspeed_804MHz
} cpu_speed;

typedef enum
{
	othcap_PERMIT_DEBUG = (1 << 0),
	othcap_FORCE_DEBUG = (1 << 1),
	othcap_CAN_USE_NON_ALPHABET_AND_NUMBER = (1 << 2),
	othcap_CAN_WRITE_SHARED_PAGE = (1 << 3),
	othcap_CAN_USE_PRIVILEGE_PRIORITY = (1 << 4),
	othcap_PERMIT_MAIN_FUNCTION_ARGUMENT = (1 << 5),
	othcap_CAN_SHARE_DEVICE_MEMORY = (1 << 6),
	othcap_RUNNABLE_ON_SLEEP = (1 << 7),
	othcap_SPECIAL_MEMORY_ARRANGE = (1 << 12),
	othcap_CAN_ACCESS_CORE2 = (1 << 13),
} other_capabilities_flags;

typedef enum
{
	fsaccess_CATEGORY_SYSTEM_APPLICATION = (1 << 0), // 0x00000001 used by all sys apps?
	fsaccess_CATEGORY_HARDWARE_CHECK = (1 << 1), // 0x00000002
	fsaccess_CATEGORY_FILE_SYSTEM_TOOL = (1 << 2), // 0x00000004
	fsaccess_DEBUG = (1 << 3), // 0x00000008
	fsaccess_TWL_CARD_BACKUP = (1 << 4), // 0x00000010
	fsaccess_TWL_NAND_DATA = (1 << 5), // 0x00000020
	fsaccess_BOSS = (1 << 6), // 0x00000040
	fsaccess_DIRECT_SDMC = (1 << 7), // 0x00000080
	fsaccess_CORE = (1 << 8), // 0x00000100
	fsaccess_CTR_NAND_RO = (1 << 9), // 0x00000200
	fsaccess_CTR_NAND_RW = (1 << 10), // 0x00000400
	fsaccess_CTR_NAND_RO_WRITE = (1 << 11), // 0x00000800
	fsaccess_CATEGORY_SYSTEM_SETTINGS = (1 << 12), // 0x00001000
	fsaccess_CARD_BOARD = (1 << 13), // 0x00002000 probably used by sys transfer
	fsaccess_EXPORT_IMPORT_IVS = (1 << 14), // 0x00004000
	fsaccess_DIRECT_SDMC_WRITE = (1 << 15), // 0x00008000
	fsaccess_SWITCH_CLEANUP = (1 << 16), // 0x00010000 reference to Sys Transfer?
	fsaccess_SAVE_DATA_MOVE = (1 << 17), // 0x00020000 used by save transfer tool
	fsaccess_SHOP = (1 << 18), // 0x00040000 probably used by eshop
	fsaccess_SHELL = (1 << 19), // 0x00080000 reference to "Nintendo [User Interface] Shell" (NS)?
	fsaccess_CATEGORY_HOME_MENU = (1 << 20), // 0x00100000 used by homemenu
	fsaccess_SEEDDB = (1 << 21), // 0x00200000 seeddb access
} file_system_access;

typedef enum
{
	attribute_NOT_USE_ROMFS = (1 << 0),
	attribute_USE_EXTENDED_SAVEDATA_ACCESS_CONTROL = (1 << 1),
} attribute_name;

typedef enum
{
	arm9cap_FS_MOUNT_NAND = (1 << 0),
	arm9cap_FS_MOUNT_NAND_RO_WRITE = (1 << 1),
	arm9cap_FS_MOUNT_TWLN = (1 << 2),
	arm9cap_FS_MOUNT_WNAND = (1 << 3),
	arm9cap_FS_MOUNT_CARD_SPI = (1 << 4),
	arm9cap_USE_SDIF3 = (1 << 5),
	arm9cap_CREATE_SEED = (1 << 6),
	arm9cap_USE_CARD_SPI = (1 << 7),
	arm9cap_SD_APPLICATION = (1 << 8),
	arm9cap_USE_DIRECT_SDMC = (1 << 9),
} arm9_capability;

typedef struct
{
	u8 address[4]; // le u32
	u8 numMaxPages[4]; // le u32
	u8 codeSize[4]; // le u32
} exhdr_CodeSegmentInfo;

typedef struct
{
	u8 name[8];
	u8 padding0[5];
	union {
		u8 flag;
		struct {
			u8 compressExeFs0 : 1;
			u8 useOnSd : 1;
		};
	};
	
	u8 remasterVersion[2]; // le u16
	exhdr_CodeSegmentInfo text;
	u8 stackSize[4]; // le u32
	exhdr_CodeSegmentInfo rodata;
	u8 padding1[4];
	exhdr_CodeSegmentInfo data;
	u8 bssSize[4]; // le u32
} exhdr_CodeSetInfo;

typedef struct
{
	u8 savedataSize[8];
	u8 jumpId[8];
	u8 padding0[0x30];
} exhdr_SystemInfo;

typedef struct
{
	u8 extSavedataId[8];
	u8 systemSavedataId[2][4];
	u8 storageAccessableUniqueIds[8];
	u8 accessInfo[7];
	u8 otherAttributes;
} exhdr_StorageInfo;

typedef struct
{
	u8 programId[8];
	u8 coreVersion[4];
	union {
		u8 flag[4];
		struct {
			u8 enableL2Cache : 1;
			u8 cpuSpeed : 1;
			u8: 6;

			u8 systemModeExt : 4;
			u8: 4;

			u8 idealProcessor : 2;
			u8 affinityMask : 2;
			u8 systemMode : 4;

			s8 threadPriority;
		};
	};
	u8 resourceLimitDescriptor[16][2];
	exhdr_StorageInfo storageInfo;
	u8 serviceAccessControl[34][8]; // Those char[8] server names
	u8 padding1[0xf];
	u8 resourceLimitCategory;
} exhdr_ARM11SystemLocalCapabilities;

typedef struct
{
	u16 num;
	u32 *data;
} ARM11KernelCapabilityDescriptor;

typedef enum
{
	desc_InteruptNumList = 0xe0000000,
	desc_SysCallControl = 0xf0000000,
	desc_KernelReleaseVersion = 0xfc000000,
	desc_HandleTableSize = 0xfe000000,
	desc_OtherCapabilities = 0xff000000,
	desc_MappingStatic = 0xff800000,
	desc_MappingIO = 0xffc00000,
} ARM11KernelCapabilityDescriptorBitmask;

typedef struct
{
	u8 descriptors[28][4];// Descripters are a collection of u32s, with bitmask idents so they can be identified, 'no matter the pos'
	u8 reserved[0x10];
} exhdr_ARM11KernelCapabilities;

typedef struct
{
	u8 descriptors[16]; //descriptors[15] = DescVersion
} exhdr_ARM9AccessControlInfo;

typedef struct
{
	// systemcontrol info {
	// coreinfo {
	exhdr_CodeSetInfo codeSetInfo;
	u8 dependencyList[0x30][8];
	// }
	exhdr_SystemInfo systemInfo;
	// }
	// accesscontrolinfo {
	exhdr_ARM11SystemLocalCapabilities arm11SystemLocalCapabilities;
	exhdr_ARM11KernelCapabilities arm11KernelCapabilities;
	exhdr_ARM9AccessControlInfo arm9AccessControlInfo;
	// }
} extended_hdr;

typedef struct 
{
	u8 signature[0x100];
	u8 ncchRsaPubKey[0x100];
	exhdr_ARM11SystemLocalCapabilities arm11SystemLocalCapabilities;
	exhdr_ARM11KernelCapabilities arm11KernelCapabilities;
	exhdr_ARM9AccessControlInfo arm9AccessControlInfo;
} access_descriptor;

/* ExHeader Signature Functions */
int SignAccessDesc(access_descriptor *acexDesc, keys_struct *keys);
int CheckAccessDescSignature(access_descriptor *acexDesc, keys_struct *keys);

/* ExHeader Settings Read from Rsf */
int GetSaveDataSizeFromString(u64 *out, char *string, char *moduleName);
int GetRemasterVersion_rsf(u16 *RemasterVersion, user_settings *usrset);

void ErrorParamNotFound(char *string);
void WarnParamNotFound(char *string);