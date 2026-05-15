#pragma once
#include "exefs.h"

typedef enum
{
	PTR_ERROR = -10,
	EXEFS_MAX_REACHED = -11,
	EXEFS_SECTION_NAME_ERROR = -12,
} exefs_errors;

typedef struct
{
	//Input
	int fileCount;
	u8 *file[MAX_EXEFS_SECTIONS];
	u32 fileSize[MAX_EXEFS_SECTIONS];
	u32 fileOffset[MAX_EXEFS_SECTIONS];
	char fileName[MAX_EXEFS_SECTIONS][8];
	u32 blockSize;
	
	//Working Data
	exefs_filehdr fileHdr[MAX_EXEFS_SECTIONS];
	u8 fileHashes[MAX_EXEFS_SECTIONS][0x20];
	
} exefs_buildctx;

/* ExeFs Build Functions */
int BuildExeFs(ncch_settings *ncchset);