#include <tc/crypto/HmacSha1Generator.h>

void tc::crypto::GenerateHmacSha1Mac(byte_t* mac, const byte_t* data, size_t data_size, const byte_t* key, size_t key_size)
{
	tc::crypto::HmacSha1Generator impl;
	impl.initialize(key, key_size);
	impl.update(data, data_size);
	impl.getMac(mac);
}