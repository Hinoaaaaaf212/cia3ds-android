#include <tc/crypto/Pbkdf2Sha512KeyDeriver.h>

void tc::crypto::DeriveKeyPbkdf2Sha512(byte_t* key, size_t key_size, const byte_t* password, size_t password_size, const byte_t* salt, size_t salt_size, size_t n_rounds)
{
	tc::crypto::Pbkdf2Sha512KeyDeriver impl;
	impl.initialize(password, password_size, salt, salt_size, n_rounds);
	impl.getBytes(key, key_size);
}