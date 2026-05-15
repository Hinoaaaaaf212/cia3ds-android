#pragma once
#include "ITestClass.h"

#include <vector>
#include <tc/ByteData.h>

class crypto_Rsa1024Pkcs1Sha256Signer_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_Constants();
	void test_UseClassSign();
	void test_UseClassVerify();
	void test_UseUtilFuncSign();
	void test_UseUtilFuncVerify();

	void test_DoesNothingWhenNotInit();
	void test_InitializeThrowsExceptionOnBadInput();
	void test_SignReturnsFalseOnBadInput();
	void test_VerifyReturnsFalseOnBadInput();
};