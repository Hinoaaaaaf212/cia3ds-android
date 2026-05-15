#pragma once
#include <bitset>
#include <tc/types.h>
#include <tc/crypto/RsaKey.h>
#include <ntd/n3ds/es/ISigner.h>

#include <tc/InvalidOperationException.h>
#include <tc/ArgumentOutOfRangeException.h>

namespace ntd { namespace n3ds { namespace es {

class RsaSigner : public ntd::n3ds::es::ISigner
{
public:
	RsaSigner(brd::es::ESSigType sig_type, const std::string& issuer, const tc::crypto::RsaKey& rsa_key);
	
	const std::string& getIssuer();
	
	brd::es::ESSigType getSigType();
	
	bool signHash(const byte_t* hash, byte_t* signature);
	
	bool verifyHash(const byte_t* hash, const byte_t* signature);
private:
	brd::es::ESSigType mSigType;
	std::string mIssuer;
	tc::crypto::RsaKey mRsaKey;
};

}}} // namespace ntd::n3ds::es