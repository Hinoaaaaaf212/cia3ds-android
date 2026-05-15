#include <iostream>
#include <sstream>

#include "io_MemorySource_TestClass.h"
#include "SourceTestUtil.h"

#include <tc.h>

void io_MemorySource_TestClass::runAllTests(void)
{
	std::cout << "[tc::io::MemorySource] START" << std::endl;
	testDefaultConstructor();
	testInitializeByCopyWithByteData();
	testInitializeByMoveWithByteData();
	testInitializeByCopyWithMemoryPointer();
	testNegativeOffset();
	testTooLargeOffset();
	std::cout << "[tc::io::MemorySource] END" << std::endl;
}

void io_MemorySource_TestClass::testDefaultConstructor()
{
	std::cout << "[tc::io::MemorySource] testDefaultConstructor : " << std::flush;
	try
	{
		try
		{
			tc::io::MemorySource source;

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

void io_MemorySource_TestClass::testInitializeByCopyWithByteData()
{
	std::cout << "[tc::io::MemorySource] testInitializeByCopyWithByteData : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			tc::ByteData data(length);
			memset(data.data(), 0xff, data.size());

			tc::io::MemorySource source = tc::io::MemorySource(data);

			SourceTestUtil::testSourceLength(source, length);
			SourceTestUtil::pullTestHelper(source, 0, data.size(), data.size(), data.data());
			SourceTestUtil::pullTestHelper(source, 0, data.size()*2, data.size(), data.data());

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

void io_MemorySource_TestClass::testInitializeByMoveWithByteData()
{
	std::cout << "[tc::io::MemorySource] testInitializeByMoveWithByteData : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			tc::ByteData control_data(length);
			memset(control_data.data(), 0xff, control_data.size());
			tc::ByteData experiment_data = control_data;

			tc::io::MemorySource source = tc::io::MemorySource(std::move(experiment_data));

			if (experiment_data.size() != 0)
			{
				throw tc::Exception("experiment_data.size() != 0 after being moved from.");
			}
			if (experiment_data.data() != nullptr)
			{
				throw tc::Exception("experiment_data.data() != nullptr after being moved from.");
			}

			SourceTestUtil::testSourceLength(source, length);
			SourceTestUtil::pullTestHelper(source, 0, control_data.size(), control_data.size(), control_data.data());
			SourceTestUtil::pullTestHelper(source, 0, control_data.size()*2, control_data.size(), control_data.data());

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

void io_MemorySource_TestClass::testInitializeByCopyWithMemoryPointer()
{
	std::cout << "[tc::io::MemorySource] testInitializeByCopyWithMemoryPointer : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			tc::ByteData data(length);
			memset(data.data(), 0xff, data.size());

			tc::io::MemorySource source = tc::io::MemorySource(data.data(), data.size());

			SourceTestUtil::testSourceLength(source, length);
			SourceTestUtil::pullTestHelper(source, 0, data.size(), data.size(), data.data());
			SourceTestUtil::pullTestHelper(source, 0, data.size()*2, data.size(), data.data());

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

void io_MemorySource_TestClass::testNegativeOffset()
{
	std::cout << "[tc::io::MemorySource] testNegativeOffset : " << std::flush;
	try
	{
		try
		{
			// create source
			size_t source_len = 0xbabe;
			tc::io::MemorySource source = tc::io::MemorySource(tc::ByteData(source_len));

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

void io_MemorySource_TestClass::testTooLargeOffset()
{
	std::cout << "[tc::io::MemorySource] testTooLargeOffset : " << std::flush;
	try
	{
		try
		{
			// create source
			size_t source_len = 0xbabe;
			tc::io::MemorySource source = tc::io::MemorySource(tc::ByteData(source_len));

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