#include <iostream>

#include "io_StreamSource_TestClass.h"
#include "SourceTestUtil.h"
#include "StreamTestUtil.h"

#include <tc.h>

void io_StreamSource_TestClass::runAllTests(void)
{
	std::cout << "[tc::io::StreamSource] START" << std::endl;
	testDefaultConstructor();
	testCreateConstructor();
	testCreateFromStreamWithoutSeek();
	testCreateFromStreamWithoutRead();
	testCreateFromStreamWithoutWrite();
	testNegativeOffset();
	testTooLargeOffset();
	std::cout << "[tc::io::StreamSource] END" << std::endl;
}

void io_StreamSource_TestClass::testDefaultConstructor()
{
	std::cout << "[tc::io::StreamSource] testDefaultConstructor : " << std::flush;
	try
	{
		try
		{
			tc::io::StreamSource source;

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

void io_StreamSource_TestClass::testCreateConstructor()
{
	std::cout << "[tc::io::StreamSource] testCreateConstructor : " << std::flush;
	try
	{
		try
		{
			// create source
			size_t expected_len = 0x1000;
			tc::ByteData expected_data(expected_len);
			memset(expected_data.data(), 0x5A, expected_data.size());
			tc::io::MemoryStream base_stream = tc::io::MemoryStream(expected_data);	
			tc::io::StreamSource source = tc::io::StreamSource(std::make_shared<tc::io::MemoryStream>(base_stream));

			// test
			SourceTestUtil::testSourceLength(source, expected_len);
			SourceTestUtil::pullTestHelper(source, 0, expected_data.size(), expected_data.size(), expected_data.data());
			SourceTestUtil::pullTestHelper(source, 0, expected_data.size()*2, expected_data.size(), expected_data.data());

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

void io_StreamSource_TestClass::testCreateFromStreamWithoutSeek()
{
	std::cout << "[tc::io::StreamSource] testCreateFromStreamWithoutSeek : " << std::flush;
	try
	{
		try
		{
			// create source
			tc::io::StreamSource source = tc::io::StreamSource(std::shared_ptr<StreamTestUtil::DummyStreamBase>(new StreamTestUtil::DummyStreamBase(0x1000, true, true, false, true, true)));

			std::cout << "FAIL" << std::endl;	
		}
		catch (const tc::Exception& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_StreamSource_TestClass::testCreateFromStreamWithoutRead()
{
	std::cout << "[tc::io::StreamSource] testCreateFromStreamWithoutRead : " << std::flush;
	try
	{
		try
		{
			// create source
			tc::io::StreamSource source = tc::io::StreamSource(std::shared_ptr<StreamTestUtil::DummyStreamBase>(new StreamTestUtil::DummyStreamBase(0x1000, false, true, true, false, true)));

			std::cout << "FAIL" << std::endl;	
		}
		catch (const tc::Exception& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_StreamSource_TestClass::testCreateFromStreamWithoutWrite()
{
	std::cout << "[tc::io::StreamSource] testCreateFromStreamWithoutWrite : " << std::flush;
	try
	{
		try
		{
			// create source
			tc::io::StreamSource source = tc::io::StreamSource(std::shared_ptr<StreamTestUtil::DummyStreamBase>(new StreamTestUtil::DummyStreamBase(0x1000, true, false, true, false, true)));

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

void io_StreamSource_TestClass::testNegativeOffset()
{
	std::cout << "[tc::io::StreamSource] testNegativeOffset : " << std::flush;
	try
	{
		try
		{
			size_t expected_len = 0x1000;
			tc::ByteData expected_data(expected_len);
			memset(expected_data.data(), 0x5A, expected_data.size());
			tc::io::MemoryStream base_stream = tc::io::MemoryStream(expected_data);	
			tc::io::StreamSource source = tc::io::StreamSource(std::make_shared<tc::io::MemoryStream>(base_stream));

			// test
			SourceTestUtil::testSourceLength(source, expected_len);
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

void io_StreamSource_TestClass::testTooLargeOffset()
{
	std::cout << "[tc::io::StreamSource] testTooLargeOffset : " << std::flush;
	try
	{
		try
		{
			size_t expected_len = 0x1000;
			tc::ByteData expected_data(expected_len);
			memset(expected_data.data(), 0x5A, expected_data.size());
			tc::io::MemoryStream base_stream = tc::io::MemoryStream(expected_data);	
			tc::io::StreamSource source = tc::io::StreamSource(std::make_shared<tc::io::MemoryStream>(base_stream));

			// test
			SourceTestUtil::testSourceLength(source, expected_len);
			SourceTestUtil::pullTestHelper(source, expected_len * 2, 20, 0, nullptr);
			
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