#pragma once
#include "ITestClass.h"

class bn_endian_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void testLocalBSwap16();
	void testLocalBSwap32();
	void testLocalBSwap64();
	void testBeUint64Inline();
	void testBeUint32Inline();
	void testBeUint16Inline();
	void testLeUint64Inline();
	void testLeUint32Inline();
	void testLeUint16Inline();

	void testBeSwap64Inline();
	void testBeSwap32Inline();
	void testBeSwap16Inline();
	void testLeSwap64Inline();
	void testLeSwap32Inline();
	void testLeSwap16Inline();

	void testBe64TemplateClass();
	void testBe32TemplateClass();
	void testBe16TemplateClass();
	void testLe64TemplateClass();
	void testLe32TemplateClass();
	void testLe16TemplateClass();
};