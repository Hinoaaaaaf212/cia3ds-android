#pragma once
#include "ITestClass.h"
#include <tc/ByteData.h>

class ByteData_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_Constructor_DefaultConstructor();
	void test_Constructor_InitializerList();
	void test_Constructor_CreateZeroSized();
	void test_Constructor_CreateSmallSized();
	void test_Constructor_CreateLargeSized();
	void test_Constructor_ThrowExceptForBadAlloc();
	void test_Constructor_CreateFromPtr();
	void test_ImplicitCopy_CopyInSameScope();
	void test_ImplicitCopy_CopyOntoInitiallisedByteData();
	void test_ImplicitMove_CopyInSameScope();
	void test_ImplicitMove_MoveOntoInitiallisedByteData();
	void test_EqualityOperator();
	void test_InequalityOperator();
};