#include <tc/crypto/Aes192CtrEncryptor.h>

void tc::crypto::EncryptAes192Ctr(byte_t* dst, const byte_t* src, size_t size, uint64_t block_number, const byte_t* key, size_t key_size, const byte_t* iv, size_t iv_size)
{
	tc::crypto::Aes192CtrEncryptor crypt;
	crypt.initialize(key, key_size, iv, iv_size);
	crypt.encrypt(dst, src, size, block_number);
}

void tc::crypto::DecryptAes192Ctr(byte_t* dst, const byte_t* src, size_t size, uint64_t block_number, const byte_t* key, size_t key_size, const byte_t* iv, size_t iv_size)
{
	tc::crypto::Aes192CtrEncryptor crypt;
	crypt.initialize(key, key_size, iv, iv_size);
	crypt.decrypt(dst, src, size, block_number);
}

void tc::crypto::IncrementCounterAes192Ctr(byte_t* counter, uint64_t incr)
{
	if (counter == nullptr)
	{
		throw tc::ArgumentOutOfRangeException("counter was null.");
	}
	tc::crypto::detail::incr_counter<16>(counter, incr);
}