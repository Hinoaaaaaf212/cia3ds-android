#include <ntd/n3ds/es/RsaSigner.h>

#include <tc/crypto/RsaPkcs1Sha1Signer.h>
#include <tc/crypto/RsaPkcs1Sha256Signer.h>

ntd::n3ds::es::RsaSigner::RsaSigner(brd::es::ESSigType sig_type, const std::string& issuer, const tc::crypto::RsaKey& rsa_key) :
	mSigType(sig_type),
	mIssuer(issuer),
	mRsaKey(rsa_key)
{
	switch (mSigType) {
		case brd::es::ESSigType::RSA4096_SHA1:
		case brd::es::ESSigType::RSA4096_SHA256:
			if ((mRsaKey.n.size() << 3) != 4096) throw tc::ArgumentOutOfRangeException("ntd::n3ds::es::RsaSigner::RsaSigner()", "Key size inferred from SigType did not match actual key size.");
			break;
		case brd::es::ESSigType::RSA2048_SHA1:
		case brd::es::ESSigType::RSA2048_SHA256:
			if ((mRsaKey.n.size() << 3) != 2048) throw tc::ArgumentOutOfRangeException("ntd::n3ds::es::RsaSigner::RsaSigner()", "Key size inferred from SigType did not match actual key size.");
			break;
		default:
			throw tc::ArgumentOutOfRangeException("ntd::n3ds::es::RsaSigner::RsaSigner()", "SigType not supported for RsaSigner.");
			break;
	}
}

const std::string& ntd::n3ds::es::RsaSigner::getIssuer()
{
	return mIssuer;
}

brd::es::ESSigType ntd::n3ds::es::RsaSigner::getSigType()
{
	return mSigType;
}

bool ntd::n3ds::es::RsaSigner::signHash(const byte_t* hash, byte_t* signature)
{
	bool signSucceed = false;

	switch (mSigType) {
		case brd::es::ESSigType::RSA4096_SHA1:
			signSucceed = tc::crypto::SignRsa4096Pkcs1Sha1(signature, hash, mRsaKey);
			break;
		case brd::es::ESSigType::RSA2048_SHA1:
			signSucceed = tc::crypto::SignRsa2048Pkcs1Sha1(signature, hash, mRsaKey);
			break;
		case brd::es::ESSigType::RSA4096_SHA256:
			signSucceed = tc::crypto::SignRsa4096Pkcs1Sha256(signature, hash, mRsaKey);
			break;
		case brd::es::ESSigType::RSA2048_SHA256:
			signSucceed = tc::crypto::SignRsa2048Pkcs1Sha256(signature, hash, mRsaKey);
			break;
		default:
			signSucceed = false;
			break;
	}

	return signSucceed;
}

bool ntd::n3ds::es::RsaSigner::verifyHash(const byte_t* hash, const byte_t* signature)
{
	bool verifySucceed = false;

	switch (mSigType) {
		case brd::es::ESSigType::RSA4096_SHA1:
			verifySucceed = tc::crypto::VerifyRsa4096Pkcs1Sha1(signature, hash, mRsaKey);
			break;
		case brd::es::ESSigType::RSA2048_SHA1:
			verifySucceed = tc::crypto::VerifyRsa2048Pkcs1Sha1(signature, hash, mRsaKey);
			break;
		case brd::es::ESSigType::RSA4096_SHA256:
			verifySucceed = tc::crypto::VerifyRsa4096Pkcs1Sha256(signature, hash, mRsaKey);
			break;
		case brd::es::ESSigType::RSA2048_SHA256:
			verifySucceed = tc::crypto::VerifyRsa2048Pkcs1Sha256(signature, hash, mRsaKey);
			break;
		default:
			verifySucceed = false;
			break;
	}

	return verifySucceed;
}