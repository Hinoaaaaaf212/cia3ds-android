#pragma once
#include <tc/types.h>
#include <tc/ByteData.h>

class RsaOaepUtil
{
public:
	struct TestVector
	{
		std::string test_name;

		tc::ByteData key_modulus;
		tc::ByteData key_private_exponent;
		tc::ByteData label;
		bool         label_is_digested;
		tc::ByteData dec_message;
		tc::ByteData enc_seed;
		tc::ByteData enc_message;
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
	static void generateRsaOaepTestVectors_Custom(std::vector<RsaOaepUtil::TestVector>& test_list, size_t key_size, RsaOaepUtil::HashAlgo hash_algo);
};