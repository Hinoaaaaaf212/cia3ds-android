#include "lib.h"
#include "romfs_fs.h"
#include "ncch_build.h"
#include "romfs.h"

int PrepareImportRomFsBinaryFromFile(ncch_settings *ncchset, romfs_buildctx *ctx)
{
	ctx->ImportRomfsBinary = true;
	ctx->romfsSize = ncchset->componentFilePtrs.romfsSize;
	ctx->romfsBinary = ncchset->componentFilePtrs.romfs;

	ivfc_hdr *hdr = calloc(1,sizeof(ivfc_hdr));

	ReadFile64(hdr,sizeof(ivfc_hdr),0,ctx->romfsBinary);
	if(memcmp(hdr->magic,"IVFC",4) != 0){
		fprintf(stderr,"[ROMFS ERROR] Invalid RomFS Binary.\n");
		return INVALID_ROMFS_FILE;
	}

	ctx->romfsHeaderSize = align(sizeof(ivfc_hdr),0x10) + (u64)u8_to_u32(hdr->masterHashSize,LE);

	return 0;
}

int ImportRomFsBinaryFromFile(romfs_buildctx *ctx)
{
	ReadFile64(ctx->output,ctx->romfsSize,0,ctx->romfsBinary);
	if(memcmp(ctx->output,"IVFC",4) != 0){
		fprintf(stderr,"[ROMFS ERROR] Invalid RomFS Binary.\n");
		return INVALID_ROMFS_FILE;
	}
	return 0;
}
