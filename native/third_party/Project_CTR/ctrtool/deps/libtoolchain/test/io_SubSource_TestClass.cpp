#include <iostream>

#include "io_SubSource_TestClass.h"
#include "SourceTestUtil.h"

#include <tc.h>

void io_SubSource_TestClass::runAllTests(void)
{
	std::cout << "[tc::io::SubSource] START" << std::endl;
	testDefaultConstructor();
	testCreateConstructor();
	testNegativeOffset();
	testTooLargeOffset();
	std::cout << "[tc::io::SubSource] END" << std::endl;
}

void io_SubSource_TestClass::testDefaultConstructor()
{
	std::cout << "[tc::io::SubSource] testDefaultConstructor : " << std::flush;
	try
	{
		try
		{
			tc::io::SubSource source;

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

void io_SubSource_TestClass::testCreateConstructor()
{
	std::cout << "[tc::io::SubSource] testCreateConstructor : " << std::flush;
	try
	{
		try
		{
			// create source
			int64_t sub_offset = 0xc000;
			int64_t sub_length = 0x1000;
			tc::io::PaddingSource expected_subsource = tc::io::PaddingSource(0xff, 0x1000);
			
			tc::io::OverlayedSource base_source = tc::io::OverlayedSource(std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0, 0x10000)), std::make_shared<tc::io::PaddingSource>(expected_subsource), sub_offset, sub_length);
			tc::io::SubSource source = tc::io::SubSource(std::make_shared<tc::io::OverlayedSource>(base_source), sub_offset, sub_length);

			// create expected data
			tc::ByteData expected_data = expected_subsource.pullData(0, expected_subsource.length());

			// test source
			SourceTestUtil::testSourceLength(source, sub_length);
			SourceTestUtil::pullTestHelper(source, 0, sub_length, sub_length, expected_data.data());
			SourceTestUtil::pullTestHelper(source, 0, sub_length*2, sub_length, expected_data.data());

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

void io_SubSource_TestClass::testNegativeOffset()
{
	std::cout << "[tc::io::SubSource] testNegativeOffset : " << std::flush;
	try
	{
		try
		{
			// create source
			int64_t sub_offset = 0xc000;
			int64_t sub_length = 0x1000;
			tc::io::PaddingSource expected_subsource = tc::io::PaddingSource(0xff, 0x1000);
			
			tc::io::OverlayedSource base_source = tc::io::OverlayedSource(std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0, 0x10000)), std::make_shared<tc::io::PaddingSource>(expected_subsource), sub_offset, sub_length);
			tc::io::SubSource source = tc::io::SubSource(std::make_shared<tc::io::OverlayedSource>(base_source), sub_offset, sub_length);

			// test
			SourceTestUtil::testSourceLength(source, sub_length);
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

void io_SubSource_TestClass::testTooLargeOffset()
{
	std::cout << "[tc::io::SubSource] testTooLargeOffset : " << std::flush;
	try
	{
		try
		{
			// create source
			int64_t sub_offset = 0xc000;
			int64_t sub_length = 0x1000;
			tc::io::PaddingSource expected_subsource = tc::io::PaddingSource(0xff, 0x1000);
			
			tc::io::OverlayedSource base_source = tc::io::OverlayedSource(std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0, 0x10000)), std::make_shared<tc::io::PaddingSource>(expected_subsource), sub_offset, sub_length);
			tc::io::SubSource source = tc::io::SubSource(std::make_shared<tc::io::OverlayedSource>(base_source), sub_offset, sub_length);

			// test
			SourceTestUtil::testSourceLength(source, sub_length);
			SourceTestUtil::pullTestHelper(source, sub_length * 2, 20, 0, nullptr);
			
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