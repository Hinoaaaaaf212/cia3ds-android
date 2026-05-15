#include <tc/Exception.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "bn_binaryutils_TestClass.h"

void bn_binaryutils_TestClass::runAllTests(void)
{
	std::cout << "[tc::bn (BinaryUtils)] START" << std::endl;
	test_RoundUpFunc();
	test_AlignFunc();
	test_MakeStructMagicU32Func();
	test_MakeStructMagicU64Func();
	std::cout << "[tc::bn (BinaryUtils)] END" << std::endl;
}

void bn_binaryutils_TestClass::test_RoundUpFunc()
{
	std::cout << "[tc::bn (BinaryUtils)] test_RoundUpFunc : " << std::flush;
	try
	{
		try 
		{
			util_RoundUpFuncTestCase(0, 0x200, 0x200);
			util_RoundUpFuncTestCase(1, 0x200, 0x200);
			util_RoundUpFuncTestCase(0x10, 0x200, 0x200);
			util_RoundUpFuncTestCase(0x1FF, 0x200, 0x200);
			util_RoundUpFuncTestCase(0x200, 0x200, 0x400);
			util_RoundUpFuncTestCase(0x201, 0x200, 0x400);
			util_RoundUpFuncTestCase(0x3FF, 0x200, 0x400);
			util_RoundUpFuncTestCase(0xDEADBE00, 0x200, 0xDEADC000);
			util_RoundUpFuncTestCase(0xDEADBEEF, 0x200, 0xDEADC000);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void bn_binaryutils_TestClass::util_RoundUpFuncTestCase(uint32_t value, uint32_t alignment, uint32_t expected_result)
{
	std::stringstream error_ss;
	uint32_t result = roundup<uint32_t>(value, alignment);
	if (result != expected_result)
	{
		error_ss << std::hex;
		error_ss << "roundup(0x" << value << ", 0x" << alignment << ") returned: 0x" << result << " (expected: 0x" << expected_result << ")" << std::endl;
		throw tc::Exception(error_ss.str());
	}
}

void bn_binaryutils_TestClass::test_AlignFunc()
{
	std::cout << "[tc::bn (BinaryUtils)] test_AlignFunc : " << std::flush;
	try
	{
		try 
		{
			util_AlignFuncTestCase(0, 0x200, 0x0);
			util_AlignFuncTestCase(1, 0x200, 0x200);
			util_AlignFuncTestCase(0x10, 0x200, 0x200);
			util_AlignFuncTestCase(0x1FF, 0x200, 0x200);
			util_AlignFuncTestCase(0x200, 0x200, 0x200);
			util_AlignFuncTestCase(0x201, 0x200, 0x400);
			util_AlignFuncTestCase(0x3FF, 0x200, 0x400);
			util_AlignFuncTestCase(0xDEADBE00, 0x200, 0xDEADBE00);
			util_AlignFuncTestCase(0xDEADBEEF, 0x200, 0xDEADC000);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void bn_binaryutils_TestClass::util_AlignFuncTestCase(uint32_t value, uint32_t alignment, uint32_t expected_result)
{
	std::stringstream error_ss;
	uint32_t result = align<uint32_t>(value, alignment);
	if (result != expected_result)
	{
		error_ss << std::hex;
		error_ss << "align(0x" << value << ", 0x" << alignment << ") returned: 0x" << result << " (expected: 0x" << expected_result << ")" << std::endl;
		throw tc::Exception(error_ss.str());
	}
}

void bn_binaryutils_TestClass::test_MakeStructMagicU32Func()
{
	std::cout << "[tc::bn (BinaryUtils)] test_MakeStructMagicU32Func : " << std::flush;
	try
	{
		try 
		{
			util_MakeStructMagicU32FuncTestCase("BABE", 0x45424142);
			util_MakeStructMagicU32FuncTestCase("NEXT", 0x5458454E);
			util_MakeStructMagicU32FuncTestCase("\x7F""ELF", 0x464C457F);
			util_MakeStructMagicU32FuncTestCase("BIN0", 0x304E4942);
		
			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void bn_binaryutils_TestClass::util_MakeStructMagicU32FuncTestCase(const char* struct_magic_str, uint32_t expected_result)
{
	std::stringstream error_ss;
	uint32_t result = tc::bn::make_struct_magic_uint32(struct_magic_str);
	if (result != expected_result)
	{
		error_ss << std::hex;
		error_ss << "make_struct_magic_uint32() returned: 0x" << result << " (expected: 0x" << expected_result << ")" << std::endl;
		throw tc::Exception(error_ss.str());
	}
}

void bn_binaryutils_TestClass::test_MakeStructMagicU64Func()
{
	std::cout << "[tc::bn (BinaryUtils)] test_MakeStructMagicU64Func : " << std::flush;
	try
	{
		try 
		{
			util_MakeStructMagicU64FuncTestCase("HOMEBREW", 0x57455242454D4F48);
			util_MakeStructMagicU64FuncTestCase("NEXTSPEC", 0x434550535458454E);
			util_MakeStructMagicU64FuncTestCase("EMPTY\0\0\0", 0x0000005954504D45);
		
			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void bn_binaryutils_TestClass::util_MakeStructMagicU64FuncTestCase(const char* struct_magic_str, uint64_t expected_result)
{
	std::stringstream error_ss;
	uint64_t result = tc::bn::make_struct_magic_uint64(struct_magic_str);
	if (result != expected_result)
	{
		error_ss << std::hex;
		error_ss << "make_struct_magic_uint32() returned: 0x" << result << " (expected: 0x" << expected_result << ")" << std::endl;
		throw tc::Exception(error_ss.str());
	}
}