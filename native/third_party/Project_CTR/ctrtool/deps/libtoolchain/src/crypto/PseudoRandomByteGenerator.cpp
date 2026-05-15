#include <tc/crypto/PseudoRandomByteGenerator.h>

void tc::crypto::GeneratePseudoRandomBytes(byte_t* data, size_t data_size)
{
	tc::crypto::PseudoRandomByteGenerator impl;
	impl.getBytes(data, data_size);
}