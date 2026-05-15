#include "lib.h"
#include "ncch_build.h"
#include "romfs.h"

const int ROMFS_BLOCK_SIZE = 0x1000;
const unsigned int ROMFS_UNUSED_ENTRY = 0xffffffff;

// Build
bool IsFileWanted(romfs_file *file, void *filter_criteria);
bool IsDirWanted(romfs_dir *dir, void *filter_criteria);
void CalcDirSize(romfs_buildctx *ctx, romfs_dir *fs);
void CalcRomfsSize(romfs_buildctx *ctx);
int FilterRomFS(romfs_dir *fs_raw, romfs_dir *fs_filtered, void *filter_criteria);
void AddFileToRomfs(romfs_buildctx *ctx, romfs_file *file, u32 parent, u32 sibling);
void AddDirToRomfs(romfs_buildctx *ctx, romfs_dir *fs, u32 parent, u32 sibling);
void AddDirChildrenToRomfs(romfs_buildctx *ctx, romfs_dir *fs, u32 parent, u32 dir);
void PopulateHashTable(romfs_buildctx *ctx);
void PopulateRomfs(romfs_buildctx *ctx);
void BuildRomfsHeader(romfs_buildctx *ctx);
void BuildIvfcHeader(romfs_buildctx *ctx);
void GenIvfcHashTree(romfs_buildctx *ctx);
u32 CalcPathHash(u32 parent, const utf16char_t* path);


int PrepareBuildRomFsBinary(ncch_settings *ncchset, romfs_buildctx *ctx)
{
	/* FS Structures */
	void *filter_criteria = NULL;
	romfs_dir *fs_raw = calloc(1,sizeof(romfs_dir));
	ctx->fs = calloc(1,sizeof(romfs_dir));

	/* Import FS and process */
	OpenRootDir(ncchset->rsfSet->RomFs.RootPath,fs_raw);
	FilterRomFS(fs_raw,ctx->fs,filter_criteria);
	
	/* free unfiltered FS */
	FreeDir(fs_raw);
	free(fs_raw);
	
	/* Abort romfs making, if no wanted files/directories were found */
	if(ctx->fs->u_file == 0 && ctx->fs->u_child == 0){
		ctx->romfsSize = 0;
		return 0;
	}
	
	CalcRomfsSize(ctx);
	
	if (ctx->verbose) {
		printf("[ROMFS] File System:\n");
		printf(" > Size:         %"PRIx64"\n", ctx->romfsSize);
		printf(" > Directories:  %d\n", ctx->dirNum);
		printf(" > Files:        %d\n", ctx->fileNum);
	}

	return 0;
}

int BuildRomFsBinary(romfs_buildctx *ctx)
{
	/* Decide IVFC Level Actual Offsets */
	ctx->level[0].offset = 0;
	ctx->level[3].offset = ctx->level[0].offset + align(ctx->level[0].size, ROMFS_BLOCK_SIZE);
	ctx->level[1].offset = ctx->level[3].offset + align(ctx->level[3].size, ROMFS_BLOCK_SIZE);
	ctx->level[2].offset = ctx->level[1].offset + align(ctx->level[1].size, ROMFS_BLOCK_SIZE);
	
	/* Decide IVFC Level Logical Offsets */
	for(int i = 1; i < 4; i++){
		if(i == 1)
			ctx->level[i].logicalOffset = 0;
		else
			ctx->level[i].logicalOffset = align(ctx->level[i-1].logicalOffset + ctx->level[i-1].size,ROMFS_BLOCK_SIZE);
	}
	
	/* Setup IVFC Level Ptrs */
	for(int i = 0; i < 4; i++){
		ctx->level[i].pos = (ctx->output + ctx->level[i].offset);
		if(i == 0)
			ctx->level[i].pos += align(sizeof(ivfc_hdr),0x10);
	}
	
	/* Build Romfs */
	ctx->romfsHdr = (romfs_infoheader*)(ctx->level[3].pos);
	BuildRomfsHeader(ctx);
	PopulateRomfs(ctx);
	
	
	/* Finalise by building IVFC hash tree */
	ctx->ivfcHdr = (ivfc_hdr*)(ctx->output + ctx->level[0].offset);
	BuildIvfcHeader(ctx);
	GenIvfcHashTree(ctx);

	return 0;
}


