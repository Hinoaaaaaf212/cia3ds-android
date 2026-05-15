#include <ntd/n3ds/es/Certificate.h>
#include <fmt/core.h>

#include <brd/es/es_cert.h>
#include <tc/ByteData.h>
#include <tc/crypto/Sha256Generator.h>

#include <tc/cli.h>

size_t ntd::n3ds::es::getCertificateSize(byte_t* data)
{
	size_t signature_size = 0;
	size_t public_key_size = 0;

	if (data == nullptr) { return 0; }

	signature_size = getCertificateSignatureSize(data);
	if (signature_size == 0) { return 0; }

	brd::es::ESCertHeader* header = (brd::es::ESCertHeader*)(data + signature_size);
	brd::es::ESCertPubKeyType public_key_type = header->pubKeyType.unwrap();
	switch (public_key_type)
	{
		case brd::es::ESCertPubKeyType::RSA4096:
			public_key_size = sizeof(brd::es::ESCertRsa4096PublicKey);
			break;
		case brd::es::ESCertPubKeyType::RSA2048:
			public_key_size = sizeof(brd::es::ESCertRsa2048PublicKey);
			break;
		case brd::es::ESCertPubKeyType::ECC:
			public_key_size = sizeof(brd::es::ESCertEcc233PublicKey);
			break;
		default:
			return 0;
	}

	return signature_size + sizeof(brd::es::ESCertHeader) + public_key_size;
}

size_t ntd::n3ds::es::getCertificateSignatureSize(byte_t* data)
{
	size_t signature_size = 0;

	if (data == nullptr) { return 0; }

	brd::es::ESSigType sig_type = ((tc::bn::be32<brd::es::ESSigType>*)data)->unwrap();
	switch (sig_type)
	{
		case brd::es::ESSigType::RSA4096_SHA1:
		case brd::es::ESSigType::RSA4096_SHA256:
			signature_size = sizeof(brd::es::ESSigRsa4096);
			break;
		case brd::es::ESSigType::RSA2048_SHA1:
		case brd::es::ESSigType::RSA2048_SHA256:
			signature_size = sizeof(brd::es::ESSigRsa2048);
			break;
		case brd::es::ESSigType::ECC_SHA1:
		case brd::es::ESSigType::ECC_SHA256:
			signature_size = sizeof(brd::es::ESSigEcc233);
			break;
		default:
			return 0;
	}

	return signature_size;
}

size_t ntd::n3ds::es::getCertificateSignableOffset(byte_t* data)
{
	size_t signature_size = 0;

	if (data == nullptr) { return 0; }

	signature_size = getCertificateSignatureSize(data);
	if (signature_size == 0) { return 0; }

	return signature_size - sizeof(brd::es::ESIssuer);
}

size_t ntd::n3ds::es::getCertificateSignableSize(byte_t* data)
{
	return getCertificateSize(data) - getCertificateSignableOffset(data);
}

void* ntd::n3ds::es::getCertificateHeaderPtr(byte_t* data)
{
	size_t signature_size = 0;

	if (data == nullptr) { return nullptr; }

	signature_size = getCertificateSignatureSize(data);
	if (signature_size == 0) { return nullptr; }

	return (data + signature_size);
}

void* ntd::n3ds::es::getCertificatePublicKeyPtr(byte_t* data)
{
	size_t signature_size = 0;

	if (data == nullptr) { return nullptr; }

	signature_size = getCertificateSignatureSize(data);
	if (signature_size == 0) { return nullptr; }

	return (data + signature_size + sizeof(brd::es::ESCertHeader));
}

