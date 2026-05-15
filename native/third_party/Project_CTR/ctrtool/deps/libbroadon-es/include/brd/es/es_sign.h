#pragma once
#include <brd/es/types.h>

namespace brd { namespace es {
	/**
     @enum ESSigType
     @brief ES Signature type definition
     */
enum class ESSigType : uint32_t
{
    RSA4096_SHA1 = 0x00010000, /* RSA 4096 bit signature */
    RSA2048_SHA1 = 0x00010001, /* RSA 2048 bit signature */
    ECC_SHA1 = 0x00010002, /* ECC signature 512 bits */
    RSA4096_SHA256 = 0x00010003, /* RSA 4096 bit sig using SHA-256 */
    RSA2048_SHA256 = 0x00010004, /* RSA 2048 bit sig using SHA-256 */ // note that Switch Ticket has this word swapped
    ECC_SHA256 = 0x00010005, /* ECC sig 512 bits using SHA-256 */
    HMAC_SHA1 = 0x00010006, /* HMAC-SHA1 160 bit signature */
};

static const size_t ES_ISSUER_SIZE = 64;

	/**
	 * @class ESIssuer
	 * @brief The signature issuer ASCII encoded certificate hierarchy. Padded with nulls.
	 * 
	 * Examples: 
	 * Root (issued by Root)
	 * Root-CAxxxxxxxx (issued by Certifcate Authority server xxxxxxxx)
	 * Root-CAxxxxxxxx-XSxxxxxxxx (issued by Ticket/Transaction server xxxxxxxx)
	 * Root-CAxxxxxxxx-CPxxxxxxxx (issued by Content Publishing server xxxxxxxx)
	 * Root-CAxxxxxxxx-MSxxxxxxxx (issued by Manufacturing server xxxxxxxx)
	 * Root-CAxxxxxxxx-MSxxxxxxxx-YYxxxxxxxx (issued by Device with of type YY and serial number xxxxxxxx)
	 * 
	 * xxxxxxxx represents the server/device serial number encoded in hex. (e.g. XS0000000f is ticket server 15).
	 */
using ESIssuer = tc::bn::string<ES_ISSUER_SIZE>;  

template <size_t _size>
using ESSigPad = tc::bn::pad<_size>; 

/* pack to 4 byte boundaries */
#pragma pack(push,4)

struct ESSigRsa2048
{
	tc::bn::be32<ESSigType>    sigType;
	Rsa2048Sig     sig;
	ESSigPad<60>   pad;
	ESIssuer       issuer;
};
static_assert(sizeof(ESSigRsa2048) == 384, "ESSigRsa2048 size");

struct ESSigRsa4096
{
	tc::bn::be32<ESSigType>    sigType;
	Rsa4096Sig    sig;
	ESSigPad<60>  pad;
	ESIssuer      issuer;
};
static_assert(sizeof(ESSigRsa4096) == 640, "ESSigRsa4096 size");

struct ESSigEcc233
{
	tc::bn::be32<ESSigType>    sigType;
	Ecc233Sig     sig;
	ESSigPad<64>  pad;
	ESIssuer      issuer;
};
static_assert(sizeof(ESSigEcc233) == 192, "ESSigEcc233 size");

#pragma pack(pop)

}} // namespace brd::es