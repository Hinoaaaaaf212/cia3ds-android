#pragma once
#include "ITestClass.h"

class crypto_Sha1Generator_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_Constants();
	void test_SingleUpdateCall();
	void test_MultiUpdateCall();
	void test_UtilFunc();

	void test_NoInitNoUpdateDoHash();
	void test_NoInitDoUpdateDoHash();
	void test_DoInitNoUpdateDoHash();
	void test_CallGetHashRepeatedly();
};