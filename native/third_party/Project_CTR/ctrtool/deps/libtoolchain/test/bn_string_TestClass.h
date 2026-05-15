#pragma once
#include "ITestClass.h"

#include <tc/bn.h>
#include <tc/ArgumentOutOfRangeException.h>
#include <tc/NotImplementedException.h>

class bn_string_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_EncodedSizeVersusLogicalSize();
	void test_StringSizeNeverExceedsLogicalSize();
	void test_EncodeStringRespectsLogicalSize();
};