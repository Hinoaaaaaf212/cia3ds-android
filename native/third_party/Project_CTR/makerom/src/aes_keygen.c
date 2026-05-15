#include "aes_keygen.h"

// 128bit wrap-around math
int32_t wrap_index(int32_t i)
{
	return i < 0 ? ((i % 16) + 16) % 16 : (i > 15 ? i % 16 : i);
}

void n128_rrot(const uint8_t *in, uint32_t rot, uint8_t *out)
{
	uint32_t bit_shift, byte_shift;

	rot = rot % 128;
	byte_shift = rot / 8;
	bit_shift = rot % 8;

	for (int32_t i = 0; i < 16; i++) {
		out[i] = (in[wrap_index(i - byte_shift)] >> bit_shift) | (in[wrap_index(i - byte_shift - 1)] << (8 - bit_shift));
	}

}

void n128_lrot(const uint8_t *in, uint32_t rot, uint8_t *out)
{
	uint32_t bit_shift, byte_shift;

	rot = rot % 128;
	byte_shift = rot / 8;
	bit_shift = rot % 8;

	for (int32_t i = 0; i < 16; i++) {
		out[i] = (in[wrap_index(i + byte_shift)] << bit_shift) | (in[wrap_index(i + byte_shift + 1)] >> (8 - bit_shift));
	}
}

/* out = a + b
*/
void n128_add(const uint8_t *a, const uint8_t *b, uint8_t *out)
{
	uint8_t carry = 0;
	uint32_t sum = 0;

	for (int i = 15; i >= 0; i--) {
		sum = a[i] + b[i] + carry;
		carry = sum >> 8;
		out[i] = sum & 0xff;
	}
}

/* out = a - b
*/
void n128_sub(const uint8_t *a, const uint8_t *b, uint8_t *out)
{
	uint8_t carry = 0;
	uint32_t sum = 0;

	for (int i = 15; i >= 0; i--) {
		sum = a[i] - (b[i] + carry);

		// check to see if anything was borrowed from next byte
		if (a[i] < (b[i] + carry)) {
			sum += 0x100;
			carry = 1;
		}
		else {
			carry = 0;
		}

		// set value
		out[i] = sum & 0xff;
	}
}

void n128_xor(const uint8_t *a, const uint8_t *b, uint8_t *out)
{
	for (int i = 0; i < 16; i++) {
		out[i] = a[i] ^ b[i];
	}
}

// keygen algorithm
/*
void n_aes_keygen(const uint8_t *x, uint8_t x_shift, const uint8_t *y, uint8_t y_shift, const uint8_t *keygen_constant, uint8_t *key)
{
	// overall algo:
	// key = ((x >>> x_shift) ^ (y >>> y_shift)) + keygen_constant
	uint8_t x_rot[16], y_rot[16], key_xy[16];

	// Rotate x and y
	n128_rrot(x, x_shift, x_rot);
	n128_rrot(y, y_shift, y_rot);

	// XOR rotated x and y
	n128_xor(x_rot, y_rot, key_xy);

	// Add secret
	n128_add(key_xy, keygen_constant, key);
}
*/

void ctr_aes_keygen(const uint8_t *x, const uint8_t *y, uint8_t *key)
{
	static const uint8_t KEYGEN_CONST[16] = { 0x1F, 0xF9, 0xE9, 0xAA, 0xC5, 0xFE, 0x04, 0x08, 0x02, 0x45, 0x91, 0xDC, 0x5D, 0x52, 0x76, 0x8A };

	// overall algo:
	// key = (((x <<< 2) ^ y) + KEYGEN_CONST) >>> 41
	uint8_t x_rot[16], key_xy[16], key_xyc[16];

	// x_rot = x <<< 2
	n128_lrot(x, 2, x_rot);

	// key_xy = x_rot ^ y
	n128_xor(x_rot, y, key_xy);

	// key_xyc = key_xy + KEYGEN_CONST
	n128_add(key_xy, KEYGEN_CONST, key_xyc);

	n128_rrot(key_xyc, 41, key);
}