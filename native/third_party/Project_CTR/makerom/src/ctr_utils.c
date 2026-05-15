#include "lib.h"

u32 GetCtrBlockSize(u8 flag)
{
	return 1 << (flag + 9);
}

u8 GetCtrBlockSizeFlag(u32 size)
{
	u8 ret = 0;
	for(u32 tmp = size; tmp > 0x200; tmp = tmp >> 1)
		ret++;
	return ret;
}