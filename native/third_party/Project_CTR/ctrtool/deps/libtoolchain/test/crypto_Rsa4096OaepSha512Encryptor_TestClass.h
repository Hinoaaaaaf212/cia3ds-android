#pragma once
#include "ITestClass.h"

#include <vector>
#include <tc/ByteData.h>

class crypto_Rsa4096OaepSha512Encryptor_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_Constants();
	void test_UseClassDec();
	void test_UseClassEnc();
	void test_UseUtilFuncDec();
	void test_UseUtilFuncEnc();
	void test_UnspecifiedSeedProducesDifferentBlock();

	void test_DoesNothingWhenNotInit();
	void test_InitializeThrowsExceptionOnBadInput();
	void test_EncryptReturnsFalseOnBadInput();
	void test_DecryptReturnsFalseOnBadInput();
};