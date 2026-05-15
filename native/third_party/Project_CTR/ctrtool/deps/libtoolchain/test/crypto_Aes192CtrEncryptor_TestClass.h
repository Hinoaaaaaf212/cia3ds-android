#pragma once
#include "ITestClass.h"

#include <vector>
#include <tc/ByteData.h>

class crypto_Aes192CtrEncryptor_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_Constants();
	void test_UseClassEnc();
	void test_UseClassDec();
	void test_UseUtilFuncEnc();
	void test_UseUtilFuncDec();

	void test_DoesNothingWhenNotInit();
	void test_InitializeThrowsExceptionOnBadInput();
	void test_EncryptThrowsExceptionOnBadInput();
	void test_DecryptThrowsExceptionOnBadInput();

	struct TestCase
	{
		std::string test_name;
		tc::ByteData key;
		tc::ByteData iv;
		uint64_t block_number;
		tc::ByteData plaintext;
		tc::ByteData ciphertext;
	};

	void util_Setup_TestCases(std::vector<crypto_Aes192CtrEncryptor_TestClass::TestCase>& test_cases);
};