bool IsFileWanted(romfs_file *file, void *filter_criteria)
{
	return true;
}

bool IsDirWanted(romfs_dir *dir, void *filter_criteria)
{
	bool ret = false;
	for(u32 i = 0; i < dir->u_file; i++)
	{
		if(IsFileWanted(&dir->file[i],filter_criteria))
		{
			ret = true;
			break;
		}
	}
	for(u32 i = 0; i < dir->u_child; i++)
	{
		if(IsDirWanted(&dir->child[i],filter_criteria))
		{
			ret = true;
			break;
		}
	}
	return ret;
}

void CalcDirSize(romfs_buildctx *ctx, romfs_dir *fs)
{
	if(ctx->m_dirTableLen == 0)
		ctx->m_dirTableLen = sizeof(romfs_direntry);
	else
		ctx->m_dirTableLen += sizeof(romfs_direntry) + align(fs->namesize,4);
		
	for(u32 i = 0; i < fs->u_file; i++)
	{
		ctx->m_fileTableLen += sizeof(romfs_fileentry) + align(fs->file[i].namesize,4);
		if(fs->file[i].size)
			ctx->m_dataLen =  align(ctx->m_dataLen,0x10) + fs->file[i].size;
	}
	
	for(u32 i = 0; i < fs->u_child; i++)
	{
		CalcDirSize(ctx,&fs->child[i]);
	}
	ctx->fileNum += fs->u_file;
	ctx->dirNum += fs->u_child;
}

u32 GetHashTableCount(u32 num)
{
	u32 count = num;
	if (num < 3)
		count = 3;
	else if (count < 19)
		count |= 1;
	else {
		while (count % 2 == 0 || count % 3 == 0 || count % 5 == 0 || count % 7 == 0 || count % 11 == 0 || count % 13 == 0 || count % 17 == 0)
			count++;
	}
	return count;
}

void CalcRomfsSize(romfs_buildctx *ctx)
{
	ctx->dirNum = 1; // root dir
	CalcDirSize(ctx,ctx->fs);
	
	ctx->m_dirHashTable = GetHashTableCount(ctx->dirNum);
		
	ctx->m_fileHashTable = GetHashTableCount(ctx->fileNum);
	
	u32 romfsHdrSize = align(sizeof(romfs_infoheader) + ctx->m_dirHashTable*sizeof(u32) + ctx->m_dirTableLen + ctx->m_fileHashTable*sizeof(u32) + ctx->m_fileTableLen,0x10); 
	
	ctx->level[3].size = romfsHdrSize + ctx->m_dataLen; // data
	ctx->level[2].size = align(ctx->level[3].size,ROMFS_BLOCK_SIZE) / ROMFS_BLOCK_SIZE * SHA_256_LEN ;
	ctx->level[1].size = align(ctx->level[2].size,ROMFS_BLOCK_SIZE) / ROMFS_BLOCK_SIZE * SHA_256_LEN ;
	ctx->level[0].size = align(ctx->level[1].size,ROMFS_BLOCK_SIZE) / ROMFS_BLOCK_SIZE * SHA_256_LEN + align(sizeof(ivfc_hdr),0x10); // hdr
	
	ctx->romfsHeaderSize = ctx->level[0].size;

	ctx->romfsSize = 0;
	for(int i = 0; i < 4; i++)
		ctx->romfsSize += align(ctx->level[i].size,ROMFS_BLOCK_SIZE);		
}

