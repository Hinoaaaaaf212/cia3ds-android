#pragma once
#include "ncsd.h"

// Read Functions
bool IsCci(u8 *ncsd);
u8* GetPartition(u8 *ncsd, u8 index);
u64 GetPartitionOffset(u8 *ncsd, u8 index);
u64 GetPartitionSize(u8 *ncsd, u8 index);