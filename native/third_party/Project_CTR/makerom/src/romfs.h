#pragma once
#include "romfs_fs.h"

typedef enum
{
	INVALID_ROMFS_FILE = -10,
} romfs_errors;


// IVFC Structs
typedef struct
{
	u64 size;
	u64 offset;
	u64 logicalOffset;
	u8 *pos;
	u8 reserved[8];
} ivfc_level;

typedef struct
{
	u8 logicalOffset[8];
	u8 hashDataSize[8];
	u8 blockSize[4];
	u8 reserved[4];
} ivfc_levelheader;

typedef struct
{
	u8 magic[4];
	u8 id[4];
	u8 masterHashSize[4];
	ivfc_levelheader level[3];
	u8 optionalSize[4];
	u8 reserved[4];
} ivfc_hdr;

typedef struct
{
	u8 offset[4];
	u8 size[4];
} romfs_sectionheader;

typedef struct
{
	u8 headersize[4];
	romfs_sectionheader section[4]; // 8*4 = 0x20
	u8 dataoffset[4];
} romfs_infoheader; //sizeof(romfs_infoheader) = 0x28


typedef struct
{
	u8 parentoffset[4];
	u8 siblingoffset[4];
	u8 childoffset[4];
	u8 fileoffset[4];
	u8 hashoffset[4];
	u8 namesize[4];
	//u8 name[ROMFS_MAXNAMESIZE];
} romfs_direntry; //sizeof(romfs_direntry)  = 0x18

typedef struct
{
	u8 parentdiroffset[4];
	u8 siblingoffset[4];
	u8 dataoffset[8];
	u8 datasize[8];
	u8 hashoffset[4];
	u8 namesize[4];
	//u8 name[ROMFS_MAXNAMESIZE];
} romfs_fileentry; //sizeof(romfs_fileentry)  = 0x20

typedef struct
{
	bool verbose;

	u8 *output;
	u64 romfsSize;
	u64 romfsHeaderSize;

	/* For Importing ROMFS Binaries */
	bool ImportRomfsBinary;
	FILE *romfsBinary;
	
	/* For Creating ROMFS Binaries */
	ivfc_hdr *ivfcHdr;
	romfs_infoheader *romfsHdr;
	
	romfs_dir *fs;
	
	u8 *dirHashTable;
	u32 m_dirHashTable;
	
	u8 *dirTable;
	u32 dirNum;
	u32 m_dirTableLen;
	u32 u_dirTableLen;
	
	u8 *fileHashTable;
	u32 m_fileHashTable;
	
	u8 *fileTable;
	u32 fileNum;
	u32 m_fileTableLen;
	u32 u_fileTableLen;
	
	u8 *data;
	u64 m_dataLen;
	u64 u_dataLen;
	
	// Levels
	ivfc_level level[4];
} romfs_buildctx;

int SetupRomFs(ncch_settings *ncchset, romfs_buildctx *ctx);
int BuildRomFs(romfs_buildctx *ctx);