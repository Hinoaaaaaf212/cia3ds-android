#include <iostream>
#include <sstream>

#include "io_MemoryStream_TestClass.h"
#include "StreamTestUtil.h"

#include <tc.h>

void io_MemoryStream_TestClass::runAllTests(void)
{
	std::cout << "[tc::io::MemoryStream] START" << std::endl;
	testCreateEmptyStream_DefaultConstructor();
	testCreateEmptyStream_SizedConstructor();
	testCreatePopulatedStream();
	testInitializeByCopyWithByteData();
	testInitializeByMoveWithByteData();
	testInitializeByCopyWithMemoryPointer();
	testSeekBeginToZero();
	testSeekBeginToMiddle();
	testSeekBeginToEnd();
	testSeekBeginPastEnd();
	testSeekBeginNegative();
	testSeekCurrentByZero();
	testSeekCurrentToMiddle();
	testSeekCurrentToEnd();
	testSeekCurrentPastEnd();
	testSeekCurrentNegative();
	testSeekEndByZero();
	testSeekEndPastEnd();
	testSeekEndNegative();
	testSeekEndTooNegative();
	testReadAllDataAvailable();
	testReadRequestsSubsetOfAvailableData();
	testReadSomeDataAvailable();
	testReadNoDataAvailable();
	testWriteAllDataWritable();
	testWriteToSomeOfDataAvailable();
	testWriteSomeDataWritable();
	testWriteNoDataWritable();
	testWriteReadDataPersistence();
	testResizeStreamLarger();
	testResizeStreamSmaller();
	testDispose();
	std::cout << "[tc::io::MemoryStream] END" << std::endl;
}

