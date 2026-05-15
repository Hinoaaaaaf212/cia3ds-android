#include <tc/crypto/RsaKeyGenerator.h>

void tc::crypto::GenerateRsaKey(RsaKey& key, size_t key_bit_size)
{
	tc::crypto::RsaKeyGenerator impl;
	impl.generateKey(key, key_bit_size);
}