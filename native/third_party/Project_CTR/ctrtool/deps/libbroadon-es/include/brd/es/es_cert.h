#pragma once
#include <brd/es/es_sign.h>

namespace brd { namespace es {
    /**
     @enum ESCertPubKeyType
     @brief ES Certificate public key type definition
     */
enum class ESCertPubKeyType : uint32_t
{
    RSA4096 = 0, /* RSA 4096 bit key */
    RSA2048 = 1, /* RSA 2048 bit key */
    ECC = 2, /* ECC pub key 512 bits */
};

static const size_t ES_CERT_NAME_SIZE = 64;
using ESCertName = tc::bn::string<ES_CERT_NAME_SIZE>;
using ESServerId = ESCertName;
using ESDeviceId = ESCertName;

template <size_t _size>
using ESPubKeyPad = std::array<byte_t, _size>; 

/* pack to 4 byte boundaries */
#pragma pack(push,4)

struct ESCertHeader
{
    tc::bn::be32<ESCertPubKeyType> pubKeyType; // ESCertPubKeyType
    union {
        ESServerId serverId;
        ESDeviceId deviceId;
    } name;
    tc::bn::be32<uint32_t> date; // unix time-stamp
};
static_assert(sizeof(ESCertHeader) == 72, "ESCertHeader size");

struct ESCertRsa2048PublicKey
{
	Rsa2048PublicKey      pubKey;
    ESPubKeyPad<52>       pad;
};
static_assert(sizeof(ESCertRsa2048PublicKey) == 312, "ESCertRsa2048PublicKey size");

struct ESCertRsa4096PublicKey
{
	Rsa4096PublicKey      pubKey;
    ESPubKeyPad<52>       pad;
};
static_assert(sizeof(ESCertRsa4096PublicKey) == 568, "ESCertRsa4096PublicKey size");

struct ESCertEcc233PublicKey
{
	Ecc233PublicKey       pubKey;
    ESPubKeyPad<60>       pad;
};
static_assert(sizeof(ESCertEcc233PublicKey) == 120, "ESCertEcc233PublicKey size");

struct ESRootCert
{
	ESSigRsa4096             sig;
	ESCertHeader             head;
	ESCertRsa4096PublicKey   body;
};
static_assert(sizeof(ESRootCert) == 1280, "ESRootCert size");

struct ESCACert
{
	ESSigRsa4096             sig;
	ESCertHeader             head;
	ESCertRsa2048PublicKey   body;
};
static_assert(sizeof(ESCACert) == 1024, "ESCACert size");

struct ESCASignedCert
{
	ESSigRsa2048             sig;
	ESCertHeader             head;
	ESCertRsa2048PublicKey   body;
};
static_assert(sizeof(ESCASignedCert) == 768, "ESCASignedCert size");

struct ESDeviceCert
{
	ESSigRsa2048             sig;
	ESCertHeader             head;
	ESCertEcc233PublicKey    body;
};
static_assert(sizeof(ESDeviceCert) == 576, "ESDeviceCert size");

struct ESDeviceSignedCert
{
	ESSigEcc233              sig;
	ESCertHeader             head;
	ESCertEcc233PublicKey    body;
};
static_assert(sizeof(ESDeviceSignedCert) == 384, "ESDeviceSignedCert size");

#pragma pack(pop)

}} // namespace brd::es
