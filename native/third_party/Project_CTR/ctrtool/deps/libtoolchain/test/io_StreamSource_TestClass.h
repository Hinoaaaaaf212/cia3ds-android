#pragma once
#include "ITestClass.h"

class io_StreamSource_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void testDefaultConstructor();
	void testCreateConstructor();
	void testCreateFromStreamWithoutSeek();
	void testCreateFromStreamWithoutRead();
	void testCreateFromStreamWithoutWrite();
	void testNegativeOffset();
	void testTooLargeOffset();
};
