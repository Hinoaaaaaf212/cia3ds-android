#pragma once
#include <bitset>
#include <tc/types.h>
#include <tc/io/IStream.h>
#include <tc/crypto/RsaKey.h>
#include <brd/es/es_sign.h>

#include <tc/ArgumentNullException.h>
#include <tc/ArgumentOutOfRangeException.h>
#include <tc/InvalidOperationException.h>

namespace ntd { namespace n3ds { namespace es {

size_t getSignatureSizeFromSigType(brd::es::ESSigType sig_type);
size_t getSignatureIssuerOffset(brd::es::ESSigType sig_type);

struct Signature
{
public:
	Signature() :
		sig_type(),
		sig(),
		issuer()
	{}
public:
	brd::es::ESSigType sig_type;
	tc::ByteData sig;
	std::string issuer;
};

class SignatureDeserialiser : public Signature
{
public:
	// input stream
	SignatureDeserialiser(const std::shared_ptr<tc::io::IStream>& stream);
private:
	std::string mModuleLabel;
};

/*
class SignatureSerialiser : public tc::io::IStream
{
public:
	SignatureSerialiser();
private:
	std::string mModuleLabel;
}
*/

}}} // namespace ntd::n3ds::es