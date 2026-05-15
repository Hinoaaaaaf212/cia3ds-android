#include "lib.h"
#include "ncch_build.h"
#include "exefs_build.h"

// Private Prototypes
u32 PredictExeFS_Size(exefs_buildctx *ctx);
int GenerateExeFS_Header(exefs_buildctx *ctx, u8 *outbuff);
void FreeExeFSContext(exefs_buildctx *ctx);
int ImportDatatoExeFS(exefs_buildctx *ctx, u8 *outbuff);
int ImportToExeFSContext(exefs_buildctx *ctx, char *name, u8 *buffer, u32 size);

// ExeFs Build Functions
int BuildExeFs(ncch_settings *ncchset)
{
	/* Intialising ExeFs Build Context */
	exefs_buildctx *ctx = calloc(1,sizeof(exefs_buildctx));
	if(!ctx) {
		fprintf(stderr,"[EXEFS ERROR] Not enough memory\n"); 
		return MEM_ERROR;
	}
	ctx->blockSize = ncchset->options.blockSize;

	/* Importing ExeFs */
	if(ncchset->exefsSections.code.size) 
		ImportToExeFSContext(ctx,".code",ncchset->exefsSections.code.buffer,ncchset->exefsSections.code.size);
	if(ncchset->exefsSections.banner.size) 
		ImportToExeFSContext(ctx,"banner",ncchset->exefsSections.banner.buffer,ncchset->exefsSections.banner.size);
	if(ncchset->exefsSections.icon.size) 
		ImportToExeFSContext(ctx,"icon",ncchset->exefsSections.icon.buffer,ncchset->exefsSections.icon.size);
	if(ncchset->sections.logo.size && ncchset->options.IncludeExeFsLogo) 
		ImportToExeFSContext(ctx,"logo",ncchset->sections.logo.buffer,ncchset->sections.logo.size);

	if(ctx->fileCount == 0){ // no exefs needed
		ncchset->sections.exeFs.size = 0;
		ncchset->sections.exeFs.buffer = NULL;
		return 0;
	}

	/* Allocating Memory for ExeFs */
	ncchset->sections.exeFs.size = PredictExeFS_Size(ctx);
	ncchset->sections.exeFs.buffer = malloc(ncchset->sections.exeFs.size);
	if(!ncchset->sections.exeFs.buffer){
		printf("[EXEFS ERROR] Could Not Allocate Memory for ExeFS\n");
		return Fail;
	}
	memset(ncchset->sections.exeFs.buffer,0,ncchset->sections.exeFs.size);

	/* Generating Header, and writing sections to buffer */
	GenerateExeFS_Header(ctx,ncchset->sections.exeFs.buffer);
	ImportDatatoExeFS(ctx,ncchset->sections.exeFs.buffer);

	/* Finish */
	FreeExeFSContext(ctx);
	return 0;
}

u32 PredictExeFS_Size(exefs_buildctx *ctx)
{
	u32 exefs_size = sizeof(exefs_hdr); // Size of header
	for(int i = 0; i < ctx->fileCount; i++)
		exefs_size += align(ctx->fileSize[i],ctx->blockSize);
	//exefs_size = align(ctx->exefs_size,ctx->mediaUnit);
	return exefs_size;
}

int GenerateExeFS_Header(exefs_buildctx *ctx, u8 *outbuff)
{
	exefs_hdr *exefs = (exefs_hdr*)outbuff;
	for(int i = 0; i < ctx->fileCount; i++){
		if(i == 0)
			ctx->fileOffset[i] = 0;
		else
			ctx->fileOffset[i] = align((ctx->fileOffset[i-1]+ctx->fileSize[i-1]),ctx->blockSize);
		
		memcpy(exefs->fileHdr[i].name,ctx->fileName[i],8);
		u32_to_u8(exefs->fileHdr[i].offset,ctx->fileOffset[i],LE);
		u32_to_u8(exefs->fileHdr[i].size,ctx->fileSize[i],LE);
		ShaCalc(ctx->file[i],ctx->fileSize[i],exefs->fileHashes[MAX_EXEFS_SECTIONS-1-i],CTR_SHA_256);
	}
	return 0;
}

