#include <iostream>

#include "io_PaddingSource_TestClass.h"
#include "SourceTestUtil.h"

#include <tc.h>

void io_PaddingSource_TestClass::runAllTests(void)
{
	std::cout << "[tc::io::PaddingSource] START" << std::endl;
	testDefaultConstructor();
	testCreateConstructor();
	testNegativeOffset();
	testTooLargeOffset();
	std::cout << "[tc::io::PaddingSource] END" << std::endl;
}

void io_PaddingSource_TestClass::testDefaultConstructor()
{
	std::cout << "[tc::io::PaddingSource] testDefaultConstructor : " << std::flush;
	try
	{
		try
		{
			tc::io::PaddingSource source;

			SourceTestUtil::testSourceLength(source, 0);
			SourceTestUtil::pullTestHelper(source, 0, 0xdead, 0, nullptr);

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

void io_PaddingSource_TestClass::testCreateConstructor()
{
	std::cout << "[tc::io::PaddingSource] testCreateConstructor : " << std::flush;
	try
	{
		try
		{
			// create source
			byte_t padding_byte = 0xef;
			int64_t source_len = 0x21432;
			tc::io::PaddingSource source(padding_byte, source_len);

			// create expected data
			tc::ByteData expected_data(source_len);
			memset(expected_data.data(), padding_byte, expected_data.size());

			// test source
			SourceTestUtil::testSourceLength(source, source_len);
			SourceTestUtil::pullTestHelper(source, 0, source_len, source_len, expected_data.data());
			SourceTestUtil::pullTestHelper(source, 0, source_len/2, source_len/2, expected_data.data());
			SourceTestUtil::pullTestHelper(source, 0, source_len*2, source_len, expected_data.data());

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

void io_PaddingSource_TestClass::testNegativeOffset()
{
	std::cout << "[tc::io::PaddingSource] testNegativeOffset : " << std::flush;
	try
	{
		try
		{
			// create source
			byte_t padding_byte = 0xef;
			int64_t source_len = 0x21432;
			tc::io::PaddingSource source(padding_byte, source_len);

			// test
			SourceTestUtil::testSourceLength(source, source_len);
			SourceTestUtil::pullTestHelper(source, -10, 20, 0, nullptr);

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

void io_PaddingSource_TestClass::testTooLargeOffset()
{
	std::cout << "[tc::io::PaddingSource] testTooLargeOffset : " << std::flush;
	try
	{
		try
		{
			// create source
			byte_t padding_byte = 0xef;
			int64_t source_len = 0x21432;
			tc::io::PaddingSource source(padding_byte, source_len);

			// test
			SourceTestUtil::testSourceLength(source, source_len);
			SourceTestUtil::pullTestHelper(source, source_len * 2, 20, 0, nullptr);
			
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