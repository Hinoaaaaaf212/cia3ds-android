#include <tc/crypto/Aes256EcbEncryptor.h>

void tc::crypto::EncryptAes256Ecb(byte_t* dst, const byte_t* src, size_t size, const byte_t* key, size_t key_size)
{
	tc::crypto::Aes256EcbEncryptor crypt;
	crypt.initialize(key, key_size);
	crypt.encrypt(dst, src, size);
}

void tc::crypto::DecryptAes256Ecb(byte_t* dst, const byte_t* src, size_t size, const byte_t* key, size_t key_size)
{
	tc::crypto::Aes256EcbEncryptor crypt;
	crypt.initialize(key, key_size);
	crypt.decrypt(dst, src, size);
}