int FilterRomFS(romfs_dir *fs_raw, romfs_dir *fs_filtered, void *filter_criteria)
{
	memset(fs_filtered,0,sizeof(romfs_dir));
	if(!IsDirWanted(fs_raw,filter_criteria))
		return 0;
	
	fs_filtered->path = os_CopyStr(fs_raw->path);

	fs_filtered->namesize = fs_raw->namesize;
	fs_filtered->name = utf16_CopyStr(fs_raw->name);
	
	fs_filtered->u_child = 0;
	fs_filtered->m_child = fs_raw->u_child;
	fs_filtered->child = calloc(fs_filtered->m_child,sizeof(romfs_dir));
	
	fs_filtered->u_file = 0;
	fs_filtered->m_file = fs_raw->u_file;
	fs_filtered->file = calloc(fs_filtered->m_file,sizeof(romfs_file));
	
	for(u32 i = 0; i < fs_raw->u_child; i++)
	{
		if(IsDirWanted(&fs_raw->child[i],filter_criteria))
		{
			FilterRomFS(&fs_raw->child[i],&fs_filtered->child[fs_filtered->u_child],filter_criteria);
			fs_filtered->u_child++;
		}
	}
	
	for(u32 i = 0; i < fs_raw->u_file; i++)
	{
		if(IsFileWanted(&fs_raw->file[i],filter_criteria))
		{
			fs_filtered->file[fs_filtered->u_file].path = os_CopyStr(fs_raw->file[i].path);

			fs_filtered->file[fs_filtered->u_file].namesize = fs_raw->file[i].namesize;
			fs_filtered->file[fs_filtered->u_file].name = utf16_CopyStr(fs_raw->file[i].name);
			
			fs_filtered->file[fs_filtered->u_file].size = fs_raw->file[i].size;
			
			fs_filtered->u_file++;
		}
	}
	
	return 0;
}

void BuildRomfsHeader(romfs_buildctx *ctx)
{
	u32 level3_pos = 0;
	
	u32_to_u8(ctx->romfsHdr->headersize,sizeof(romfs_infoheader),LE);
	
	level3_pos += sizeof(romfs_infoheader);
	
	for(int i = 0; i < 4; i++){
		if(i == 0){
			ctx->dirHashTable = ctx->level[3].pos + level3_pos;
			u32_to_u8(ctx->romfsHdr->section[i].offset,level3_pos,LE);
			u32_to_u8(ctx->romfsHdr->section[i].size,ctx->m_dirHashTable*sizeof(u32),LE);
			level3_pos += ctx->m_dirHashTable*sizeof(u32);
		}
		else if(i == 1 && ctx->m_dirTableLen){
			ctx->dirTable = ctx->level[3].pos + level3_pos;
			u32_to_u8(ctx->romfsHdr->section[i].offset,level3_pos,LE);
			u32_to_u8(ctx->romfsHdr->section[i].size,ctx->m_dirTableLen,LE);
			level3_pos += ctx->m_dirTableLen;
		}
		else if(i == 2){
			ctx->fileHashTable = ctx->level[3].pos + level3_pos;
			u32_to_u8(ctx->romfsHdr->section[i].offset,level3_pos,LE);
			u32_to_u8(ctx->romfsHdr->section[i].size,ctx->m_fileHashTable*sizeof(u32),LE);
			level3_pos += ctx->m_fileHashTable*sizeof(u32);
		}
		else if(i == 3 && ctx->m_fileTableLen){
			ctx->fileTable = ctx->level[3].pos + level3_pos;
			u32_to_u8(ctx->romfsHdr->section[i].offset,level3_pos,LE);
			u32_to_u8(ctx->romfsHdr->section[i].size,ctx->m_fileTableLen,LE);
			level3_pos += ctx->m_fileTableLen;
		}
		else{
			u32_to_u8(ctx->romfsHdr->section[i].offset,0,LE);
			u32_to_u8(ctx->romfsHdr->section[i].size,0,LE);
		}
	}
	
	ctx->data = ctx->level[3].pos + align(level3_pos,0x10);
	u32_to_u8(ctx->romfsHdr->dataoffset,align(level3_pos,0x10),LE);
	
	for (u32 i = 0; i < ctx->m_dirHashTable; i++) {
		u32_to_u8(ctx->dirHashTable+i*4, ROMFS_UNUSED_ENTRY, LE);
	}

	for (u32 i = 0; i < ctx->m_fileHashTable; i++) {
		u32_to_u8(ctx->fileHashTable+i*4, ROMFS_UNUSED_ENTRY, LE);
	}
}

u32 GetFileHashTableIndex(romfs_buildctx *ctx, u32 parent, const utf16char_t *path)
{
	u32 hash = CalcPathHash(parent, path);
	return hash % ctx->m_fileHashTable;
}

