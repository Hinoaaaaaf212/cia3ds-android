#pragma once
#include <stdint.h>

typedef struct CtrRsa2048Key {
	uint8_t modulus[0x100];
	uint8_t priv_exponent[0x100];
	//uint8_t pub_exponent[0x3];
} CtrRsa2048Key;

typedef struct CtrRsa4096Key {
	uint8_t modulus[0x200];
	uint8_t priv_exponent[0x200];
	//uint8_t pub_exponent[0x3];
} CtrRsa4096Key;