#pragma once
#include "ITestClass.h"

#include <tc/Optional.h>

class Optional_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void testDefaultConstructor();
	void testWrapConstructor();
	void testCopyConstructorFromNullOptional();
	void testCopyConstructorFromExistingOptional();
	void testWrapOperator();
	void testCopyOperatorFromNullOptional();
	void testCopyOperatorFromExistingOptional();
	void testMakeNullOnNullOptional();
	void testMakeNullOnExistingOptional();
};