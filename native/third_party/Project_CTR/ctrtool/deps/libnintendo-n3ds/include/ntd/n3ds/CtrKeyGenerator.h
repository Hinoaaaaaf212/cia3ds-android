#pragma once
#include <tc/types.h>

namespace ntd { namespace n3ds {

class CtrKeyGenerator
{
public:
	static void GenerateKey(const uint8_t *x, const uint8_t *y, uint8_t *key);
private:
	static int32_t wrap_index(int32_t i);
	static void n128_rrot(const uint8_t *in, uint32_t rot, uint8_t *out);
	static void n128_lrot(const uint8_t *in, uint32_t rot, uint8_t *out);
	static void n128_add(const uint8_t *a, const uint8_t *b, uint8_t *out);
	static void n128_sub(const uint8_t *a, const uint8_t *b, uint8_t *out);
	static void n128_xor(const uint8_t *a, const uint8_t *b, uint8_t *out);
};

}} // namespace ntd::n3ds