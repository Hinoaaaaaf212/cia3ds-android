#include <tc/crypto/Sha512Generator.h>

const std::array<byte_t, tc::crypto::Sha512Generator::kAsn1OidDataSize> tc::crypto::Sha512Generator::kAsn1OidData = {0x30, 0x51, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03, 0x05, 0x00, 0x04, 0x40};

void tc::crypto::GenerateSha512Hash(byte_t* hash, const byte_t* data, size_t data_size)
{
	tc::crypto::Sha512Generator impl;
	impl.initialize();
	impl.update(data, data_size);
	impl.getHash(hash);
}