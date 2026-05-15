#include <tc/crypto/Aes128XtsEncryptor.h>

void tc::crypto::EncryptAes128Xts(byte_t* dst, const byte_t* src, size_t size, uint64_t sector_number, const byte_t* key1, size_t key1_size, const byte_t* key2, size_t key2_size, size_t sector_size, bool tweak_word_order)
{
	tc::crypto::Aes128XtsEncryptor crypt;
	crypt.initialize(key1, key1_size, key2, key2_size, sector_size, tweak_word_order);
	crypt.encrypt(dst, src, size, sector_number);
}

void tc::crypto::DecryptAes128Xts(byte_t* dst, const byte_t* src, size_t size, uint64_t sector_number, const byte_t* key1, size_t key1_size, const byte_t* key2, size_t key2_size, size_t sector_size, bool tweak_word_order)
{
	tc::crypto::Aes128XtsEncryptor crypt;
	crypt.initialize(key1, key1_size, key2, key2_size, sector_size, tweak_word_order);
	crypt.decrypt(dst, src, size, sector_number);
}