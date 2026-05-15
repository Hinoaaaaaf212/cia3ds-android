#pragma once
#include "ITestClass.h"

#include <tc/bn.h>
#include <tc/ArgumentOutOfRangeException.h>
#include <tc/NotImplementedException.h>

class bn_binaryutils_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_RoundUpFunc();
	void util_RoundUpFuncTestCase(uint32_t value, uint32_t alignment, uint32_t expected_result);

	void test_AlignFunc();
	void util_AlignFuncTestCase(uint32_t value, uint32_t alignment, uint32_t expected_result);

	void test_MakeStructMagicU32Func();
	void util_MakeStructMagicU32FuncTestCase(const char* struct_magic_str, uint32_t expected_result);

	void test_MakeStructMagicU64Func();
	void util_MakeStructMagicU64FuncTestCase(const char* struct_magic_str, uint64_t expected_result);
	
};