#pragma once
#include "ITestClass.h"

class crypto_PseudoRandomByteGenerator_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_Class();
	void test_UtilFunc();

	void test_MultipleObjectsCreateDifferentData();
	void test_RepeatedCallsCreateDifferentData();
};