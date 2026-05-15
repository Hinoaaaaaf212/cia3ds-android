#include <tc/Exception.h>
#include <iostream>

#include "bn_pad_TestClass.h"

void bn_pad_TestClass::runAllTests(void)
{
	std::cout << "[tc::bn::pad] START" << std::endl;
	test_CorrectSize();
	std::cout << "[tc::bn::pad] END" << std::endl;
}

void bn_pad_TestClass::test_CorrectSize()
{
	std::cout << "[tc::bn::pad] test_CorrectSize : " << std::flush;
	try
	{
		try 
		{
			tc::bn::pad<5> test_pad0;

			if (sizeof(test_pad0) != 5)
			{
				throw tc::Exception("tc::bn::pad<5> had incorrect sizeof()");
			}

			if (test_pad0.size() != 5)
			{
				throw tc::Exception("tc::bn::pad<5> had incorrect pad::size() result");
			}

			tc::bn::pad<0x200> test_pad1;

			if (sizeof(test_pad1) != 0x200)
			{
				throw tc::Exception("tc::bn::pad<0x200> had incorrect sizeof()");
			}

			if (test_pad1.size() != 0x200)
			{
				throw tc::Exception("tc::bn::pad<0x200> had incorrect pad::size() result");
			}
		
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