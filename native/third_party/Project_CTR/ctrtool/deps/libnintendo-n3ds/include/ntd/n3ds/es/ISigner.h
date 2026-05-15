#pragma once
#include <bitset>
#include <tc/types.h>
#include <brd/es/es_sign.h>

namespace ntd { namespace n3ds { namespace es {

class ISigner
{
public:
	virtual ~ISigner() = default;
	virtual const std::string& getIssuer() = 0;
	virtual brd::es::ESSigType getSigType() = 0;
	virtual bool signHash(const byte_t* hash, byte_t* signature) = 0;
	virtual bool verifyHash(const byte_t* hash, const byte_t* signature) = 0;
};

}}} // namespace ntd::n3ds::es