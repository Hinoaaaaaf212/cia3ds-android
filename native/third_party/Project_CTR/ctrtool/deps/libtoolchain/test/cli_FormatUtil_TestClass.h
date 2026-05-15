#pragma once
#include "ITestClass.h"
#include <tc.h>

class cli_FormatUtil_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void testHexStringToBytes();
	void testFormatBytesAsString();
	void testFormatBytesAsStringWithLineLimit();
	void testFormatListWithLineLimit();
	void testFormatBytesAsHxdHexString();
};