#include <tc/crypto/Sha1Generator.h>

const std::array<byte_t, tc::crypto::Sha1Generator::kAsn1OidDataSize> tc::crypto::Sha1Generator::kAsn1OidData = {0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E, 0x03, 0x02, 0x1A, 0x05, 0x00, 0x04, 0x14};


void tc::crypto::GenerateSha1Hash(byte_t* hash, const byte_t* data, size_t data_size)
{
	tc::crypto::Sha1Generator impl;
	impl.initialize();
	impl.update(data, data_size);
	impl.getHash(hash);
}