#pragma once
#include "ITestClass.h"
#include <tc/cli/OptionParser.h>

class cli_OptionParser_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_Constructor_DefaultConstructor();
	void test_ProcessNoOptionsWithNoHandlers();
	void test_ProcessOptionsWithNoHandlers();
	void test_ProcessOptionsWithOnlyUnkHandler();
	void test_ProcessOptionsWithLiteralHandlers();
	void test_ProcessOptionsWithRegexHandlers();
	void test_ProcessOptionsWithLiteralAndRegexHandlers();
	void test_NullHandlerSupplied();
	void test_RegularHandlerProvidesNoOptionLiteralOrRegex();
	void test_ProcessMalformedOptions();
};