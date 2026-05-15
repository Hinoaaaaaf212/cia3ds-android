#pragma once
#include <bitset>
#include <tc/types.h>
#include <tc/io/IStream.h>
#include <tc/crypto/RsaKey.h>
#include <brd/es/es_cert.h>
#include <ntd/n3ds/es/Signature.h>

#include <tc/ArgumentNullException.h>
#include <tc/ArgumentOutOfRangeException.h>
#include <tc/InvalidOperationException.h>

namespace ntd { namespace n3ds { namespace es {

	/**
	 * @brief Get total size of certificate, from raw certificate blob.
	 * 
	 * @param[in] data Raw certificate data.
	 * 
	 * @return Size in bytes of certificate data, 0 if invalid data.
	 */
size_t getCertificateSize(byte_t* data);

	/**
	 * @brief Get total size of certificate signature structure, from raw certificate blob.
	 * 
	 * @param[in] data Raw certificate data.
	 * 
	 * @return Size in bytes of certificate signature structure, 0 if invalid data.
	 */
size_t getCertificateSignatureSize(byte_t* data);

	/**
	 * @brief Get offset of certificate signed data, from raw certificate blob.
	 * 
	 * @param[in] data Raw certificate data.
	 * 
	 * @return Offset in bytes of signed certificate data, 0 if invalid data.
	 */
size_t getCertificateSignableOffset(byte_t* data);

	/**
	 * @brief Get size of certificate signed data, from raw certificate blob.
	 * 
	 * @param[in] data Raw certificate data.
	 * 
	 * @return Size in bytes of signed certificate data, 0 if invalid data.
	 */
size_t getCertificateSignableSize(byte_t* data);

	/**
	 * @brief Get pointer to certificate header, from raw certificate blob.
	 * 
	 * @param[in] data Raw certificate data.
	 * 
	 * @return Void pointer to certificate header. See @ref ntd::es::ESCertHeader.
	 */
void* getCertificateHeaderPtr(byte_t* data);

	/**
	 * @brief Get pointer to certificate public key, from raw certificate blob.
	 * 
	 * @param[in] data Raw certificate data.
	 * 
	 * @return Void pointer to certificate public. See @ref ntd::es::ESCertRsa4096PublicKey @ref ntd::es::ESCertRsa2048PublicKey @ref ntd::es::ESCertEcc233PublicKey.
	 */
void* getCertificatePublicKeyPtr(byte_t* data);

struct Certificate
{
public:
	Certificate() :
		signature(),
		subject(),
		date(),
		public_key_type(),
		rsa4096_public_key(),
		rsa2048_public_key(),
		ecc233_public_key()
	{
		memset(calculated_hash.data(), 0, calculated_hash.size());
	}
public:
	// these fields are only used when deserialised
	ntd::n3ds::es::Signature signature; // This includes the signature type, signature data, and issuer
	std::array<byte_t, 32> calculated_hash; // This hash is calculated when deserialised so that signature validation can be performed.

	// these fields are used in both deserialisation & serialisation
	std::string subject;
	uint32_t date; // 32bit unix timestamp, not always set
	// public key
	brd::es::ESCertPubKeyType public_key_type;
	brd::es::Rsa4096PublicKey rsa4096_public_key;
	brd::es::Rsa2048PublicKey rsa2048_public_key;
	brd::es::Ecc233PublicKey ecc233_public_key;
};

class CertificateDeserialiser : public Certificate
{
public:
	// cert stream
	CertificateDeserialiser(const std::shared_ptr<tc::io::IStream>& cert_stream);
private:
	std::string mModuleLabel;
};

/*
class CertificateSerialiser : public tc::io::IStream
{
public:
	CertificateSerialiser();
private:
	std::string mModuleLabel;
}
*/

}}} // namespace ntd::n3ds::es