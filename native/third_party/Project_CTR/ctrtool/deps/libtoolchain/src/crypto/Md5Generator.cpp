#include <tc/crypto/Md5Generator.h>

const std::array<byte_t, tc::crypto::Md5Generator::kAsn1OidDataSize> tc::crypto::Md5Generator::kAsn1OidData = {0x30, 0x20, 0x30, 0x0C, 0x06, 0x08, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x02, 0x05, 0x05, 0x00, 0x04, 0x10};

void tc::crypto::GenerateMd5Hash(byte_t* hash, const byte_t* data, size_t data_size)
{
	tc::crypto::Md5Generator impl;
	impl.initialize();
	impl.update(data, data_size);
	impl.getHash(hash);
}