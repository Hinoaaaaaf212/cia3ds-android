#pragma once
#include "romfs.h"

int PrepareBuildRomFsBinary(ncch_settings *ncchset, romfs_buildctx *ctx);
int BuildRomFsBinary(romfs_buildctx *ctx);