u32 GetDirHashTableIndex(romfs_buildctx *ctx, u32 parent, const utf16char_t* path)
{
	u32 hash = CalcPathHash(parent, path);
	return hash % ctx->m_dirHashTable;
}

void AddFileToRomfs(romfs_buildctx *ctx, romfs_file *file, u32 parent, u32 sibling)
{
	romfs_fileentry *entry = (romfs_fileentry*)(ctx->fileTable + ctx->u_fileTableLen);
	
	u32_to_u8(entry->parentdiroffset,parent,LE);
	u32_to_u8(entry->siblingoffset,sibling,LE);	
	
	/* Import name */
	u32_to_u8(entry->namesize,file->namesize,LE);
	u8 *name_pos = (u8*)(ctx->fileTable + ctx->u_fileTableLen + sizeof(romfs_fileentry));
	memset(name_pos,0,align(file->namesize,4));
	memcpy(name_pos,(u8*)file->name,file->namesize);

	/* Set hash data */
	u32 hashindex = GetFileHashTableIndex(ctx, parent, file->name);
	u32_to_u8(entry->hashoffset, u8_to_u32(ctx->fileHashTable + hashindex*4, LE), LE);
	u32_to_u8(ctx->fileHashTable + hashindex*4, ctx->u_fileTableLen, LE);
	
	/* Import data */
	if(file->size)
	{
		ctx->u_dataLen = align(ctx->u_dataLen,0x10); // Padding
		u64_to_u8(entry->dataoffset,ctx->u_dataLen,LE);
		u64_to_u8(entry->datasize,file->size,LE);
		u8 *data_pos = (ctx->data + ctx->u_dataLen);

		if (ctx->verbose) {
			printf("[ROMFS] Reading \"");
			os_fputs(file->path, stdout);
			printf("\"... ");
		}

		FILE *fp = os_fopen(file->path, OS_MODE_READ);
		fread(data_pos, file->size, 1, fp);
		fclose(fp);

		if (ctx->verbose) {
			printf("Done!\n");
		}

		ctx->u_dataLen += file->size; // adding file size
	}
	else
		u64_to_u8(entry->dataoffset,0x00,LE);
		
	/* Increment used file table length */
	ctx->u_fileTableLen += sizeof(romfs_fileentry) + align(file->namesize,4);
}

void AddDirToRomfs(romfs_buildctx *ctx, romfs_dir *fs, u32 parent, u32 sibling)
{
	u32 offset = ctx->u_dirTableLen;
	romfs_direntry *entry = (romfs_direntry*)(ctx->dirTable + offset);
	
	/* Set entry data */
	u32_to_u8(entry->parentoffset,parent,LE);
	u32_to_u8(entry->siblingoffset,sibling,LE);	
	u32_to_u8(entry->childoffset, ROMFS_UNUSED_ENTRY, LE);
	u32_to_u8(entry->fileoffset, ROMFS_UNUSED_ENTRY, LE);
	
	/* Import name */
	u32_to_u8(entry->namesize,fs->namesize,LE);
	u8 *name_pos = (u8*)(ctx->dirTable + ctx->u_dirTableLen + sizeof(romfs_direntry));
	memset(name_pos,0,(u32)align(fs->namesize,4));
	memcpy(name_pos,(u8*)fs->name,fs->namesize);

	/* Set hash data */
	u32 hashindex = GetDirHashTableIndex(ctx, parent, fs->name);
	u32_to_u8(entry->hashoffset, u8_to_u32(ctx->dirHashTable + hashindex * 4, LE), LE);
	u32_to_u8(ctx->dirHashTable + hashindex * 4, offset, LE);

	/* Increment used dir table length */
	ctx->u_dirTableLen += sizeof(romfs_direntry) + (u32)align(fs->namesize,4);
}

