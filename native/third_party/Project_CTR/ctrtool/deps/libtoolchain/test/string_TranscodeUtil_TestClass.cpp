#include <tc/Exception.h>
#include <iostream>
#include <iomanip>

#include "string_TranscodeUtil_TestClass.h"

static const size_t kUtf8RawLen = 46;
static byte_t kUtf8Raw[kUtf8RawLen] = {0x2E, 0x2F, 0xD0, 0x80, 0xD0, 0x81, 0xD0, 0x82, 0xD0, 0x83, 0xD0, 0x84, 0xD0, 0x85, 0x2D, 0xD7, 0x9E, 0xD7, 0x91, 0xD7, 0x97, 0xD7, 0x9F, 0x2D, 0xD1, 0x82, 0xD0, 0xB5, 0xD1, 0x81, 0xD1, 0x82, 0x2D, 0xE3, 0x83, 0x86, 0xE3, 0x82, 0xB9, 0xE3, 0x83, 0x88, 0x2E, 0x62, 0x69, 0x6E};
static const size_t kUtf16RawLen = 26;
static uint16_t kUtf16Raw[kUtf16RawLen] = {0x2e, 0x2f, 0x400, 0x401, 0x402, 0x403, 0x404, 0x405, 0x2d, 0x5de, 0x5d1, 0x5d7, 0x5df, 0x2d, 0x442, 0x435, 0x441, 0x442, 0x2d, 0x30c6, 0x30b9, 0x30c8, 0x2e, 0x62, 0x69, 0x6e};

static std::string kUtf8Str = std::string((const char*)kUtf8Raw, kUtf8RawLen);
static std::u16string kUtf16Str = std::u16string((const char16_t*)kUtf16Raw, kUtf16RawLen);

void string_TranscodeUtil_TestClass::runAllTests(void)
{
	std::cout << "[tc::string::TranscodeUtil] START" << std::endl;
	testUtf8ToUtf16();
	testUtf16ToUtf8();
	std::cout << "[tc::string::TranscodeUtil] END" << std::endl;
}

void string_TranscodeUtil_TestClass::testUtf8ToUtf16()
{
	std::cout << "[tc::string::TranscodeUtil] testUtf8ToUtf16 : ";
	try
	{
		std::u16string utf16_str;

		tc::string::TranscodeUtil::UTF8ToUTF16(kUtf8Str, utf16_str);

		for (size_t i = 0; i < kUtf16RawLen; i++)
		{
			if (kUtf16Raw[i] != utf16_str.c_str()[i])
			{
				throw tc::Exception("unexpected UTF-16 char");
			}
		}
		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e)
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl;
	}
}

void string_TranscodeUtil_TestClass::testUtf16ToUtf8()
{
	std::cout << "[tc::string::TranscodeUtil] testUtf16ToUtf8 : ";
	try
	{
		std::string utf8_str;

		tc::string::TranscodeUtil::UTF16ToUTF8(kUtf16Str, utf8_str);

		for (size_t i = 0; i < kUtf8RawLen; i++)
		{
			if ((kUtf8Raw[i] & 0xff) != (utf8_str.c_str()[i] & 0xff))
			{
				throw tc::Exception("unexpected UTF-8 char");
			}
		}
		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e)
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl;
	}
}