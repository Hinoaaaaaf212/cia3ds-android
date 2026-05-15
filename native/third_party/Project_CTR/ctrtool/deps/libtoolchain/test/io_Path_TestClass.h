#pragma once
#include "ITestClass.h"
#include <tc/io.h>
#include <tc/string.h>

class io_Path_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_Constructor_Default();
	void test_Constructor_InitializerList();
	void test_Constructor_u8string();
	void test_Constructor_u16string();
	void test_Constructor_u32string();
	void test_Method_pop_front();
	void test_Method_pop_back();
	void test_Method_push_front();
	void test_Method_push_back();
	void test_Method_clear();
	void test_Method_substr_withPosLen();
	void test_Method_substr_withIterator();
	void test_Method_to_string();
	void test_Method_to_u16string();
	void test_Method_to_u32string();
	void test_Operator_string();
	void test_Operator_u16string();
	void test_Operator_u32string();
	void test_Operator_Addition();
	void test_Operator_Append();
	void test_Scenario_AppendStressTest();
	void test_Operator_EqualityTest();
	void test_Operator_InequalityTest();
	void test_Operator_LessThanTest();
};