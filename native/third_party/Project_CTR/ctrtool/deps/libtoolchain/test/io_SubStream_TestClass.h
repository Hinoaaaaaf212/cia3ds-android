#pragma once
#include "ITestClass.h"

#include <tc/io.h>
#include <tc/ArgumentOutOfRangeException.h>
#include <tc/NotImplementedException.h>

class io_SubStream_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void testProperties();
	void testSize();
	void testSeekPos();
	void testRead();
	void testWrite();
};