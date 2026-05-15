#include "lib.h"
#include "romfs_fs.h"

/* This is the FS interface for ROMFS generation */
/* Tested working on Windows/Linux/OSX */
int PopulateDir(romfs_dir *dir);
int InitDir(romfs_dir *dir);
int ManageDir(romfs_dir *dir);


int InitDir(romfs_dir *dir)
{
	dir->m_child = 10;
	dir->u_child = 0;
	dir->child = calloc(dir->m_child,sizeof(romfs_dir));
	
	dir->m_file = 10;
	dir->u_file = 0;
	dir->file = calloc(dir->m_file,sizeof(romfs_file));

	if (dir->child == NULL || dir->file == NULL)
		return MEM_ERROR;
	
	return 0;
}

int ManageDir(romfs_dir *dir)
{
	if (dir->u_child >= dir->m_child) {
		dir->m_child = 2 * dir->u_child;
		dir->child = realloc(dir->child, dir->m_child*sizeof(romfs_dir));
	}
	if (dir->u_file >= dir->m_file) {
		dir->m_file = 2 * dir->u_file;
		dir->file = realloc(dir->file, dir->m_file*sizeof(romfs_file));
	}

	if (dir->child == NULL || dir->file == NULL)
		return MEM_ERROR;

	return 0;
}

int OpenRootDir(const char *path, romfs_dir *dir)
{
	// Create native FS path
	dir->path = os_CopyConvertCharStr(path);
	// Copy romfs name (empty string)
	dir->name = utf16_CopyStr(ROMFS_EMPTY_PATH);
	dir->namesize = 0;
	
	return PopulateDir(dir);
}

int PopulateDir(romfs_dir *dir)
{
	_OSDIR *dp, *tmp_dp;
	struct _osdirent *entry;

	if (InitDir(dir))
		return MEM_ERROR;

	// Open Directory
	if((dp = os_opendir(dir->path)) == NULL)
	{
		printf("[ROMFS] Failed to open directory: \"");
		os_fputs(dir->path, stdout);
		printf("\"\n");
		return -1;
	}
	
	// Process Entries
	while ((entry = os_readdir(dp)) != NULL)
	{
		// Skip if "." or ".."
		if (os_strcmp(entry->d_name, OS_CURRENT_DIR_PATH) == 0 || os_strcmp(entry->d_name, OS_PARENT_DIR_PATH) == 0)
			continue;

		// Ensures that there is always memory for child directory and file structs
		if (ManageDir(dir))
			return MEM_ERROR;

		// Get native FS path
		oschar_t *path = os_AppendToPath(dir->path, entry->d_name);
		
		// Opening directory with fs path to test if directory
		if ((tmp_dp = os_opendir(path)) != NULL) {
			os_closedir(tmp_dp);

			dir->child[dir->u_child].path = path;
			dir->child[dir->u_child].name = utf16_CopyConvertOsStr(entry->d_name);
			dir->child[dir->u_child].namesize = os_strlen(entry->d_name)*sizeof(utf16char_t);
			dir->u_child++;
			
			// Populate directory
			PopulateDir(&dir->child[dir->u_child-1]);
		}
		// Otherwise this is a file
		else {
			dir->file[dir->u_file].path = path;
			dir->file[dir->u_file].name = utf16_CopyConvertOsStr(entry->d_name);
			dir->file[dir->u_file].namesize = os_strlen(entry->d_name)*sizeof(utf16char_t);
			dir->file[dir->u_file].size = os_fsize(path);
			dir->u_file++;
		}
	}

	os_closedir(dp);

	return 0;
}


void PrintDir(romfs_dir *dir, u32 depth)
{
	for(u32 i = 0; i < depth; i++)
		printf(" ");

	if (depth > 0)
		utf16_fputs(dir->name, stdout);
	else
		printf("romfs:");
	putchar('\n');
	
	if(dir->u_file)
	{
		for(u32 i = 0; i < dir->u_file; i++)
		{
			for(u32 j = 0; j < depth+1; j++)
				printf(" ");
			utf16_fputs(dir->file[i].name, stdout);
			printf(" (0x%"PRIx64")\n", dir->file[i].size);
		}
	}
	if(dir->u_child)
	{
		for(u32 i = 0; i < dir->u_child; i++)
			PrintDir(&dir->child[i],depth+1);
	}
}

void FreeDir(romfs_dir *dir)
{
	//printf("DIR!! free file names\n");
	for(u32 i = 0; i < dir->u_file; i++)
	{
		free(dir->file[i].path);
		free(dir->file[i].name);
	}
	//printf("free file struct\n");
	free(dir->file);
	
	
	//printf("free dir names and\n");
	for(u32 i = 0; i < dir->u_child; i++)
	{	
		free(dir->child[i].path);
		free(dir->child[i].name);
		FreeDir(&dir->child[i]);
	}
	//printf("free dir struct\n");
	free(dir->child);
	
}