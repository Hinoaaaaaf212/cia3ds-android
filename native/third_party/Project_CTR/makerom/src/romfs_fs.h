#pragma once
#include "oschar.h"

struct romfs_file
{
	oschar_t *path;
	utf16char_t *name;
	u32 namesize;
	u64 size;
};

struct romfs_dir
{
	oschar_t *path;
	utf16char_t *name;
	u32 namesize;
	
	struct romfs_dir *child;
	u32 m_child;
	u32 u_child;
	
	struct romfs_file *file;
	u32 m_file;
	u32 u_file;
};

typedef struct romfs_file romfs_file;
typedef struct romfs_dir romfs_dir;

static const utf16char_t ROMFS_EMPTY_PATH[2] = { 0 };
static const oschar_t OS_EMPTY_PATH[2] = { 0 };
static const oschar_t OS_CURRENT_DIR_PATH[2] = { '.' };
static const oschar_t OS_PARENT_DIR_PATH[3] = { '.', '.' };

int OpenRootDir(const char *path, romfs_dir *dir);
void PrintDir(romfs_dir *dir, u32 depth);
void FreeDir(romfs_dir *dir);