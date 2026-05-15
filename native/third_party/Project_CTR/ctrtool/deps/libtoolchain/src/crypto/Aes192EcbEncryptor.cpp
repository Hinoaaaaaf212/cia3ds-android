#include <tc/crypto/Aes192EcbEncryptor.h>

void tc::crypto::EncryptAes192Ecb(byte_t* dst, const byte_t* src, size_t size, const byte_t* key, size_t key_size)
{
	tc::crypto::Aes192EcbEncryptor crypt;
	crypt.initialize(key, key_size);
	crypt.encrypt(dst, src, size);
}

void tc::crypto::DecryptAes192Ecb(byte_t* dst, const byte_t* src, size_t size, const byte_t* key, size_t key_size)
{
	tc::crypto::Aes192EcbEncryptor crypt;
	crypt.initialize(key, key_size);
	crypt.decrypt(dst, src, size);
}