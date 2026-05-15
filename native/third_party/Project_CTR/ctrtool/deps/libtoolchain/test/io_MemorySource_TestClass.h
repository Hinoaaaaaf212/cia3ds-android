#pragma once
#include "ITestClass.h"

#include <tc/io/MemorySource.h>

class io_MemorySource_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void testDefaultConstructor();
	void testInitializeByCopyWithByteData();
	void testInitializeByMoveWithByteData();
	void testInitializeByCopyWithMemoryPointer();
	void testNegativeOffset();
	void testTooLargeOffset();
};