void FreeExeFSContext(exefs_buildctx *ctx)
{
	/*
	if(ctx->outbuff != NULL)
		free(ctx->outbuff);
	for(int i = 0; i < 10; i++){
		if(ctx->file[i] != NULL)
			free(ctx->file[i]);
	}
	*/
	memset(ctx,0,sizeof(exefs_buildctx));
	free(ctx);
}

int ImportDatatoExeFS(exefs_buildctx *ctx, u8 *outbuff)
{
	for(int i = 0; i < ctx->fileCount; i++){
		memcpy(outbuff+ctx->fileOffset[i]+0x200,ctx->file[i],ctx->fileSize[i]);
	}
	return 0;
}

int ImportToExeFSContext(exefs_buildctx *ctx, char *name, u8 *buffer, u32 size)
{
	if(ctx == NULL || name == NULL || buffer == NULL){
		printf("[!] PTR ERROR\n");
		return PTR_ERROR;
	}
	if(ctx->fileCount >= MAX_EXEFS_SECTIONS){
		printf("[!] Maximum ExeFS Capacity Reached\n");
		return EXEFS_MAX_REACHED;
	}
	if(strlen(name) > 8){
		printf("[!] ExeFS File Name: '%s' is too large\n",name);
		return EXEFS_SECTION_NAME_ERROR;
	}	
	
	ctx->fileCount++;
	ctx->file[ctx->fileCount - 1] = buffer;
	ctx->fileSize[ctx->fileCount - 1] = size;
	strcpy(ctx->fileName[ctx->fileCount - 1],name);
	return 0;
}

// ExeFs Read Functions
bool DoesExeFsSectionExist(char *section, u8 *ExeFs)
{
	exefs_hdr *hdr = (exefs_hdr*) ExeFs;
	for(int i = 0; i < MAX_EXEFS_SECTIONS; i++){
		if(strncmp(hdr->fileHdr[i].name,section,8) == 0) return true;
	}
	return false;
}
u8* GetExeFsSection(char *section, u8 *ExeFs)
{
	exefs_hdr *hdr = (exefs_hdr*) ExeFs;
	for(int i = 0; i < MAX_EXEFS_SECTIONS; i++){
		if(strncmp(hdr->fileHdr[i].name,section,8) == 0){ 
			u32 offset = u8_to_u32(hdr->fileHdr[i].offset,LE) + sizeof(exefs_hdr);
			return (u8*)(ExeFs+offset);
		}
	}
	return NULL;
}

u8* GetExeFsSectionHash(char *section, u8 *ExeFs)
{
	exefs_hdr *hdr = (exefs_hdr*) ExeFs;
	for(int i = 0; i < MAX_EXEFS_SECTIONS; i++){
		if(strncmp(hdr->fileHdr[i].name,section,8) == 0){ 
			return (u8*)(hdr->fileHashes[MAX_EXEFS_SECTIONS-1-i]);
		}
	}
	return NULL;
}

u32 GetExeFsSectionSize(char *section, u8 *ExeFs)
{
	exefs_hdr *hdr = (exefs_hdr*) ExeFs;
	for(int i = 0; i < MAX_EXEFS_SECTIONS; i++){
		if(strncmp(hdr->fileHdr[i].name,section,8) == 0){ 
			return u8_to_u32(hdr->fileHdr[i].size,LE);
		}
	}
	return 0;
}

u32 GetExeFsSectionOffset(char *section, u8 *ExeFs)
{
	exefs_hdr *hdr = (exefs_hdr*) ExeFs;
	for(int i = 0; i < MAX_EXEFS_SECTIONS; i++){
		if(strncmp(hdr->fileHdr[i].name,section,8) == 0){ 
			return u8_to_u32(hdr->fileHdr[i].offset,LE) + sizeof(exefs_hdr);
		}
	}
	return 0;
}