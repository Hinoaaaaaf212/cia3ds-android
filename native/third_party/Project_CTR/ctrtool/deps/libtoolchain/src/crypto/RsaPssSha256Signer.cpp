#include <tc/crypto/RsaPssSha256Signer.h>

bool tc::crypto::SignRsa1024PssSha256(byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa1024PssSha256Signer impl;
	impl.initialize(key);
	return impl.sign(signature, message_digest);
}

bool tc::crypto::VerifyRsa1024PssSha256(const byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa1024PssSha256Signer impl;
	impl.initialize(key);
	return impl.verify(signature, message_digest);
}

bool tc::crypto::SignRsa2048PssSha256(byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa2048PssSha256Signer impl;
	impl.initialize(key);
	return impl.sign(signature, message_digest);
}

bool tc::crypto::VerifyRsa2048PssSha256(const byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa2048PssSha256Signer impl;
	impl.initialize(key);
	return impl.verify(signature, message_digest);
}

bool tc::crypto::SignRsa4096PssSha256(byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa4096PssSha256Signer impl;
	impl.initialize(key);
	return impl.sign(signature, message_digest);
}

bool tc::crypto::VerifyRsa4096PssSha256(const byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa4096PssSha256Signer impl;
	impl.initialize(key);
	return impl.verify(signature, message_digest);
}