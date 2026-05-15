#pragma once
#include "ncsd.h"
#include "tmd_read.h"


// Enums
typedef enum
{
	NCSD_NO_NCCH0 = -1,
	NCSD_INVALID_NCCH0 = -2,
	NCSD_INVALID_NCCH = -3,
	INVALID_RSF_OPT = -4,
	GEN_HDR_FAIL = -5,
	INCOMPAT_CIA = -6,
	CCI_CONFIG_FAIL = -7,
} ncsd_errors;

typedef struct
{
	rsf_settings *rsf;
	keys_struct *keys;
	
	FILE *out;
	
	struct{
		bool verbose;
		bool padCci;
		bool noModTid;
		bool useExternalSdkCardInfo;
		bool closeAlignWR;
		
		u8 cverDataType;
		char *cverDataPath;
		tmd_hdr *tmdHdr;
	} options;
	
	struct{
		u32 blockSize;
	
		u64 mediaSize;
		u64 usedSize;
		
		u8 mediaType;
		u8 cardDevice;
		u64 saveSize;
		u64 card2SaveOffset;
	} romInfo;
	
	struct{
		u8 *data;
		u64 dataLen;
		infile_type dataType;
	
		char **path;
	
		bool active[CCI_MAX_CONTENT];
		u64 dOffset[CCI_MAX_CONTENT];
		u64 *dSize;
		u64 titleId[CCI_MAX_CONTENT];
		
		u64 cOffset[CCI_MAX_CONTENT];
	} content;
	
	struct{
		buffer_struct ccihdr;
		buffer_struct cardinfohdr;
	} headers;
} cci_settings;

// Public Prototypes
// Build Functions
int build_CCI(user_settings *usrset);