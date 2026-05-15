#include <ntd/n3ds/es/Signature.h>
#include <fmt/core.h>

#include <brd/es/es_cert.h>
#include <tc/ByteData.h>
#include <tc/crypto/Sha256Generator.h>

#include <tc/cli.h>

size_t ntd::n3ds::es::getSignatureSizeFromSigType(brd::es::ESSigType sig_type)
{
	size_t signature_size = 0;
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
			signature_size = 0;
	}

	return signature_size;
}

size_t ntd::n3ds::es::getSignatureIssuerOffset(brd::es::ESSigType sig_type)
{
	size_t issuer_offset = 0;
	switch (sig_type)
	{
		case brd::es::ESSigType::RSA4096_SHA1:
		case brd::es::ESSigType::RSA4096_SHA256:
			issuer_offset = sizeof(brd::es::ESSigRsa4096) - sizeof(brd::es::ESIssuer);
			break;
		case brd::es::ESSigType::RSA2048_SHA1:
		case brd::es::ESSigType::RSA2048_SHA256:
			issuer_offset = sizeof(brd::es::ESSigRsa2048) - sizeof(brd::es::ESIssuer);
			break;
		case brd::es::ESSigType::ECC_SHA1:
		case brd::es::ESSigType::ECC_SHA256:
			issuer_offset = sizeof(brd::es::ESSigEcc233) - sizeof(brd::es::ESIssuer);
			break;
		default:
			issuer_offset = 0;
	}

	return issuer_offset;
}

ntd::n3ds::es::SignatureDeserialiser::SignatureDeserialiser(const std::shared_ptr<tc::io::IStream>& stream) :
	Signature(),
	mModuleLabel("ntd::n3ds::es::SignatureDeserialiser")
{
	if (stream == nullptr)
	{
		throw tc::ArgumentNullException(mModuleLabel, "Stream was null.");
	}

	// must have at least 4 bytes for signature magic code
	if (stream->length() < sizeof(uint32_t))
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "Stream was too small to import signature.");
	}

	// get signature
	enum class SignType {
		RSA4096,
		RSA2048,
		ECDSA233,
	};
	union SignatureSignature
	{
		tc::bn::be32<brd::es::ESSigType>    sigType;
		brd::es::ESSigRsa4096 rsa4096;
		brd::es::ESSigRsa2048 rsa2048;
		brd::es::ESSigEcc233 ecdsa233;
	} signature_data;
	size_t signature_size = 0;

	stream->seek(0, tc::io::SeekOrigin::Begin);
	if (stream->read((byte_t*)&signature_data, sizeof(uint32_t)) < sizeof(uint32_t))
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "Unexpected size after reading.");
	}
	switch (signature_data.sigType.unwrap())
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
			throw tc::ArgumentOutOfRangeException(mModuleLabel, "Unexpected signature type.");
	}
	stream->seek(0, tc::io::SeekOrigin::Begin);
	if (stream->read((byte_t*)&signature_data, signature_size) < signature_size)
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "Unexpected size after reading.");
	}

	// store properties
	this->sig_type = signature_data.sigType.unwrap();
	switch (signature_data.sigType.unwrap())
	{
		case brd::es::ESSigType::RSA4096_SHA1:
		case brd::es::ESSigType::RSA4096_SHA256:
			this->sig = tc::ByteData(signature_data.rsa4096.sig.data(), signature_data.rsa4096.sig.size());
			this->issuer = signature_data.rsa4096.issuer.decode();
			break;
		case brd::es::ESSigType::RSA2048_SHA1:
		case brd::es::ESSigType::RSA2048_SHA256:
			this->sig = tc::ByteData(signature_data.rsa2048.sig.data(), signature_data.rsa2048.sig.size());
			this->issuer = signature_data.rsa2048.issuer.decode();
			break;
		case brd::es::ESSigType::ECC_SHA1:
		case brd::es::ESSigType::ECC_SHA256:
			this->sig = tc::ByteData((byte_t*)&signature_data.ecdsa233.sig, sizeof(brd::es::Ecc233Sig));
			this->issuer = signature_data.ecdsa233.issuer.decode();
			break;
		default:
			break;
	}
}