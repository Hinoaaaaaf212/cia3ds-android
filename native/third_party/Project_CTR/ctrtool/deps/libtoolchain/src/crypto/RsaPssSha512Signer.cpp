#include <tc/crypto/RsaPssSha512Signer.h>

bool tc::crypto::SignRsa1024PssSha512(byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa1024PssSha512Signer impl;
	impl.initialize(key);
	return impl.sign(signature, message_digest);
}

bool tc::crypto::VerifyRsa1024PssSha512(const byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa1024PssSha512Signer impl;
	impl.initialize(key);
	return impl.verify(signature, message_digest);
}

bool tc::crypto::SignRsa2048PssSha512(byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa2048PssSha512Signer impl;
	impl.initialize(key);
	return impl.sign(signature, message_digest);
}

bool tc::crypto::VerifyRsa2048PssSha512(const byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa2048PssSha512Signer impl;
	impl.initialize(key);
	return impl.verify(signature, message_digest);
}

bool tc::crypto::SignRsa4096PssSha512(byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa4096PssSha512Signer impl;
	impl.initialize(key);
	return impl.sign(signature, message_digest);
}

bool tc::crypto::VerifyRsa4096PssSha512(const byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa4096PssSha512Signer impl;
	impl.initialize(key);
	return impl.verify(signature, message_digest);
}