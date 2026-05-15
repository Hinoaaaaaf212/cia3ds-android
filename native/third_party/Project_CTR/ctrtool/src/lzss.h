#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t lzss_get_decompressed_size(uint8_t* compressed, uint32_t compressedsize);
int lzss_decompress(uint8_t* compressed, uint32_t compressedsize, uint8_t* decompressed, uint32_t decompressedsize);

#ifdef __cplusplus
}
#endif