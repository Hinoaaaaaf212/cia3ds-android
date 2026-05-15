#include <tc/crypto/RsaPkcs1Sha256Signer.h>

bool tc::crypto::SignRsa1024Pkcs1Sha256(byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa1024Pkcs1Sha256Signer impl;
	impl.initialize(key);
	return impl.sign(signature, message_digest);
}

bool tc::crypto::VerifyRsa1024Pkcs1Sha256(const byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa1024Pkcs1Sha256Signer impl;
	impl.initialize(key);
	return impl.verify(signature, message_digest);
}

bool tc::crypto::SignRsa2048Pkcs1Sha256(byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa2048Pkcs1Sha256Signer impl;
	impl.initialize(key);
	return impl.sign(signature, message_digest);
}

bool tc::crypto::VerifyRsa2048Pkcs1Sha256(const byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa2048Pkcs1Sha256Signer impl;
	impl.initialize(key);
	return impl.verify(signature, message_digest);
}

bool tc::crypto::SignRsa4096Pkcs1Sha256(byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa4096Pkcs1Sha256Signer impl;
	impl.initialize(key);
	return impl.sign(signature, message_digest);
}

bool tc::crypto::VerifyRsa4096Pkcs1Sha256(const byte_t* signature, const byte_t* message_digest, const RsaKey& key)
{
	tc::crypto::Rsa4096Pkcs1Sha256Signer impl;
	impl.initialize(key);
	return impl.verify(signature, message_digest);
}