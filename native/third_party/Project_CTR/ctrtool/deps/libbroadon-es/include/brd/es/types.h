#pragma once
#include <tc/types.h>

namespace brd { namespace es {

#pragma pack(push,4)

static const size_t kAes128KeySize = 16;
using Aes128Key = std::array<uint8_t, kAes128KeySize>;

static const size_t kAes192KeySize = 24;
using Aes192Key = std::array<uint8_t, kAes192KeySize>;

static const size_t kAes256KeySize = 32;
using Aes256Key = std::array<uint8_t, kAes256KeySize>;

static const size_t kSha1Size = 20;
using Sha1Hash = std::array<uint8_t, kSha1Size>;
using Sha1Hmac = std::array<uint8_t, kSha1Size>;

static const size_t kSha256Size = 32;
using Sha256Hash = std::array<uint8_t, kSha256Size>;
using Sha256Hmac = std::array<uint8_t, kSha256Size>;

static const size_t kRsaPublicExponentSize = 4;
using RsaPublicExponent = std::array<uint8_t, kRsaPublicExponentSize>;

static const size_t kRsa2048Size = 0x100;
using Rsa2048Integer = std::array<uint8_t, kRsa2048Size>;
struct Rsa2048PublicKey
{
	Rsa2048Integer m; // modulus
	RsaPublicExponent e; // public_exponent
};
struct Rsa2048PrivateKey
{
	Rsa2048Integer m; // modulus
	Rsa2048Integer d; // private_exponent
};
using Rsa2048Sig = Rsa2048Integer;

static const size_t kRsa4096Size = 0x200;
using Rsa4096Integer = std::array<uint8_t, kRsa4096Size>;
struct Rsa4096PublicKey
{
	Rsa4096Integer m; // modulus
	RsaPublicExponent e; // public_exponent
};
struct Rsa4096PrivateKey
{
	Rsa4096Integer m; // modulus
	Rsa4096Integer d; // private_exponent
};
using Rsa4096Sig = Rsa4096Integer;

static const size_t kEcc233Size = 60;
using Ecc233Integer = std::array<uint8_t, kEcc233Size / 2>;
struct Ecc233Point
{
	Ecc233Integer x;
	Ecc233Integer y;
};
using Ecc233PrivateKey = Ecc233Integer;
using Ecc233PublicKey = Ecc233Point;
struct Ecc233Sig
{
	Ecc233Integer r;
	Ecc233Integer s;
};

#pragma pack(pop)

}} // namespace brd::es