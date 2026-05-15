#pragma once
#include "ITestClass.h"

#include <tc/crypto.h>
#include <tc/ArgumentOutOfRangeException.h>
#include <tc/NotImplementedException.h>

class crypto_Aes128CbcEncryptedStream_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_CreateEmptyStream_DefaultConstructor();
	void test_CreateValidStream_CreateConstructor();
	void test_RunTestCases();

	struct TestCase
	{
		std::string test_name;
		tc::ByteData key;
		tc::ByteData iv;
		tc::ByteData ciphertext;
		int64_t read_offset;
		size_t read_size;
		tc::ByteData read_plaintext;
	};

	void util_Setup_TestCases(std::vector<crypto_Aes128CbcEncryptedStream_TestClass::TestCase>& test_cases);
};