void AddDirChildrenToRomfs(romfs_buildctx *ctx, romfs_dir *fs, u32 parent, u32 dir)
{
	romfs_direntry *entry = (romfs_direntry*)(ctx->dirTable + dir);
	
	if (fs->u_file)
	{
		u32_to_u8(entry->fileoffset, ctx->u_fileTableLen, LE);

		/* Create file entries*/
		for (u32 i = 0; i < fs->u_file; i++)
		{
			/* If is the last file, no more siblings */
			u32 file_sibling = 0;
			if (i >= fs->u_file - 1)
				file_sibling = ROMFS_UNUSED_ENTRY;
			else
				file_sibling = ctx->u_fileTableLen + sizeof(romfs_fileentry) + (u32)align(fs->file[i].namesize, 4);

			/* Create file entry */
			AddFileToRomfs(ctx, &fs->file[i], dir, file_sibling);
		}
	}

	if (fs->u_child)
	{
		/* Prepare to store child addresses */
		u32 *childs = calloc(fs->u_child, sizeof(u32));

		/* Create child directory entries*/
		u32_to_u8(entry->childoffset, ctx->u_dirTableLen, LE);
		for (u32 i = 0; i < fs->u_child; i++)
		{
			/* Store address for child */
			childs[i] = ctx->u_dirTableLen;
			
			/* If is the last child directory, no more siblings  */
			u32 dir_sibling = 0;
			if (i >= fs->u_child - 1)
				dir_sibling = ROMFS_UNUSED_ENTRY;
			else
				dir_sibling = ctx->u_dirTableLen + sizeof(romfs_direntry) + (u32)align(fs->child[i].namesize, 4);
			
			/* Create child directory entry */
			AddDirToRomfs(ctx, &fs->child[i], dir, dir_sibling);
		}

		/* Populate child's childs */
		for (u32 i = 0; i < fs->u_child; i++)
		{
			AddDirChildrenToRomfs(ctx, &fs->child[i], dir, childs[i]);
		}

		free(childs);
	}
}

void PopulateRomfs(romfs_buildctx *ctx)
{
	AddDirToRomfs(ctx, ctx->fs, 0x0, ROMFS_UNUSED_ENTRY);
	AddDirChildrenToRomfs(ctx, ctx->fs, 0x0, 0);
}

void BuildIvfcHeader(romfs_buildctx *ctx)
{
	memcpy(ctx->ivfcHdr->magic,"IVFC",4);
	u32_to_u8(ctx->ivfcHdr->id,0x10000,LE);
	
	u32 masterHashSize = ( align(ctx->level[1].size,ROMFS_BLOCK_SIZE) / ROMFS_BLOCK_SIZE ) * SHA_256_LEN ;
	u32_to_u8(ctx->ivfcHdr->masterHashSize,masterHashSize,LE);
	
	for(int i = 1; i < 4; i++){
		u64_to_u8(ctx->ivfcHdr->level[i-1].logicalOffset,ctx->level[i].logicalOffset,LE);
		u64_to_u8(ctx->ivfcHdr->level[i-1].hashDataSize,ctx->level[i].size,LE);
		u32_to_u8(ctx->ivfcHdr->level[i-1].blockSize,log2l(ROMFS_BLOCK_SIZE),LE);
	}
	
	u32_to_u8(ctx->ivfcHdr->optionalSize,sizeof(ivfc_hdr),LE);
	
	return;
}

void GenIvfcHashTree(romfs_buildctx *ctx)
{
	for(int i = 2; i >= 0; i--){
		if (ctx->verbose)
			printf("[ROMFS] Generating IVFC level %d hashes... ", i+1);
		u32 numHashes = align(ctx->level[i+1].size,ROMFS_BLOCK_SIZE) / ROMFS_BLOCK_SIZE;
		for(u32 j = 0; j < numHashes; j++){
			u8 *datapos = (u8*)(ctx->level[i+1].pos + ROMFS_BLOCK_SIZE * j);
			u8 *hashpos = (u8*)(ctx->level[i].pos + SHA_256_LEN * j);
			ShaCalc(datapos, ROMFS_BLOCK_SIZE, hashpos, CTR_SHA_256);
		}
		if (ctx->verbose)
			printf("Done!\n");
	}
	
	return;
}

u32 CalcPathHash(u32 parent, const utf16char_t* path)
{
	u32 len = utf16_strlen(path);
	u32 hash = parent ^ 123456789;
	for( u32 i = 0; i < len; i++ )
	{
		hash = (u32)((hash >> 5) | (hash << 27));//ror
		hash ^= (u16)path[i];
	}
	return hash;
}
