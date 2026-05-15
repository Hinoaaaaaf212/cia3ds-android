#pragma once
#include "ITestClass.h"

class crypto_Pbkdf2Sha256KeyDeriver_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_Constants();
	void test_ConfirmTestVector_Class();
	void test_ConfirmTestVector_UtilFunc();
	void test_WillThrowExceptionOnZeroRounds();
	void test_WillThrowExceptionOnTooLargeDkSize();
	void test_GetBytesWithoutInitDoesNothing();
};