void io_MemoryStream_TestClass::testCreateEmptyStream_DefaultConstructor()
{
	std::cout << "[tc::io::MemoryStream] testCreateEmptyStream_DefaultConstructor : " << std::flush;
	try
	{
		try
		{
			tc::io::MemoryStream stream;

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, true, true);

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

void io_MemoryStream_TestClass::testCreateEmptyStream_SizedConstructor()
{
	std::cout << "[tc::io::MemoryStream] testCreateEmptyStream_SizedConstructor : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0;
			tc::io::MemoryStream stream(length);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, true, true);

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

void io_MemoryStream_TestClass::testCreatePopulatedStream()
{
	std::cout << "[tc::io::MemoryStream] testCreatePopulatedStream : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			tc::io::MemoryStream stream(length);

			StreamTestUtil::constructor_TestHelper(stream, 0xcafe, 0, true, true, true);

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

void io_MemoryStream_TestClass::testInitializeByCopyWithByteData()
{
	std::cout << "[tc::io::MemoryStream] testInitializeByCopyWithByteData : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;

			tc::ByteData data(length);
			memset(data.data(), 0xff, data.size());

			tc::io::MemoryStream stream = tc::io::MemoryStream(data);

			StreamTestUtil::constructor_TestHelper(stream, 0xcafe, 0, true, true, true);

			tc::ByteData output_data(stream.length());
			stream.read(output_data.data(), output_data.size());

			if (memcmp(output_data.data(), data.data(), length) != 0)
			{
				throw tc::Exception("Data in memory stream was not correct after being constructed from a ByteData object");
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

void io_MemoryStream_TestClass::testInitializeByMoveWithByteData()
{
	std::cout << "[tc::io::MemoryStream] testInitializeByMoveWithByteData : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;

			tc::ByteData control_data(length);
			memset(control_data.data(), 0xff, control_data.size());

			tc::ByteData experiment_data = control_data;

			tc::io::MemoryStream stream = tc::io::MemoryStream(std::move(experiment_data));

			if (experiment_data.size() != 0)
			{
				throw tc::Exception("experiment_data.size() != 0 after being moved from.");
			}
			if (experiment_data.data() != nullptr)
			{
				throw tc::Exception("experiment_data.data() != nullptr after being moved from.");
			}

			StreamTestUtil::constructor_TestHelper(stream, 0xcafe, 0, true, true, true);

			tc::ByteData output_data(stream.length());
			stream.read(output_data.data(), output_data.size());

			if (memcmp(output_data.data(), control_data.data(), length) != 0)
			{
				throw tc::Exception("Data in memory stream was not correct after being constructed from a ByteData object");
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

void io_MemoryStream_TestClass::testInitializeByCopyWithMemoryPointer()
{
	std::cout << "[tc::io::MemoryStream] testInitializeByCopyWithMemoryPointer : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			tc::ByteData data(length);
			tc::io::MemoryStream stream(data.data(), data.size());

			StreamTestUtil::constructor_TestHelper(stream, 0xcafe, 0, true, true, true);

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

void io_MemoryStream_TestClass::testSeekBeginToZero()
{
	std::cout << "[tc::io::MemoryStream] testSeekBeginToZero : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			tc::io::MemoryStream stream(length);

			StreamTestUtil::seek_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, 0, 0);

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

void io_MemoryStream_TestClass::testSeekBeginToMiddle()
{
	std::cout << "[tc::io::MemoryStream] testSeekBeginToMiddle : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			tc::io::MemoryStream stream(length);

			StreamTestUtil::seek_TestHelper(stream, 0xbabe, tc::io::SeekOrigin::Begin, 0xbabe, 0xbabe);

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

void io_MemoryStream_TestClass::testSeekBeginToEnd()
{
	std::cout << "[tc::io::MemoryStream] testSeekBeginToEnd : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			tc::io::MemoryStream stream(length);

			StreamTestUtil::seek_TestHelper(stream, length, tc::io::SeekOrigin::Begin, length, length);

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

void io_MemoryStream_TestClass::testSeekBeginPastEnd()
{
	std::cout << "[tc::io::MemoryStream] testSeekBeginPastEnd : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			tc::io::MemoryStream stream(length);

			StreamTestUtil::seek_TestHelper(stream, length+0x10, tc::io::SeekOrigin::Begin, length+0x10, length+0x10);

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

void io_MemoryStream_TestClass::testSeekBeginNegative()
{
	std::cout << "[tc::io::MemoryStream] testSeekBeginNegative : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			tc::io::MemoryStream stream(length);

			StreamTestUtil::seek_TestHelper(stream, -23, tc::io::SeekOrigin::Begin, 0, 0);

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

void io_MemoryStream_TestClass::testSeekCurrentByZero()
{
	std::cout << "[tc::io::MemoryStream] testSeekCurrentByZero : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			int64_t seek_pos = 0xbabe;
			tc::io::MemoryStream stream(length);

			stream.seek(seek_pos, tc::io::SeekOrigin::Begin);

			StreamTestUtil::seek_TestHelper(stream, 0, tc::io::SeekOrigin::Current, seek_pos, seek_pos);

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

void io_MemoryStream_TestClass::testSeekCurrentToMiddle()
{
	std::cout << "[tc::io::MemoryStream] testSeekCurrentToMiddle : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			int64_t seek_pos = 0xbabe;
			tc::io::MemoryStream stream(length);

			stream.seek(seek_pos, tc::io::SeekOrigin::Begin);

			StreamTestUtil::seek_TestHelper(stream, 0x20, tc::io::SeekOrigin::Current, seek_pos+0x20, seek_pos+0x20);

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

void io_MemoryStream_TestClass::testSeekCurrentToEnd()
{
	std::cout << "[tc::io::MemoryStream] testSeekCurrentToEnd : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			int64_t seek_pos = 0xbabe;
			tc::io::MemoryStream stream(length);

			stream.seek(seek_pos, tc::io::SeekOrigin::Begin);

			StreamTestUtil::seek_TestHelper(stream, length - seek_pos, tc::io::SeekOrigin::Current, length, length);

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

void io_MemoryStream_TestClass::testSeekCurrentPastEnd()
{
	std::cout << "[tc::io::MemoryStream] testSeekCurrentPastEnd : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			int64_t seek_pos = 0xbabe;
			tc::io::MemoryStream stream(length);

			stream.seek(seek_pos, tc::io::SeekOrigin::Begin);

			StreamTestUtil::seek_TestHelper(stream, length, tc::io::SeekOrigin::Current, length, length);

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

void io_MemoryStream_TestClass::testSeekCurrentNegative()
{
	std::cout << "[tc::io::MemoryStream] testSeekCurrentNegative : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			int64_t seek_pos = 0xbabe;
			tc::io::MemoryStream stream(length);

			stream.seek(seek_pos, tc::io::SeekOrigin::Begin);

			StreamTestUtil::seek_TestHelper(stream, 0 - seek_pos + 1, tc::io::SeekOrigin::Current, 1, 1);

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

void io_MemoryStream_TestClass::testSeekEndByZero()
{
	std::cout << "[tc::io::MemoryStream] testSeekEndByZero : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			int64_t seek_pos = 0;
			tc::io::MemoryStream stream(length);

			StreamTestUtil::seek_TestHelper(stream, seek_pos, tc::io::SeekOrigin::End, length, length);

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

void io_MemoryStream_TestClass::testSeekEndPastEnd()
{
	std::cout << "[tc::io::MemoryStream] testSeekEndPastEnd : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			int64_t seek_pos = 1;
			tc::io::MemoryStream stream(length);

			StreamTestUtil::seek_TestHelper(stream, seek_pos, tc::io::SeekOrigin::End, length + seek_pos, length + seek_pos);

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

void io_MemoryStream_TestClass::testSeekEndNegative()
{
	std::cout << "[tc::io::MemoryStream] testSeekEndNegative : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			tc::io::MemoryStream stream(length);

			StreamTestUtil::seek_TestHelper(stream, 0 - length, tc::io::SeekOrigin::End, 0, 0);

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

void io_MemoryStream_TestClass::testSeekEndTooNegative()
{
	std::cout << "[tc::io::MemoryStream] testSeekEndTooNegative : " << std::flush;
	try
	{
		try
		{
			int64_t length = 0xcafe;
			tc::io::MemoryStream stream(length);

			StreamTestUtil::seek_TestHelper(stream, 0 - length - length, tc::io::SeekOrigin::End, 0, 0);

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

void io_MemoryStream_TestClass::testReadAllDataAvailable()
{
	std::cout << "[tc::io::MemoryStream] testReadAllDataAvailable : " << std::flush;
	try
	{
		try
		{
			int64_t stream_length = 0xcafe;
			int64_t stream_offset = 0;
			size_t read_len = size_t(stream_length);

			tc::io::MemoryStream stream(stream_length);

			StreamTestUtil::read_TestHelper(stream, stream_offset, tc::io::SeekOrigin::Begin, read_len, read_len, read_len, stream_length);

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

void io_MemoryStream_TestClass::testReadRequestsSubsetOfAvailableData()
{
	std::cout << "[tc::io::MemoryStream] testReadRequestsSubsetOfAvailableData : " << std::flush;
	try
	{
		try
		{
			int64_t stream_length = 0xcafe;
			int64_t stream_offset = 0;
			size_t read_len = 0xbabe;

			tc::io::MemoryStream stream(stream_length);

			StreamTestUtil::read_TestHelper(stream, stream_offset, tc::io::SeekOrigin::Begin, read_len, read_len, read_len, int64_t(read_len));

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

void io_MemoryStream_TestClass::testReadSomeDataAvailable()
{
	std::cout << "[tc::io::MemoryStream] testReadSomeDataAvailable : " << std::flush;
	try
	{
		try
		{
			int64_t stream_length = 0xcafe;
			int64_t stream_offset = 0xbabe;
			size_t read_len = size_t(stream_length);

			tc::io::MemoryStream stream(stream_length);

			StreamTestUtil::read_TestHelper(stream, stream_offset, tc::io::SeekOrigin::Begin, read_len, read_len, read_len - size_t(stream_offset), stream_length);

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

void io_MemoryStream_TestClass::testReadNoDataAvailable()
{
	std::cout << "[tc::io::MemoryStream] testReadNoDataAvailable : " << std::flush;
	try
	{
		try
		{
			int64_t stream_length = 0xcafe;
			size_t read_len = size_t(stream_length);
			tc::io::MemoryStream stream(stream_length);

			StreamTestUtil::read_TestHelper(stream, 0, tc::io::SeekOrigin::End, read_len, read_len, 0, stream_length);
			StreamTestUtil::read_TestHelper(stream, 20, tc::io::SeekOrigin::End, read_len, read_len, 0, stream_length + 20);

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

void io_MemoryStream_TestClass::testWriteAllDataWritable()
{
	std::cout << "[tc::io::MemoryStream] testWriteAllDataWritable : " << std::flush;
	try
	{
		try
		{
			int64_t stream_length = 0xcafe;
			int64_t stream_offset = 0;
			size_t data_len = size_t(stream_length);

			tc::ByteData data = tc::ByteData(data_len);
			tc::io::MemoryStream stream(stream_length);

			int64_t stream_expected_position = int64_t(data_len) + stream_offset;

			StreamTestUtil::write_TestHelper(stream, stream_offset, tc::io::SeekOrigin::Begin, data, stream_expected_position);

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

void io_MemoryStream_TestClass::testWriteToSomeOfDataAvailable()
{
	std::cout << "[tc::io::MemoryStream] testWriteToSomeOfDataAvailable : " << std::flush;
	try
	{
		try
		{
			int64_t stream_length = 0xcafe;
			int64_t stream_offset = 0x10;
			size_t data_len = size_t(0xbabe);

			tc::ByteData data = tc::ByteData(data_len);
			tc::io::MemoryStream stream(stream_length);

			int64_t stream_expected_position = int64_t(data_len) + stream_offset;

			StreamTestUtil::write_TestHelper(stream, stream_offset, tc::io::SeekOrigin::Begin, data, stream_expected_position);

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

void io_MemoryStream_TestClass::testWriteSomeDataWritable()
{
	std::cout << "[tc::io::MemoryStream] testWriteSomeDataWritable : " << std::flush;
	try
	{
		try
		{
			int64_t stream_length = 0xcafe;
			int64_t stream_offset = 0;
			size_t data_len = size_t(0xdead);

			tc::ByteData data = tc::ByteData(data_len);
			tc::io::MemoryStream stream(stream_length);

			int64_t stream_expected_position = int64_t(data_len) + stream_offset;

			StreamTestUtil::write_TestHelper(stream, stream_offset, tc::io::SeekOrigin::Begin, data, stream_expected_position, stream_expected_position);

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

void io_MemoryStream_TestClass::testWriteNoDataWritable()
{
	std::cout << "[tc::io::MemoryStream] testWriteNoDataWritable : " << std::flush;
	try
	{
		try
		{
			int64_t stream_length = 0xcafe;
			int64_t stream_offset = stream_length;
			size_t data_len = size_t(0xcafe);

			tc::ByteData data = tc::ByteData(data_len);
			tc::io::MemoryStream stream(stream_length);

			int64_t stream_expected_position = int64_t(data_len) + stream_offset;

			StreamTestUtil::write_TestHelper(stream, stream_offset, tc::io::SeekOrigin::Begin, data, stream_expected_position, stream_expected_position);

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

void io_MemoryStream_TestClass::testWriteReadDataPersistence()
{
	std::cout << "[tc::io::MemoryStream] testWriteReadDataPersistence : " << std::flush;
	try
	{
		try
		{
			size_t data_size = 0x100;
			tc::ByteData source(data_size), dst(data_size);

			memset(source.data(), 0xab, source.size());

			tc::io::MemoryStream stream(data_size);

			stream.write(source.data(), source.size());
			stream.seek(0, tc::io::SeekOrigin::Begin);
			stream.read(dst.data(), dst.size());

			if (memcmp(source.data(), dst.data(), data_size) != 0)
			{
				throw tc::Exception("Stream did not read back data written to it");
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

void io_MemoryStream_TestClass::testResizeStreamLarger()
{
	std::cout << "[tc::io::MemoryStream] testResizeStreamLarger : " << std::flush;
	try
	{
		try
		{
			int64_t initial_len = 0xbabe;
			int64_t new_len = 0xcafe;

			tc::io::MemoryStream stream(initial_len);
			
			// write data to stream
			tc::ByteData initial_data = tc::ByteData(size_t(initial_len));
			memset(initial_data.data(), 0xdf, initial_data.size());
			stream.write(initial_data.data(), initial_data.size());

			// resize stream larger
			stream.setLength(new_len);

			// check stream length
			int64_t len_res = stream.length();

			if (len_res != new_len)
			{
				throw tc::Exception("Stream length was not correct after resizing stream");
			}

			// check stream position
			int64_t pos_res = stream.position();

			if (pos_res != initial_len)
			{
				throw tc::Exception("Stream position was not correct after resizing stream");
			}

			// read data from stream
			tc::ByteData new_data = tc::ByteData(size_t(stream.length()));
			stream.seek(0, tc::io::SeekOrigin::Begin);
			stream.read(new_data.data(), new_data.size());

			// check data was correct
			size_t cmp_size = std::min<size_t>(initial_data.size(), new_data.size());
			if (memcmp(initial_data.data(), new_data.data(), cmp_size) != 0)
			{
				throw tc::Exception("After resizing data was not correct");
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

void io_MemoryStream_TestClass::testResizeStreamSmaller()
{
	std::cout << "[tc::io::MemoryStream] testResizeStreamSmaller : " << std::flush;
	try
	{
		try
		{
			int64_t initial_len = 0xdead;
			int64_t new_len = 0xcafe;

			tc::io::MemoryStream stream(initial_len);
			
			// write data to stream
			tc::ByteData initial_data = tc::ByteData(size_t(initial_len));
			memset(initial_data.data(), 0xdf, initial_data.size());
			stream.write(initial_data.data(), initial_data.size());

			// resize stream larger
			stream.setLength(new_len);

			// check stream length
			int64_t len_res = stream.length();

			if (len_res != new_len)
			{
				throw tc::Exception("Stream length was not correct after resizing stream");
			}

			// check stream position
			int64_t pos_res = stream.position();

			if (pos_res != new_len)
			{
				throw tc::Exception("Stream position was not correct after resizing stream");
			}

			// read data from stream
			tc::ByteData new_data = tc::ByteData(size_t(stream.length()));
			stream.seek(0, tc::io::SeekOrigin::Begin);
			stream.read(new_data.data(), new_data.size());

			// check data was correct
			size_t cmp_size = std::min<size_t>(initial_data.size(), new_data.size());
			if (memcmp(initial_data.data(), new_data.data(), cmp_size) != 0)
			{
				throw tc::Exception("After resizing data was not correct");
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

void io_MemoryStream_TestClass::testDispose()
{
	std::cout << "[tc::io::MemoryStream] testDispose : " << std::flush;
	try
	{
		try
		{
			int64_t stream_length = 0xdead;
			tc::io::MemoryStream stream(stream_length);

			// test stream has a valid length pre-disposal
			if (stream.length() != stream_length)
			{
				throw tc::Exception("Stream did not have expected length pre-disposal");
			}

			// dispose stream
			stream.dispose();

			// test stream has a no length post-disposal
			if (stream.length() != 0)
			{
				throw tc::Exception("Stream did not have no length post-disposal");
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