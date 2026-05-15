#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BLZ_NORMAL    0          // normal mode
#define BLZ_BEST      1          // best mode

uint8_t *BLZ_Code(uint8_t *raw_buffer, int raw_len, uint32_t *new_len, int best);

#ifdef __cplusplus
}
#endif