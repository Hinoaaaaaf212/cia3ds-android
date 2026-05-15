#pragma once
#include "ITestClass.h"

class io_PaddingSource_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void testDefaultConstructor();
	void testCreateConstructor();
	void testNegativeOffset();
	void testTooLargeOffset();
};
