#include <iostream>
#include <sstream>

#include "io_StreamSink_TestClass.h"
#include "SinkTestUtil.h"
#include "StreamTestUtil.h"

#include <tc.h>

void io_StreamSink_TestClass::runAllTests(void)
{
	std::cout << "[tc::io::StreamSink] START" << std::endl;
	testDefaultConstructor();
	testCreateConstructor();
	testCreateFromNullStream();
	testCreateFromStreamWithoutSeek();
	testCreateFromStreamWithoutRead();
	testCreateFromStreamWithoutWrite();
	testSetLengthOnDisposedBase();
	testPushDataOnDisposedBase();
	testPushDataOutsideOfBaseRange();
	std::cout << "[tc::io::StreamSink] END" << std::endl;
}

void io_StreamSink_TestClass::testDefaultConstructor()
{
	std::cout << "[tc::io::StreamSink] testDefaultConstructor : " << std::flush;
	try
	{
		try
		{
			tc::io::StreamSink sink;

			SinkTestUtil::testSinkLength(sink, 0);

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

void io_StreamSink_TestClass::testCreateConstructor()
{
	std::cout << "[tc::io::StreamSink] testCreateConstructor : " << std::flush;
	try
	{
		try
		{
			// create sink
			size_t expected_data_len = 0x1000;
			tc::ByteData expected_data(expected_data_len);
			int64_t base_stream_len = 0x100000;
			auto base_stream = std::shared_ptr<tc::io::MemoryStream>(new tc::io::MemoryStream(base_stream_len));	
			tc::io::StreamSink sink = tc::io::StreamSink(base_stream);

			// test
			SinkTestUtil::testSinkLength(sink, base_stream->length());

			memset(expected_data.data(), 0x5A, expected_data.size());
			pushTestHelper(sink, base_stream, expected_data, 0);

			memset(expected_data.data(), 0x08, expected_data.size());
			pushTestHelper(sink, base_stream, expected_data, 0xcafe);

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

void io_StreamSink_TestClass::testCreateFromNullStream()
{
	std::cout << "[tc::io::StreamSink] testCreateFromNullStream : " << std::flush;
	try
	{
		try
		{
			// create sink
			tc::io::StreamSink sink = tc::io::StreamSink(nullptr);

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

void io_StreamSink_TestClass::testCreateFromStreamWithoutSeek()
{
	std::cout << "[tc::io::StreamSink] testCreateFromStreamWithoutSeek : " << std::flush;
	try
	{
		try
		{
			// create sink
			auto base_stream = StreamTestUtil::DummyStreamBase(0x1000, true, true, false, false, true);
			tc::io::StreamSink sink = tc::io::StreamSink(std::make_shared<StreamTestUtil::DummyStreamBase>(base_stream));

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

void io_StreamSink_TestClass::testCreateFromStreamWithoutRead()
{
	std::cout << "[tc::io::StreamSink] testCreateFromStreamWithoutRead : " << std::flush;
	try
	{
		try
		{
			// create sink
			auto base_stream = StreamTestUtil::DummyStreamBase(0x1000, false, true, true, true, true);
			tc::io::StreamSink sink = tc::io::StreamSink(std::make_shared<StreamTestUtil::DummyStreamBase>(base_stream));

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

void io_StreamSink_TestClass::testCreateFromStreamWithoutWrite()
{
	std::cout << "[tc::io::StreamSink] testCreateFromStreamWithoutWrite : " << std::flush;
	try
	{
		try
		{
			// create sink
			auto base_stream = StreamTestUtil::DummyStreamBase(0x1000, true, false, true, true, false);
			tc::io::StreamSink sink = tc::io::StreamSink(std::make_shared<StreamTestUtil::DummyStreamBase>(base_stream));

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

void io_StreamSink_TestClass::testSetLengthOnDisposedBase()
{
	std::cout << "[tc::io::StreamSink] testSetLengthOnDisposedBase : " << std::flush;
	try
	{
		try
		{
			// create sink
			tc::io::StreamSink sink;

			sink.setLength(0xdeadcafe);

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

void io_StreamSink_TestClass::testPushDataOnDisposedBase()
{
	std::cout << "[tc::io::StreamSink] testPushDataOnDisposedBase : " << std::flush;
	try
	{
		try
		{
			// create sink
			tc::io::StreamSink sink;

			sink.pushData(tc::ByteData(0xff), 0);

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

void io_StreamSink_TestClass::testPushDataOutsideOfBaseRange()
{
	std::cout << "[tc::io::StreamSink] testPushDataOutsideOfBaseRange : " << std::flush;
	try
	{
		try
		{
			// create sink
			size_t data_len = 0x1000;
			tc::ByteData data(data_len);
			int64_t base_stream_len = 0x100000;
			auto base_stream = std::shared_ptr<tc::io::MemoryStream>(new tc::io::MemoryStream(base_stream_len));	
			tc::io::StreamSink sink = tc::io::StreamSink(base_stream);

			// test
			SinkTestUtil::testSinkLength(sink, base_stream->length());

			memset(data.data(), 0x08, data.size());
			pushTestHelper(sink, base_stream, data, base_stream_len);

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

void io_StreamSink_TestClass::pushTestHelper(tc::io::ISink& sink, const std::shared_ptr<tc::io::IStream>& base_stream, tc::ByteData& expected_data, int64_t push_offset)
{
	std::stringstream error_ss;

	// push data
	size_t push_ret = sink.pushData(expected_data, push_offset);
	if (push_ret != expected_data.size())
	{
		error_ss << "pushData(offset: " << push_offset << ") returned: " << push_ret << ", when it should have been " << expected_data.size();
		throw tc::Exception(error_ss.str());
	}	

	// setup memory for reading result of push
	tc::ByteData output_data(expected_data.size());

	int64_t position_ret = base_stream->position();
	int64_t expected_position = push_offset + expected_data.size();
	if (position_ret != expected_position)
	{
		error_ss << "pushData(offset: " << push_offset << ") failed to write enough bytes, position(): " << position_ret << ", when it should have been " << expected_position;
		throw tc::Exception(error_ss.str());
	}	

	int64_t seek_ret = base_stream->seek(push_offset, tc::io::SeekOrigin::Begin);
	if (seek_ret != push_offset)
	{
		error_ss << "internal test method to adjust base_stream position failed. seek(offset:" << push_offset << ", origin: Begin): " << seek_ret << ", when it should have been " << push_offset;
		throw tc::Exception(error_ss.str());
	}
	
	size_t read_ret = base_stream->read(output_data.data(), output_data.size());
	if (read_ret != expected_data.size())
	{
		error_ss << "internal test method to read from base_stream failed. read(size: " << expected_data.size() << "): " << read_ret << ", when it should have been " << expected_data.size();
		throw tc::Exception(error_ss.str());
	}

	if (memcmp(expected_data.data(), output_data.data(), expected_data.size()) != 0)
	{
		error_ss << "pushData(offset: " << push_offset << ") did not write data to base_stream as expected";
		throw tc::Exception(error_ss.str());
	}


}