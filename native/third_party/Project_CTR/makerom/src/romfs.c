#include "lib.h"
#include "ncch_build.h"
#include "romfs.h"
#include "romfs_gen.h"
#include "romfs_import.h"

void FreeRomFsCtx(romfs_buildctx *ctx);

// RomFs Build Functions
int SetupRomFs(ncch_settings *ncchset, romfs_buildctx *ctx)
{
	ctx->verbose = ncchset->options.verbose;
	ctx->output = NULL;
	ctx->romfsSize = 0;

	// If Not Using RomFS Return
	if(!ncchset->options.UseRomFS)
		return 0;

	if(ncchset->componentFilePtrs.romfs)// The user has specified a pre-built RomFs Binary
		return PrepareImportRomFsBinaryFromFile(ncchset,ctx);
	
	else // Otherwise build ROMFS
		return PrepareBuildRomFsBinary(ncchset,ctx);

}

int BuildRomFs(romfs_buildctx *ctx)
{
	// If Not Using RomFS Return
	if(!ctx->romfsSize)
		return 0;

	int result = 0;
	
	if(ctx->ImportRomfsBinary) // The user has specified a pre-built RomFs Binary
		result = ImportRomFsBinaryFromFile(ctx);
	else // Otherwise build ROMFS
		result = BuildRomFsBinary(ctx);	

	FreeRomFsCtx(ctx);

	return result;
}

void FreeRomFsCtx(romfs_buildctx *ctx)
{
	if(ctx->fs){
		FreeDir(ctx->fs);	
		free(ctx->fs);
	}
}