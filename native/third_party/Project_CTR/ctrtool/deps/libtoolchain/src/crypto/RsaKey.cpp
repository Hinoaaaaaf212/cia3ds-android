#include <tc/crypto/RsaKey.h>

tc::crypto::RsaPublicKey::RsaPublicKey(const byte_t* modulus, size_t modulus_size)
{
	static const byte_t kPublicExponent[3] = { 0x01, 0x00, 0x01 };

	if (modulus != nullptr && modulus_size != 0)
	{
		this->n = tc::ByteData(modulus, modulus_size);
		this->e = tc::ByteData(kPublicExponent, sizeof(kPublicExponent));
	}
}

tc::crypto::RsaPrivateKey::RsaPrivateKey(const byte_t* modulus, size_t modulus_size, const byte_t* private_exponent, size_t private_exponent_size)
{
	static const byte_t kPublicExponent[3] = { 0x01, 0x00, 0x01 };

	if (modulus != nullptr && modulus_size != 0 && private_exponent != nullptr && private_exponent_size != 0)
	{
		this->n = tc::ByteData(modulus, modulus_size);
		this->d = tc::ByteData(private_exponent, private_exponent_size);
		this->e = tc::ByteData(kPublicExponent, sizeof(kPublicExponent));
	}
}

tc::crypto::RsaKey tc::crypto::RsaPrivateKey::getPublicKey()
{
	return RsaPublicKey(this->n.data(), this->n.size());
}