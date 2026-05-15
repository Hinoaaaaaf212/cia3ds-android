#pragma once
#include <tc/types.h>
#include <tc/ByteData.h>

class PbkdfUtil
{
public:
	struct TestVector
	{
		std::string test_name;

		std::string in_password;
		std::string in_salt;
		size_t in_rounds;
		size_t in_dk_len;
		tc::ByteData out_dk;
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

	static void generatePbkdf1TestVectors_Custom(std::vector<PbkdfUtil::TestVector>& test_list, PbkdfUtil::HashAlgo hash_algo);

		/// Implementation of https://gist.github.com/jakcron/ab05a200e5a6b53c77f28ebfc6342885
	static void generatePbkdf2TestVectors_RFC6070(std::vector<PbkdfUtil::TestVector>& test_list, PbkdfUtil::HashAlgo hash_algo);
};