ntd::n3ds::es::CertificateDeserialiser::CertificateDeserialiser(const std::shared_ptr<tc::io::IStream>& cert_stream) :
	Certificate(),
	mModuleLabel("ntd::n3ds::es::CertificateDeserialiser")
{
	if (cert_stream == nullptr)
	{
		throw tc::ArgumentNullException(mModuleLabel, "Stream was null.");
	}

	// process signature
	this->signature = SignatureDeserialiser(cert_stream);
	switch (this->signature.sig_type)
	{
		case brd::es::ESSigType::RSA4096_SHA1:
		case brd::es::ESSigType::RSA4096_SHA256:
		case brd::es::ESSigType::RSA2048_SHA1:
		case brd::es::ESSigType::RSA2048_SHA256:
		case brd::es::ESSigType::ECC_SHA1:
		case brd::es::ESSigType::ECC_SHA256:
			break;
		default:
			throw tc::ArgumentOutOfRangeException(mModuleLabel, "CERT had unexpected signature type.");
	}
	size_t signature_size = getSignatureSizeFromSigType(this->signature.sig_type);

	// get certificate header
	brd::es::ESCertHeader header;
	cert_stream->seek(signature_size, tc::io::SeekOrigin::Begin);
	if (cert_stream->read((byte_t*)&header, sizeof(brd::es::ESCertHeader)) < sizeof(brd::es::ESCertHeader))
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "CERT had unexpected size after reading.");
	}

	// get public key
	union CertificatePublicKey
	{
		brd::es::ESCertRsa4096PublicKey rsa4096;
		brd::es::ESCertRsa2048PublicKey rsa2048;
		brd::es::ESCertEcc233PublicKey ecc233;
	} public_key;
	size_t public_key_size;

	switch (header.pubKeyType.unwrap())
	{
		case brd::es::ESCertPubKeyType::RSA4096:
			public_key_size = sizeof(brd::es::ESCertRsa4096PublicKey);
			break;
		case brd::es::ESCertPubKeyType::RSA2048:
			public_key_size = sizeof(brd::es::ESCertRsa2048PublicKey);
			break;
		case brd::es::ESCertPubKeyType::ECC:
			public_key_size = sizeof(brd::es::ESCertEcc233PublicKey);
			break;
		default:
			throw tc::ArgumentOutOfRangeException(mModuleLabel, "CERT had unexpected public key type.");
	}

	cert_stream->seek(signature_size + sizeof(brd::es::ESCertHeader), tc::io::SeekOrigin::Begin);
	if (cert_stream->read((byte_t*)&public_key, public_key_size) < public_key_size)
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "CERT had unexpected size after reading.");
	}

	// calculate hash for optional signature validation later
	tc::ByteData total_cert_data = tc::ByteData(signature_size + sizeof(brd::es::ESCertHeader) + public_key_size);
	cert_stream->seek(0, tc::io::SeekOrigin::Begin);
	if (cert_stream->read(total_cert_data.data(), total_cert_data.size()) < total_cert_data.size())
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "CERT had unexpected size after reading.");
	}
	tc::crypto::GenerateSha256Hash(calculated_hash.data(), total_cert_data.data() + getCertificateSignableOffset(total_cert_data.data()), ntd::n3ds::es::getCertificateSignableSize(total_cert_data.data()));


	// store properties
	//this->signature = SignatureDeserialiser(cert_stream);
	this->subject = header.name.deviceId.decode();
	this->date = header.date.unwrap();
	this->public_key_type = header.pubKeyType.unwrap();
	switch (header.pubKeyType.unwrap())
	{
		case brd::es::ESCertPubKeyType::RSA4096:
			memcpy(&this->rsa4096_public_key, &public_key.rsa4096.pubKey, sizeof(brd::es::Rsa4096PublicKey));
			break;
		case brd::es::ESCertPubKeyType::RSA2048:
			memcpy(&this->rsa2048_public_key, &public_key.rsa2048.pubKey, sizeof(brd::es::Rsa2048PublicKey));
			break;
		case brd::es::ESCertPubKeyType::ECC:
			memcpy(&this->ecc233_public_key, &public_key.ecc233.pubKey, sizeof(brd::es::Ecc233PublicKey));
			break;
	}
	
}