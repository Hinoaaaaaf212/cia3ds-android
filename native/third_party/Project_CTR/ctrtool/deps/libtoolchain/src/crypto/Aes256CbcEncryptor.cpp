#include <tc/crypto/Aes256CbcEncryptor.h>

void tc::crypto::EncryptAes256Cbc(byte_t* dst, const byte_t* src, size_t size, const byte_t* key, size_t key_size, const byte_t* iv, size_t iv_size)
{
	tc::crypto::Aes256CbcEncryptor crypt;
	crypt.initialize(key, key_size, iv, iv_size);
	crypt.encrypt(dst, src, size);
}

void tc::crypto::DecryptAes256Cbc(byte_t* dst, const byte_t* src, size_t size, const byte_t* key, size_t key_size, const byte_t* iv, size_t iv_size)
{
	tc::crypto::Aes256CbcEncryptor crypt;
	crypt.initialize(key, key_size, iv, iv_size);
	crypt.decrypt(dst, src, size);
}