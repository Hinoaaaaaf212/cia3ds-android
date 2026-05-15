#include <iostream>

#include "io_SubSink_TestClass.h"
#include "SinkTestUtil.h"

#include <tc.h>
#include <tc/io/IOUtil.h>
#include <sstream>
#include <iomanip>

void io_SubSink_TestClass::runAllTests(void)
{
	std::cout << "[tc::io::SubSink] START" << std::endl;
	testDefaultConstructor();
	testCreateConstructor();
	testCreateFromNullBase();
	testCreateWithNegativeSubSinkOffset();
	testCreateWithNegativeSubSinkLength();
	testCreateWithExcessiveSubSink();
	testCreateThenSetLength();
	testSetLengthOnDisposedBase();
	testPushDataOnDisposedBase();
	testPushDataOutsideOfBaseRange();
	std::cout << "[tc::io::SubSink] END" << std::endl;
}

void io_SubSink_TestClass::testDefaultConstructor()
{
	std::cout << "[tc::io::SubSink] testDefaultConstructor : " << std::flush;
	try
	{
		try
		{
			tc::io::SubSink sink;

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

void io_SubSink_TestClass::testCreateConstructor()
{
	std::cout << "[tc::io::SubSink] testCreateConstructor : " << std::flush;
	try
	{
		try
		{
			// create data to push
			auto data = tc::ByteData(0x100);

			// create base sink
			auto base_sink = std::shared_ptr<SinkTestUtil::DummySinkTestablePushData>(new SinkTestUtil::DummySinkTestablePushData());
			base_sink->setLength(0x10000);

			// create sub sink
			int64_t sub_sink_offset = 0xcafe;
			int64_t sub_sink_size = 0x1000;
			auto sub_sink = tc::io::SubSink(base_sink, sub_sink_offset, sub_sink_size);

			// test
			SinkTestUtil::testSinkLength(sub_sink, sub_sink_size);

			memset(data.data(), 0x33, data.size());
			pushDataTestHelper(sub_sink, base_sink, sub_sink_offset, 0, data, data);
			
			memset(data.data(), 0xea, data.size());
			pushDataTestHelper(sub_sink, base_sink, sub_sink_offset, 0x200, data, data);

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

void io_SubSink_TestClass::testCreateFromNullBase()
{
	std::cout << "[tc::io::SubSink] testCreateFromNullBase : " << std::flush;
	try
	{
		try
		{
			// create sink
			auto sub_sink = tc::io::SubSink(nullptr, 0, 0);

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

void io_SubSink_TestClass::testCreateWithNegativeSubSinkOffset()
{
	std::cout << "[tc::io::SubSink] testCreateWithNegativeSubSinkOffset : " << std::flush;
	try
	{
		try
		{
			// create data to push
			auto data = tc::ByteData(0x100);

			// create base sink
			auto base_sink = std::shared_ptr<SinkTestUtil::DummySinkTestablePushData>(new SinkTestUtil::DummySinkTestablePushData());
			base_sink->setLength(0x10000);

			// create sub sink
			int64_t sub_sink_offset = -1;
			int64_t sub_sink_size = 0x1000;
			auto sub_sink = tc::io::SubSink(base_sink, sub_sink_offset, sub_sink_size);

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

void io_SubSink_TestClass::testCreateWithNegativeSubSinkLength()
{
	std::cout << "[tc::io::SubSink] testCreateWithNegativeSubSinkLength : " << std::flush;
	try
	{
		try
		{
			// create data to push
			auto data = tc::ByteData(0x100);

			// create base sink
			auto base_sink = std::shared_ptr<SinkTestUtil::DummySinkTestablePushData>(new SinkTestUtil::DummySinkTestablePushData());
			base_sink->setLength(0x10000);

			// create sub sink
			int64_t sub_sink_offset = 0x200;
			int64_t sub_sink_size = -1;
			auto sub_sink = tc::io::SubSink(base_sink, sub_sink_offset, sub_sink_size);

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

void io_SubSink_TestClass::testCreateWithExcessiveSubSink()
{
	std::cout << "[tc::io::SubSink] testCreateWithExcessiveSubSink : " << std::flush;
	try
	{
		try
		{
			// create data to push
			auto data = tc::ByteData(0x100);

			// create base sink
			auto base_sink = std::shared_ptr<SinkTestUtil::DummySinkTestablePushData>(new SinkTestUtil::DummySinkTestablePushData());
			base_sink->setLength(0x10000);

			// create sub sink
			int64_t sub_sink_offset = base_sink->length() - 1;
			int64_t sub_sink_size = 2;
			auto sub_sink = tc::io::SubSink(base_sink, sub_sink_offset, sub_sink_size);

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

void io_SubSink_TestClass::testCreateThenSetLength()
{
	std::cout << "[tc::io::SubSink] testCreateThenSetLength : " << std::flush;
	try
	{
		try
		{
			// create data to push
			auto data = tc::ByteData(0x100);

			// create base sink
			auto base_sink = std::shared_ptr<SinkTestUtil::DummySinkTestablePushData>(new SinkTestUtil::DummySinkTestablePushData());
			base_sink->setLength(0x10000);

			// create sub sink
			int64_t sub_sink_offset = 0xcafe;
			int64_t sub_sink_size = 0x1000;
			auto sub_sink = tc::io::SubSink(base_sink, sub_sink_offset, sub_sink_size);

			// test
			int64_t new_length = 0xdeadcafe;
			sub_sink.setLength(new_length);

			SinkTestUtil::testSinkLength(sub_sink, new_length);
			

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

void io_SubSink_TestClass::testSetLengthOnDisposedBase()
{
	std::cout << "[tc::io::SubSink] testSetLengthOnDisposedBase : " << std::flush;
	try
	{
		try
		{
			// create sink
			tc::io::SubSink sink;

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

void io_SubSink_TestClass::testPushDataOnDisposedBase()
{
	std::cout << "[tc::io::SubSink] testPushDataOnDisposedBase : " << std::flush;
	try
	{
		try
		{
			// create sink
			tc::io::SubSink sink;

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

void io_SubSink_TestClass::testPushDataOutsideOfBaseRange()
{
	std::cout << "[tc::io::SubSink] testPushDataOutsideOfBaseRange : " << std::flush;
	try
	{
		try
		{
			// create base sink
			auto base_sink = std::shared_ptr<SinkTestUtil::DummySinkTestablePushData>(new SinkTestUtil::DummySinkTestablePushData());
			base_sink->setLength(0x10000);

			// create sub sink
			int64_t sub_sink_offset = 0xcafe;
			int64_t sub_sink_size = 0x1000;
			auto sub_sink = tc::io::SubSink(base_sink, sub_sink_offset, sub_sink_size);

			// test
			SinkTestUtil::testSinkLength(sub_sink, sub_sink_size);

			// create data to push
			auto push_data = tc::ByteData(0x100);
			memset(push_data.data(), 0x08, push_data.size());

			// create data to expect
			int64_t push_offset = sub_sink_size - 0x20;
			auto expected_data = tc::ByteData(push_data.data(), tc::io::IOUtil::getWritableCount(sub_sink_size, push_offset, push_data.size()));

			pushDataTestHelper(sub_sink, base_sink, sub_sink_offset, push_offset, push_data, expected_data);

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

void io_SubSink_TestClass::pushDataTestHelper(tc::io::ISink& sub_sink, const std::shared_ptr<SinkTestUtil::DummySinkTestablePushData>& base_sink, int64_t sub_base_offset, int64_t sub_push_offset, tc::ByteData& push_data, tc::ByteData& expected_data)
{
	base_sink->setExpectedPushDataCfg(expected_data, sub_base_offset + sub_push_offset);
	size_t push_ret = sub_sink.pushData(push_data, sub_push_offset);
	if (push_ret != expected_data.size())
	{
		std::stringstream error_ss;
		error_ss << "pushData(offset: " << sub_push_offset << ") returned: " << push_ret << ", when it should have been " << expected_data.size();;
		throw tc::Exception(error_ss.str());
	}
}