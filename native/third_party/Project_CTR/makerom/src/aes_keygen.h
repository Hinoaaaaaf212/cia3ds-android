#pragma once
#include <stdint.h>

/*
	AES Key generator for the Nintendo 3DS (CTR) Consoles
*/

void ctr_aes_keygen(const uint8_t *x, const uint8_t *y, uint8_t *key);
