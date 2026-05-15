#pragma once
#include <tc/types.h>
#include <tc/ByteData.h>

class RsaPkcs1Util
{
public:
	struct TestVector
	{
		std::string test_name;

		tc::ByteData key_modulus;
		tc::ByteData key_private_exponent;
		tc::ByteData message_digest;
		tc::ByteData signature;
	};

	enum HashAlgo
	{
		MD4,
		MD5,
		SHA1,
		SHA224,
		SHA256,
		SHA384,
		SHA512
	};

		/// Implementation of https://gist.github.com/jakcron/1c00ed37089743e38df46da2d9ccf6a0
	static void generateRsaPkcs1TestVectors_Custom(std::vector<RsaPkcs1Util::TestVector>& test_list, size_t key_size, RsaPkcs1Util::HashAlgo hash_algo);
};