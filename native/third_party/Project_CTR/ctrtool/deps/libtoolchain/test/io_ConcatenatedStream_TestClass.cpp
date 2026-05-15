#include "io_ConcatenatedStream_TestClass.h"

#include <fmt/core.h>
#include "StreamTestUtil.h"

#include <tc.h>
#include <tc/io/IOUtil.h>

void io_ConcatenatedStream_TestClass::runAllTests(void)
{
	fmt::print("[tc::io::ConcatenatedStream] START\n");
	test_DefaultConstructor();
	test_CreateConstructor_ThrowsOnBadInput();
	test_CreateConstructor_SetsCorrectStreamState();
	test_setLength_ThrowsOnUse();
	test_read_ThrowsOnUnsupported();
	test_write_ThrowsOnUnsupported();
	test_seek_ThrowsOnUnsupported();
	test_seek_SeeksToBeginOnNegativeSeek();
	test_seek_SeeksToEndOnTooLargeSeek();
	test_seek_CanFindCorrectStreamForSeek();
	test_read_CanReadFromSingleStream();
	test_read_CanReadFromMultipleStreamWithSeekSupport();
	test_read_CanReadFromMultipleStreamWithNoSeekSupport();
	test_read_ReadFromMultiStream_NoSeekSupport_ThrowsOnSeekRequired();
	test_write_CanWriteFromSingleStream();
	test_write_CanWriteFromMultipleStreamWithSeekSupport();
	test_write_CanWriteFromMultipleStreamWithNoSeekSupport();
	test_write_WriteFromMultiStream_NoSeekSupport_ThrowsOnSeekRequired();
	test_MoveOperator_MoveDisposedToDisposed();
	test_MoveOperator_MoveInitializedToDisposed();
	test_MoveOperator_MoveDisposedToInitialized();
	test_MoveOperator_MoveInitializedToInitialized();
	test_MoveConstructor_MoveDisposed();
	test_MoveConstructor_MoveInitialized();
	fmt::print("[tc::io::ConcatenatedStream] END\n");
}

void io_ConcatenatedStream_TestClass::test_DefaultConstructor()
{
	fmt::print("[tc::io::ConcatenatedStream] test_DefaultConstructor : ");
	try
	{
		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream();

		// test state of stream
		StreamTestUtil::constructor_TestHelper(stream, 0, 0, false, false, false);

		try
		{
			stream.read(nullptr, 0);
			throw tc::Exception(".read() failed to throw tc::ObjectDisposedException when class was not initilaized.");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		try
		{
			stream.write(nullptr, 0);
			throw tc::Exception(".write() failed to throw tc::ObjectDisposedException when class was not initilaized.");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		try
		{
			stream.seek(0, tc::io::SeekOrigin::Begin);
			throw tc::Exception(".seek() failed to throw tc::ObjectDisposedException when class was not initilaized.");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		try
		{
			stream.setLength(0);
			throw tc::Exception(".setLength() failed to throw tc::ObjectDisposedException when class was not initilaized.");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		try
		{
			stream.flush();
			throw tc::Exception(".flush() failed to throw tc::ObjectDisposedException when class was not initilaized.");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_CreateConstructor_ThrowsOnBadInput()
{
	fmt::print("[tc::io::ConcatenatedStream] test_CreateConstructor_ThrowsOnBadInput : ");
	try
	{
		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {

			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);
			
			throw tc::Exception(".ctor() did not throw tc::NotSupportedException where there were no input streams");
		}
		catch (tc::NotSupportedException&)
		{
			// do nothing
		}

		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				nullptr,
				nullptr,
				nullptr
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);
			
			throw tc::Exception(".ctor() did not throw tc::NotSupportedException where there were null input streams");
		}
		catch (tc::NotSupportedException&)
		{
			// do nothing
		}

		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x0)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x0)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x0))
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);
			
			throw tc::Exception(".ctor() did not throw tc::NotSupportedException where there total length of input streams was 0.");
		}
		catch (tc::NotSupportedException&)
		{
			// do nothing
		}

		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, true, true, false, false)), // canRead=false, canWrite=true, canSeek=true, ...
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, false, true, false, false)), // canRead=true, canWrite=false, canSeek=true, ...
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)) // canRead=true, canWrite=true, canSeek=true, ...
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);
			
			throw tc::Exception(".ctor() did not throw tc::NotSupportedException where the input streams did not all support atleast either read or write.");
		}
		catch (tc::NotSupportedException&)
		{
			// do nothing
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_CreateConstructor_SetsCorrectStreamState()
{
	fmt::print("[tc::io::ConcatenatedStream] test_CreateConstructor_SetsCorrectStreamState : ");
	try
	{
		// test 1) (all input streams, length=0x100, canRead=true, canWrite=false, canSeek=false)
		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, false, false, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, false, false, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, false, false, false, false))
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

			StreamTestUtil::constructor_TestHelper(stream, 0x300, 0, true, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("Test 1 Failed: {}", e.error()));
		}

		// test 2) (all input streams, length=0x100, canRead=true, canWrite=mixed, canSeek=false)
		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, false, false, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, false, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, false, false, false, false))
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

			StreamTestUtil::constructor_TestHelper(stream, 0x300, 0, true, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("Test 2 Failed: {}", e.error()));
		}

		// test 3) (all input streams, length=0x100, canRead=true, canWrite=true, canSeek=false)
		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, false, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, false, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, false, false, false))
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

			StreamTestUtil::constructor_TestHelper(stream, 0x300, 0, true, true, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("Test 3 Failed: {}", e.error()));
		}

		// test 4) (all input streams, length=0x100, canRead=true, canWrite=true, canSeek=mixed)
		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, false, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, false, false, false))
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

			StreamTestUtil::constructor_TestHelper(stream, 0x300, 0, true, true, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("Test 4 Failed: {}", e.error()));
		}

		// test 5) (all input streams, length=0x100, canRead=true, canWrite=true, canSeek=true)
		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false))
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

			StreamTestUtil::constructor_TestHelper(stream, 0x300, 0, true, true, true);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("Test 5 Failed: {}", e.error()));
		}

		// test 6) (all input streams, length=0x100, canRead=mixed, canWrite=true, canSeek=true)
		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, true, true, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false))
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

			StreamTestUtil::constructor_TestHelper(stream, 0x300, 0, false, true, true);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("Test 6 Failed: {}", e.error()));
		}

		// test 7) (all input streams, length=0x100, canRead=false, canWrite=true, canSeek=true)
		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, true, true, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, true, true, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, true, true, false, false))
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

			StreamTestUtil::constructor_TestHelper(stream, 0x300, 0, false, true, true);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("Test 7 Failed: {}", e.error()));
		}

		// test 8) (all input streams, length=0x100, canRead=false, canWrite=true, canSeek=mixed)
		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, true, false, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, true, true, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, true, true, false, false))
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

			StreamTestUtil::constructor_TestHelper(stream, 0x300, 0, false, true, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("Test 8 Failed: {}", e.error()));
		}

		// test 9) (all input streams, length=0x100, canRead=false, canWrite=true, canSeek=false)
		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, true, false, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, true, false, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, true, false, false, false))
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

			StreamTestUtil::constructor_TestHelper(stream, 0x300, 0, false, true, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("Test 9 Failed: {}", e.error()));
		}

		// test 10) (all input streams, length=0x100, canRead=true, canWrite=true, canSeek=true) & one empty stream with only read
		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x000, true, false, false, false, false)), // this should be skipped because it has no size
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

			StreamTestUtil::constructor_TestHelper(stream, 0x300, 0, true, true, true);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("Test 10 Failed: {}", e.error()));
		}

		// test 11) (all input streams, length=0x100, canRead=true, canWrite=true, canSeek=true) & one populated stream with only no read/write
		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, false, false, false, false)), // this should be skipped because it cannot be read or written too
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

			StreamTestUtil::constructor_TestHelper(stream, 0x300, 0, true, true, true);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("Test 11 Failed: {}", e.error()));
		}

		// test 12) (all input streams, length=0x100, canRead=true, canWrite=true, canSeek=true) & one nullptr stream
		try
		{
			std::vector<std::shared_ptr<tc::io::IStream>> streams {
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
				nullptr, // this should be skipped because it is null
				std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			};

			tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

			StreamTestUtil::constructor_TestHelper(stream, 0x300, 0, true, true, true);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("Test 12 Failed: {}", e.error()));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_setLength_ThrowsOnUse()
{
	fmt::print("[tc::io::ConcatenatedStream] test_setLength_ThrowsOnUse : ");
	try
	{
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false))
		};

		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		try
		{
			stream.setLength(0);
			throw tc::Exception(".setLength() did not throw tc::NotImplementedException when called from an initalized class.");
		}
		catch (const tc::NotImplementedException&)
		{
			// do nothing
		}
		
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_read_ThrowsOnUnsupported()
{
	fmt::print("[tc::io::ConcatenatedStream] test_read_ThrowsOnUnsupported : ");
	try
	{
		
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, false, true, true, false, false))
		};

		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		try
		{
			stream.read(nullptr, 0);
			throw tc::Exception(".read() did not throw tc::NotSupportedException when canRead() == false.");
		}
		catch (const tc::NotSupportedException&)
		{
			// do nothing
		}
		
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_write_ThrowsOnUnsupported()
{
	fmt::print("[tc::io::ConcatenatedStream] test_write_ThrowsOnUnsupported : ");
	try
	{
		
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, false, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, false, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, false, true, false, false))
		};

		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		try
		{
			stream.write(nullptr, 0);
			throw tc::Exception(".write() did not throw tc::NotSupportedException when canWrite() == false.");
		}
		catch (const tc::NotSupportedException&)
		{
			// do nothing
		}
		
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_seek_ThrowsOnUnsupported()
{
	fmt::print("[tc::io::ConcatenatedStream] test_seek_ThrowsOnUnsupported : ");
	try
	{
		
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, false, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, false, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, false, false, false))
		};

		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		try
		{
			stream.seek(0, tc::io::SeekOrigin::Begin);
			throw tc::Exception(".seek() did not throw tc::NotSupportedException when canSeek() == false.");
		}
		catch (const tc::NotSupportedException&)
		{
			// do nothing
		}
		
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_seek_SeeksToBeginOnNegativeSeek()
{
	fmt::print("[tc::io::ConcatenatedStream] test_seek_SeeksToBeginOnNegativeSeek : ");
	try
	{
		class ValidateSeekParamDummyStream : public StreamTestUtil::DummyStreamBase
		{
		public:
			ValidateSeekParamDummyStream(int64_t seek_offset, tc::io::SeekOrigin seek_origin) :
				DummyStreamBase(0x100, true, true, true, false, false),
				mExpectedSeekOffset(seek_offset),
				mExpectedSeekOrigin(seek_origin)
			{}

			int64_t seek(int64_t offset, tc::io::SeekOrigin origin)
			{
				if (offset != mExpectedSeekOffset)
				{
					throw tc::Exception(fmt::format("offset passed to seek() was 0x{:x} (expected 0x{:x}", offset, mExpectedSeekOffset));
				}

				if (origin != mExpectedSeekOrigin)
				{
					std::string origin_str;
					switch (origin)
					{
					case tc::io::SeekOrigin::Begin:
						origin_str = "SeekOrigin::Begin";
						break;
					case tc::io::SeekOrigin::Current:
						origin_str = "SeekOrigin::Current";
						break;
					case tc::io::SeekOrigin::End:
						origin_str = "SeekOrigin::End";
						break;
					}

					std::string expected_origin_str;
					switch (mExpectedSeekOrigin)
					{
					case tc::io::SeekOrigin::Begin:
						expected_origin_str = "SeekOrigin::Begin";
						break;
					case tc::io::SeekOrigin::Current:
						expected_origin_str = "SeekOrigin::Current";
						break;
					case tc::io::SeekOrigin::End:
						expected_origin_str = "SeekOrigin::End";
						break;
					}

					throw tc::Exception(fmt::format("origin passed to seek() was {:s} (expected {:s}", origin_str, expected_origin_str));
				}

				return DummyStreamBase::seek(offset, origin);
			}
		private:
			int64_t mExpectedSeekOffset;
			tc::io::SeekOrigin mExpectedSeekOrigin;
		};

		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<ValidateSeekParamDummyStream>(ValidateSeekParamDummyStream(0, tc::io::SeekOrigin::Begin)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false))
		};

		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		struct Test {
			int64_t seek_pos, exp_seek_res;
		};

		std::vector<Test> tests {
			{0x0, 0x0},
			{-1, 0x0},
			{-2, 0x0},
			{-3, 0x0},
			{-10, 0x0},
			{-1000, 0x0},
			{-20000, 0x0},
		};

		for (auto test = tests.begin(); test != tests.end(); test++)
		{
			int64_t seek_res = stream.seek(test->seek_pos, tc::io::SeekOrigin::Begin);
			if (seek_res != test->exp_seek_res)
			{
				throw tc::Exception(fmt::format(".seek({}, tc::io::SeekOrigin::Begin) returned {} (expected {})", test->seek_pos, seek_res, test->exp_seek_res));
			}
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_seek_SeeksToEndOnTooLargeSeek()
{
	fmt::print("[tc::io::ConcatenatedStream] test_seek_SeeksToEndOnTooLargeSeek : ");
	try
	{
		class ValidateSeekParamDummyStream : public StreamTestUtil::DummyStreamBase
		{
		public:
			ValidateSeekParamDummyStream(int64_t seek_offset, tc::io::SeekOrigin seek_origin) :
				DummyStreamBase(0x100, true, true, true, false, false),
				mExpectedSeekOffset(seek_offset),
				mExpectedSeekOrigin(seek_origin)
			{}

			int64_t seek(int64_t offset, tc::io::SeekOrigin origin)
			{
				if (offset != mExpectedSeekOffset)
				{
					throw tc::Exception(fmt::format("offset passed to seek() was 0x{:x} (expected 0x{:x}", offset, mExpectedSeekOffset));
				}

				if (origin != mExpectedSeekOrigin)
				{
					std::string origin_str;
					switch (origin)
					{
					case tc::io::SeekOrigin::Begin:
						origin_str = "SeekOrigin::Begin";
						break;
					case tc::io::SeekOrigin::Current:
						origin_str = "SeekOrigin::Current";
						break;
					case tc::io::SeekOrigin::End:
						origin_str = "SeekOrigin::End";
						break;
					}

					std::string expected_origin_str;
					switch (mExpectedSeekOrigin)
					{
					case tc::io::SeekOrigin::Begin:
						expected_origin_str = "SeekOrigin::Begin";
						break;
					case tc::io::SeekOrigin::Current:
						expected_origin_str = "SeekOrigin::Current";
						break;
					case tc::io::SeekOrigin::End:
						expected_origin_str = "SeekOrigin::End";
						break;
					}

					throw tc::Exception(fmt::format("origin passed to seek() was {:s} (expected {:s}", origin_str, expected_origin_str));
				}

				return DummyStreamBase::seek(offset, origin);
			}
		private:
			int64_t mExpectedSeekOffset;
			tc::io::SeekOrigin mExpectedSeekOrigin;
		};

		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			std::make_shared<ValidateSeekParamDummyStream>(ValidateSeekParamDummyStream(0, tc::io::SeekOrigin::End))
		};

		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		struct Test {
			int64_t seek_pos, exp_seek_res;
		};

		std::vector<Test> tests {
			{0x300, 0x300},
			{0x301, 0x300},
			{0x302, 0x300},
			{0x310, 0x300},
			{0x400, 0x300},
			{0x1000, 0x300},
			{0x20000, 0x300},
		};

		for (auto test = tests.begin(); test != tests.end(); test++)
		{
			int64_t seek_res = stream.seek(test->seek_pos, tc::io::SeekOrigin::Begin);
			if (seek_res != test->exp_seek_res)
			{
				throw tc::Exception(fmt::format(".seek({}, tc::io::SeekOrigin::Begin) returned {} (expected {})", test->seek_pos, seek_res, test->exp_seek_res));
			}
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_seek_CanFindCorrectStreamForSeek()
{
	fmt::print("[tc::io::ConcatenatedStream] test_seek_CanFindCorrectStreamForSeek : ");
	try
	{
		struct SeekReport
		{
			int64_t seek_pos;
			size_t stream_id;
		};

		class ReportsSeekPosParamDummyStream : public StreamTestUtil::DummyStreamBase
		{
		public:
			ReportsSeekPosParamDummyStream(SeekReport& seek_resport, size_t stream_id) :
				DummyStreamBase(0x100, true, true, true, false, false),
				mSeekReport(seek_resport),
				mStreamId(stream_id)
			{}

			int64_t seek(int64_t offset, tc::io::SeekOrigin origin)
			{
				mSeekReport.seek_pos = DummyStreamBase::seek(offset, origin);
				mSeekReport.stream_id = mStreamId;
				
				return mSeekReport.seek_pos;
			}
		private:
			SeekReport& mSeekReport;
			size_t mStreamId;
		};

		SeekReport seek_report;

		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<ReportsSeekPosParamDummyStream>(ReportsSeekPosParamDummyStream(seek_report, 0)), // 0x000 - 0x0ff
			std::make_shared<ReportsSeekPosParamDummyStream>(ReportsSeekPosParamDummyStream(seek_report, 1)), // 0x100 - 0x1ff
			std::make_shared<ReportsSeekPosParamDummyStream>(ReportsSeekPosParamDummyStream(seek_report, 2)), // 0x200 - 0x2ff
			std::make_shared<ReportsSeekPosParamDummyStream>(ReportsSeekPosParamDummyStream(seek_report, 3)), // 0x300 - 0x3ff
			std::make_shared<ReportsSeekPosParamDummyStream>(ReportsSeekPosParamDummyStream(seek_report, 4)), // 0x400 - 0x4ff
			std::make_shared<ReportsSeekPosParamDummyStream>(ReportsSeekPosParamDummyStream(seek_report, 5)), // 0x500 - 0x5ff
			std::make_shared<ReportsSeekPosParamDummyStream>(ReportsSeekPosParamDummyStream(seek_report, 6)), // 0x600 - 0x6ff
			std::make_shared<ReportsSeekPosParamDummyStream>(ReportsSeekPosParamDummyStream(seek_report, 7)), // 0x700 - 0x7ff
		};

		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		struct Test {
			int64_t seek_pos;
			int64_t exp_seek_res;
			SeekReport exp_seek_report;
		};

		std::vector<Test> tests {
			{0x0, 0x0, {0x0, 0}},
			{0x20, 0x20, {0x20, 0}},
			{0xff, 0xff, {0xff, 0}},
			{0x100, 0x100, {0x0, 1}},
			{0x101, 0x101, {0x1, 1}},
			{0x1ff, 0x1ff, {0xff, 1}},
			{0x200, 0x200, {0x0, 2}},
			{0x600, 0x600, {0x0, 6}},
			{0x378, 0x378, {0x78, 3}},
			{0x7ff, 0x7ff, {0xff, 7}},
			{0x8, 0x8, {0x8, 0}},
			// prior of stream case
			{-1, 0x0, {0x0, 0}},
			{-120, 0x0, {0x0, 0}},
			// end of stream case
			{0x800, 0x800, {0x100, 7}},
			{0x1000, 0x800, {0x100, 7}},
		};

		for (auto test = tests.begin(); test != tests.end(); test++)
		{
			int64_t seek_res = stream.seek(test->seek_pos, tc::io::SeekOrigin::Begin);
			if (seek_res != test->exp_seek_res)
			{
				throw tc::Exception(fmt::format(".seek({}) returned {} (expected {})", test->seek_pos, seek_res, test->exp_seek_res));
			}

			if (seek_report.seek_pos != test->exp_seek_report.seek_pos || seek_report.stream_id != test->exp_seek_report.stream_id)
			{
				throw tc::Exception(fmt::format(".seek({}) triggered .seek({}) in stream {} (expected .seek({}) in stream {})", test->seek_pos, test->exp_seek_report.seek_pos, test->exp_seek_report.stream_id, seek_report.seek_pos, seek_report.stream_id));
			}
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_read_CanReadFromSingleStream()
{
	fmt::print("[tc::io::ConcatenatedStream] test_read_CanReadFromSingleStream : ");

	class ReportDummyStream : public StreamTestUtil::DummyStreamBase
	{
	public:
		struct SeekReport
		{
			int64_t seek_offset;
			size_t stream_id;

			bool operator==(const SeekReport& other) const
			{
				return seek_offset == other.seek_offset \
					&& stream_id == other.stream_id;
			}
		};

		struct ReadReport
		{
			const byte_t* read_ptr;
			size_t read_count;
			size_t stream_id;

			bool operator==(const ReadReport& other) const
			{
				return read_ptr == other.read_ptr \
					&& read_count == other.read_count \
					&& stream_id == other.stream_id;
			}
		};

		ReportDummyStream(std::vector<ReadReport>& read_report, std::vector<SeekReport>& seek_report, size_t stream_id) :
			DummyStreamBase(0x100, true, true, true, false, false),
			mReadReport(read_report),
			mSeekReport(seek_report),
			mStreamId(stream_id)
		{}

		size_t read(byte_t* ptr, size_t count)
		{
			mReadReport.push_back({ptr, count, mStreamId});

			size_t readable_count = tc::io::IOUtil::getReadableCount(DummyStreamBase::length(), DummyStreamBase::position(), count);

			// update stream position
			DummyStreamBase::seek(int64_t(readable_count), tc::io::SeekOrigin::Current);

			return readable_count;
		}

		int64_t seek(int64_t offset, tc::io::SeekOrigin origin)
		{	
			SeekReport seek_report = {DummyStreamBase::seek(offset, origin), mStreamId};

			mSeekReport.push_back(seek_report);

			return seek_report.seek_offset;
		}
	private:
		std::vector<ReadReport>& mReadReport;
		std::vector<SeekReport>& mSeekReport;
		size_t mStreamId;
	};

	try
	{
		// declare report logs
		std::vector<ReportDummyStream::ReadReport> read_report;
		std::vector<ReportDummyStream::SeekReport> seek_report;

		// create stream list with links to report logs
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 0)), // 0x000 - 0x0ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 1)), // 0x100 - 0x1ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 2)), // 0x200 - 0x2ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 3)), // 0x300 - 0x3ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 4)), // 0x400 - 0x4ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 5)), // 0x500 - 0x5ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 6)), // 0x600 - 0x6ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 7)), // 0x700 - 0x7ff
		};

		// create concatenated stream
		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		// define test
		struct Test {
			// test name
			std::string test_name;

			// these seeks are done then seek log is cleared
			std::vector<int64_t> unlogged_seeks;

			// these seeks are done before reading but will be logged
			std::vector<int64_t> logged_seeks;
			
			// read param
			byte_t* read_ptr;
			size_t read_count;

			// expected log reports
			std::vector<ReportDummyStream::ReadReport> exp_read_report;
			std::vector<ReportDummyStream::SeekReport> exp_seek_report;
		};

		// create tests
		std::vector<Test> tests {
			{"ReadFromBeginning", {0}, {}, (byte_t*)0x1000, 0x30, {{(byte_t*)0x1000, 0x30, 0}}, {}},
			{"ContinueReadingFromBeginning", {}, {}, (byte_t*)0x1000, 0x30, {{(byte_t*)0x1000, 0x30, 0}}, {}},
			{"ReadFromSomewhereElseInExistingStream", {0xe0}, {}, (byte_t*)0x1000, 0x20, {{(byte_t*)0x1000, 0x20, 0}}, {}},
		};

		// run tests
		for (auto test = tests.begin(); test != tests.end(); test++)
		{
			for (auto itr = test->unlogged_seeks.begin(); itr != test->unlogged_seeks.end(); itr++)
			{
				stream.seek(*itr, tc::io::SeekOrigin::Begin);
			}

			read_report.clear();
			seek_report.clear();

			for (auto itr = test->logged_seeks.begin(); itr != test->logged_seeks.end(); itr++)
			{
				stream.seek(*itr, tc::io::SeekOrigin::Begin);
			}

			stream.read(test->read_ptr, test->read_count);

			if (read_report != test->exp_read_report)
			{
				throw tc::Exception(fmt::format("Test \"{}\" Failed: .read() issued to base streams were not as expected", test->test_name));
			}

			if (seek_report != test->exp_seek_report)
			{
				throw tc::Exception(fmt::format("Test \"{}\" Failed: .seek() issued to base streams were not as expected", test->test_name));
			}
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_read_CanReadFromMultipleStreamWithSeekSupport()
{
	fmt::print("[tc::io::ConcatenatedStream] test_read_CanReadFromMultipleStreamWithSeekSupport : ");

	class ReportDummyStream : public StreamTestUtil::DummyStreamBase
	{
	public:
		struct SeekReport
		{
			int64_t seek_offset;
			size_t stream_id;

			bool operator==(const SeekReport& other) const
			{
				return seek_offset == other.seek_offset \
					&& stream_id == other.stream_id;
			}
		};

		struct ReadReport
		{
			const byte_t* read_ptr;
			size_t read_count;
			size_t stream_id;

			bool operator==(const ReadReport& other) const
			{
				return read_ptr == other.read_ptr \
					&& read_count == other.read_count \
					&& stream_id == other.stream_id;
			}
		};

		ReportDummyStream(std::vector<ReadReport>& read_report, std::vector<SeekReport>& seek_report, size_t stream_id) :
			DummyStreamBase(0x100, true, true, true, false, false),
			mReadReport(read_report),
			mSeekReport(seek_report),
			mStreamId(stream_id)
		{}

		size_t read(byte_t* ptr, size_t count)
		{
			mReadReport.push_back({ptr, count, mStreamId});

			size_t readable_count = tc::io::IOUtil::getReadableCount(DummyStreamBase::length(), DummyStreamBase::position(), count);

			// update stream position
			DummyStreamBase::seek(int64_t(readable_count), tc::io::SeekOrigin::Current);

			return readable_count;
		}

		int64_t seek(int64_t offset, tc::io::SeekOrigin origin)
		{	
			SeekReport seek_report = {DummyStreamBase::seek(offset, origin), mStreamId};

			mSeekReport.push_back(seek_report);

			return seek_report.seek_offset;
		}
	private:
		std::vector<ReadReport>& mReadReport;
		std::vector<SeekReport>& mSeekReport;
		size_t mStreamId;
	};

	try
	{
		// declare report logs
		std::vector<ReportDummyStream::ReadReport> read_report;
		std::vector<ReportDummyStream::SeekReport> seek_report;

		// create stream list with links to report logs
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 0)), // 0x000 - 0x0ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 1)), // 0x100 - 0x1ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 2)), // 0x200 - 0x2ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 3)), // 0x300 - 0x3ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 4)), // 0x400 - 0x4ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 5)), // 0x500 - 0x5ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 6)), // 0x600 - 0x6ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 7)), // 0x700 - 0x7ff
		};

		// create concatenated stream
		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		// define test
		struct Test {
			// test name
			std::string test_name;

			// these seeks are done then seek log is cleared
			std::vector<int64_t> unlogged_seeks;

			// these seeks are done before reading but will be logged
			std::vector<int64_t> logged_seeks;
			
			// read param
			byte_t* read_ptr;
			size_t read_count;

			// expected log reports
			std::vector<ReportDummyStream::ReadReport> exp_read_report;
			std::vector<ReportDummyStream::SeekReport> exp_seek_report;
		};

		// create tests
		std::vector<Test> tests {
			{"ReadAllOfStream 0-7 (stream 0x0 positions)", {0x000, 0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x000}, {}, (byte_t*)0x1000, 0x800, {{(byte_t*)0x1000, 0x100, 0}, {(byte_t*)0x1100, 0x100, 1}, {(byte_t*)0x1200, 0x100, 2}, {(byte_t*)0x1300, 0x100, 3}, {(byte_t*)0x1400, 0x100, 4}, {(byte_t*)0x1500, 0x100, 5}, {(byte_t*)0x1600, 0x100, 6}, {(byte_t*)0x1700, 0x100, 7}}, {}},
			{"ReadAllOfStream 0-7 (stream 0x80 positions)", {0x080, 0x180, 0x280, 0x380, 0x480, 0x580, 0x680, 0x780, 0x000}, {}, (byte_t*)0x1000, 0x800, {{(byte_t*)0x1000, 0x100, 0}, {(byte_t*)0x1100, 0x100, 1}, {(byte_t*)0x1200, 0x100, 2}, {(byte_t*)0x1300, 0x100, 3}, {(byte_t*)0x1400, 0x100, 4}, {(byte_t*)0x1500, 0x100, 5}, {(byte_t*)0x1600, 0x100, 6}, {(byte_t*)0x1700, 0x100, 7}}, {{0x0, 1}, {0x0, 2}, {0x0, 3}, {0x0, 4}, {0x0, 5}, {0x0, 6}, {0x0, 7}}},
			{"ReadAllOfStream 0-7 (stream 0x00 or 0x80 positions)", {0x080, 0x100, 0x280, 0x300, 0x480, 0x500, 0x680, 0x700, 0x000}, {}, (byte_t*)0x1000, 0x800, {{(byte_t*)0x1000, 0x100, 0}, {(byte_t*)0x1100, 0x100, 1}, {(byte_t*)0x1200, 0x100, 2}, {(byte_t*)0x1300, 0x100, 3}, {(byte_t*)0x1400, 0x100, 4}, {(byte_t*)0x1500, 0x100, 5}, {(byte_t*)0x1600, 0x100, 6}, {(byte_t*)0x1700, 0x100, 7}}, {{0x0, 2}, {0x0, 4}, {0x0, 6}}},
			{"ReadAllOfStream 3-6 (stream 0x0 positions)", {0x000, 0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x300}, {}, (byte_t*)0x1000, 0x400, {{(byte_t*)0x1000, 0x100, 3}, {(byte_t*)0x1100, 0x100, 4}, {(byte_t*)0x1200, 0x100, 5}, {(byte_t*)0x1300, 0x100, 6}}, {}},
			{"ReadAllOfStream 3-6 (stream 0x80 positions)", {0x080, 0x180, 0x280, 0x380, 0x480, 0x580, 0x680, 0x780, 0x300}, {}, (byte_t*)0x1000, 0x400, {{(byte_t*)0x1000, 0x100, 3}, {(byte_t*)0x1100, 0x100, 4}, {(byte_t*)0x1200, 0x100, 5}, {(byte_t*)0x1300, 0x100, 6}}, {{0x0, 4}, {0x0, 5}, {0x0, 6}}},
			{"ReadStream (partial)3,4-5,(partial)6 (stream 0x0 positions)", {0x000, 0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x350}, {}, (byte_t*)0x1000, 0x300, {{(byte_t*)0x1000, 0xB0, 3}, {(byte_t*)0x10B0, 0x100, 4}, {(byte_t*)0x11B0, 0x100, 5}, {(byte_t*)0x12B0, 0x50, 6}}, {}},
			{"ReadStream (partial)3,4-5,(partial)6 (stream 0x80 positions)", {0x080, 0x180, 0x280, 0x380, 0x480, 0x580, 0x680, 0x780, 0x350}, {}, (byte_t*)0x1000, 0x300, {{(byte_t*)0x1000, 0xB0, 3}, {(byte_t*)0x10B0, 0x100, 4}, {(byte_t*)0x11B0, 0x100, 5}, {(byte_t*)0x12B0, 0x50, 6}}, {{0x0, 4}, {0x0, 5}, {0x0, 6}}},
		};

		// run tests
		for (auto test = tests.begin(); test != tests.end(); test++)
		{
			for (auto itr = test->unlogged_seeks.begin(); itr != test->unlogged_seeks.end(); itr++)
			{
				stream.seek(*itr, tc::io::SeekOrigin::Begin);
			}

			read_report.clear();
			seek_report.clear();

			for (auto itr = test->logged_seeks.begin(); itr != test->logged_seeks.end(); itr++)
			{
				stream.seek(*itr, tc::io::SeekOrigin::Begin);
			}

			stream.read(test->read_ptr, test->read_count);

			if (read_report != test->exp_read_report)
			{
				throw tc::Exception(fmt::format("Test \"{}\" Failed: .read() issued to base streams were not as expected", test->test_name));
			}

			if (seek_report != test->exp_seek_report)
			{
				throw tc::Exception(fmt::format("Test \"{}\" Failed: .seek() issued to base streams were not as expected", test->test_name));
			}
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_read_CanReadFromMultipleStreamWithNoSeekSupport()
{
	fmt::print("[tc::io::ConcatenatedStream] test_read_CanReadFromMultipleStreamWithNoSeekSupport : ");

	class ReportDummyStream : public StreamTestUtil::DummyStreamBase
	{
	public:
		struct SeekReport
		{
			int64_t seek_offset;
			size_t stream_id;

			bool operator==(const SeekReport& other) const
			{
				return seek_offset == other.seek_offset \
					&& stream_id == other.stream_id;
			}
		};

		struct ReadReport
		{
			const byte_t* read_ptr;
			size_t read_count;
			size_t stream_id;

			bool operator==(const ReadReport& other) const
			{
				return read_ptr == other.read_ptr \
					&& read_count == other.read_count \
					&& stream_id == other.stream_id;
			}
		};

		ReportDummyStream(std::vector<ReadReport>& read_report, std::vector<SeekReport>& seek_report, size_t stream_id) :
			DummyStreamBase(0x100, true, true, false, false, false),
			mReadReport(read_report),
			mSeekReport(seek_report),
			mStreamId(stream_id)
		{}

		size_t read(byte_t* ptr, size_t count)
		{
			mReadReport.push_back({ptr, count, mStreamId});

			size_t readable_count = tc::io::IOUtil::getReadableCount(DummyStreamBase::length(), DummyStreamBase::position(), count);

			// update stream position
			DummyStreamBase::seek(int64_t(readable_count), tc::io::SeekOrigin::Current);

			return readable_count;
		}

		int64_t seek(int64_t offset, tc::io::SeekOrigin origin)
		{	
			SeekReport seek_report = {DummyStreamBase::seek(offset, origin), mStreamId};

			mSeekReport.push_back(seek_report);

			return seek_report.seek_offset;
		}
	private:
		std::vector<ReadReport>& mReadReport;
		std::vector<SeekReport>& mSeekReport;
		size_t mStreamId;
	};

	try
	{
		// declare report logs
		std::vector<ReportDummyStream::ReadReport> read_report;
		std::vector<ReportDummyStream::SeekReport> seek_report;

		// create stream list with links to report logs
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 0)), // 0x000 - 0x0ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 1)), // 0x100 - 0x1ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 2)), // 0x200 - 0x2ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 3)), // 0x300 - 0x3ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 4)), // 0x400 - 0x4ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 5)), // 0x500 - 0x5ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 6)), // 0x600 - 0x6ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 7)), // 0x700 - 0x7ff
		};

		// create concatenated stream
		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		// define test
		struct Test {
			// test name
			std::string test_name;

			// these seeks are done then seek log is cleared
			std::vector<int64_t> unlogged_seeks;

			// these seeks are done before reading but will be logged
			std::vector<int64_t> logged_seeks;
			
			// read param
			byte_t* read_ptr;
			size_t read_count;

			// expected log reports
			std::vector<ReportDummyStream::ReadReport> exp_read_report;
			std::vector<ReportDummyStream::SeekReport> exp_seek_report;
		};

		// create tests
		std::vector<Test> tests {
			{"ReadStream (partial)0", {}, {}, (byte_t*)0x1000, 0xA0, {{(byte_t*)0x1000, 0xA0, 0}}, {}},
			{"ReadStream (partial)0,1-4", {}, {}, (byte_t*)0x1000, 0x460, {{(byte_t*)0x1000, 0x60, 0}, {(byte_t*)0x1060, 0x100, 1}, {(byte_t*)0x1160, 0x100, 2}, {(byte_t*)0x1260, 0x100, 3}, {(byte_t*)0x1360, 0x100, 4}}, {}},
			{"ReadStream 5-7", {}, {}, (byte_t*)0x1000, 0x300, {{(byte_t*)0x1000, 0x100, 5}, {(byte_t*)0x1100, 0x100, 6}, {(byte_t*)0x1200, 0x100, 7}}, {}},
		};

		// run tests
		for (auto test = tests.begin(); test != tests.end(); test++)
		{
			for (auto itr = test->unlogged_seeks.begin(); itr != test->unlogged_seeks.end(); itr++)
			{
				stream.seek(*itr, tc::io::SeekOrigin::Begin);
			}

			read_report.clear();
			seek_report.clear();

			for (auto itr = test->logged_seeks.begin(); itr != test->logged_seeks.end(); itr++)
			{
				stream.seek(*itr, tc::io::SeekOrigin::Begin);
			}

			stream.read(test->read_ptr, test->read_count);

			if (read_report != test->exp_read_report)
			{
				fmt::print("\n");
				fmt::print("ReadLog:\n");
				for (auto itr = read_report.begin(); itr != read_report.end(); itr++)
				{
					fmt::print("ReadReport (ptr: 0x{:x}, count: 0x{:x}, stream: {:d})\n", (size_t)itr->read_ptr, itr->read_count, itr->stream_id);
				}
				fmt::print("ExpReadLog:\n");
				for (auto itr = test->exp_read_report.begin(); itr != test->exp_read_report.end(); itr++)
				{
					fmt::print("ReadReport (ptr: 0x{:x}, count: 0x{:x}, stream: {:d})\n", (size_t)itr->read_ptr, itr->read_count, itr->stream_id);
				}
				throw tc::Exception(fmt::format("Test \"{}\" Failed: .read() issued to base streams were not as expected", test->test_name));
			}

			if (seek_report != test->exp_seek_report)
			{
				throw tc::Exception(fmt::format("Test \"{}\" Failed: .seek() issued to base streams were not as expected", test->test_name));
			}
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_read_ReadFromMultiStream_NoSeekSupport_ThrowsOnSeekRequired()
{
	fmt::print("[tc::io::ConcatenatedStream] test_read_ReadFromMultiStream_NoSeekSupport_ThrowsOnSeekRequired : ");

	class ReportDummyStream : public StreamTestUtil::DummyStreamBase
	{
	public:
		struct SeekReport
		{
			int64_t seek_offset;
			size_t stream_id;

			bool operator==(const SeekReport& other) const
			{
				return seek_offset == other.seek_offset \
					&& stream_id == other.stream_id;
			}
		};

		struct ReadReport
		{
			const byte_t* read_ptr;
			size_t read_count;
			size_t stream_id;

			bool operator==(const ReadReport& other) const
			{
				return read_ptr == other.read_ptr \
					&& read_count == other.read_count \
					&& stream_id == other.stream_id;
			}
		};

		ReportDummyStream(std::vector<ReadReport>& read_report, std::vector<SeekReport>& seek_report, size_t stream_id) :
			DummyStreamBase(0x100, 0x80, true, true, false, false, false), // the initial position is 0x80
			mReadReport(read_report),
			mSeekReport(seek_report),
			mStreamId(stream_id)
		{}

		size_t read(byte_t* ptr, size_t count)
		{
			mReadReport.push_back({ptr, count, mStreamId});

			size_t readable_count = tc::io::IOUtil::getReadableCount(DummyStreamBase::length(), DummyStreamBase::position(), count);

			// update stream position
			DummyStreamBase::seek(int64_t(readable_count), tc::io::SeekOrigin::Current);

			return readable_count;
		}

		int64_t seek(int64_t offset, tc::io::SeekOrigin origin)
		{	
			SeekReport seek_report = {DummyStreamBase::seek(offset, origin), mStreamId};

			mSeekReport.push_back(seek_report);

			return seek_report.seek_offset;
		}
	private:
		std::vector<ReadReport>& mReadReport;
		std::vector<SeekReport>& mSeekReport;
		size_t mStreamId;
	};

	try
	{
		// declare report logs
		std::vector<ReportDummyStream::ReadReport> read_report;
		std::vector<ReportDummyStream::SeekReport> seek_report;

		// create stream list with links to report logs
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 0)), // 0x000 - 0x0ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 1)), // 0x100 - 0x1ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 2)), // 0x200 - 0x2ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 3)), // 0x300 - 0x3ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 4)), // 0x400 - 0x4ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 5)), // 0x500 - 0x5ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 6)), // 0x600 - 0x6ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(read_report, seek_report, 7)), // 0x700 - 0x7ff
		};

		// create concatenated stream
		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		try
		{
			stream.read((byte_t*)0x1000, 0x200);
			throw tc::Exception(".read() did not throw tc::io::IOException where stream required seeking to begining but did not support seeking.");
		}
		catch (const tc::io::IOException&)
		{
			// do nothing
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_write_CanWriteFromSingleStream()
{
	fmt::print("[tc::io::ConcatenatedStream] test_write_CanWriteFromSingleStream : ");

	class ReportDummyStream : public StreamTestUtil::DummyStreamBase
	{
	public:
		struct SeekReport
		{
			int64_t seek_offset;
			size_t stream_id;

			bool operator==(const SeekReport& other) const
			{
				return seek_offset == other.seek_offset \
					&& stream_id == other.stream_id;
			}
		};

		struct WriteReport
		{
			const byte_t* write_ptr;
			size_t write_count;
			size_t stream_id;

			bool operator==(const WriteReport& other) const
			{
				return write_ptr == other.write_ptr \
					&& write_count == other.write_count \
					&& stream_id == other.stream_id;
			}
		};

		ReportDummyStream(std::vector<WriteReport>& write_report, std::vector<SeekReport>& seek_report, size_t stream_id) :
			DummyStreamBase(0x100, true, true, true, false, false),
			mWriteReport(write_report),
			mSeekReport(seek_report),
			mStreamId(stream_id)
		{}

		size_t write(const byte_t* ptr, size_t count)
		{
			mWriteReport.push_back({ptr, count, mStreamId});

			size_t writable_count = tc::io::IOUtil::getWritableCount(DummyStreamBase::length(), DummyStreamBase::position(), count);

			// update stream position
			DummyStreamBase::seek(int64_t(writable_count), tc::io::SeekOrigin::Current);

			return writable_count;
		}

		int64_t seek(int64_t offset, tc::io::SeekOrigin origin)
		{	
			SeekReport seek_report = {DummyStreamBase::seek(offset, origin), mStreamId};

			mSeekReport.push_back(seek_report);

			return seek_report.seek_offset;
		}
	private:
		std::vector<WriteReport>& mWriteReport;
		std::vector<SeekReport>& mSeekReport;
		size_t mStreamId;
	};

	try
	{
		// declare report logs
		std::vector<ReportDummyStream::WriteReport> write_report;
		std::vector<ReportDummyStream::SeekReport> seek_report;

		// create stream list with links to report logs
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 0)), // 0x000 - 0x0ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 1)), // 0x100 - 0x1ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 2)), // 0x200 - 0x2ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 3)), // 0x300 - 0x3ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 4)), // 0x400 - 0x4ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 5)), // 0x500 - 0x5ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 6)), // 0x600 - 0x6ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 7)), // 0x700 - 0x7ff
		};

		// create concatenated stream
		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		// define test
		struct Test {
			// test name
			std::string test_name;

			// these seeks are done then seek log is cleared
			std::vector<int64_t> unlogged_seeks;

			// these seeks are done before writing but will be logged
			std::vector<int64_t> logged_seeks;
			
			// write param
			byte_t* write_ptr;
			size_t write_count;

			// expected log reports
			std::vector<ReportDummyStream::WriteReport> exp_write_report;
			std::vector<ReportDummyStream::SeekReport> exp_seek_report;
		};

		// create tests
		std::vector<Test> tests {
			{"WriteFromBeginning", {0}, {}, (byte_t*)0x1000, 0x30, {{(byte_t*)0x1000, 0x30, 0}}, {}},
			{"ContinueWritingFromBeginning", {}, {}, (byte_t*)0x1000, 0x30, {{(byte_t*)0x1000, 0x30, 0}}, {}},
			{"WriteFromSomewhereElseInExistingStream", {0xe0}, {}, (byte_t*)0x1000, 0x20, {{(byte_t*)0x1000, 0x20, 0}}, {}},
		};

		// run tests
		for (auto test = tests.begin(); test != tests.end(); test++)
		{
			for (auto itr = test->unlogged_seeks.begin(); itr != test->unlogged_seeks.end(); itr++)
			{
				stream.seek(*itr, tc::io::SeekOrigin::Begin);
			}

			write_report.clear();
			seek_report.clear();

			for (auto itr = test->logged_seeks.begin(); itr != test->logged_seeks.end(); itr++)
			{
				stream.seek(*itr, tc::io::SeekOrigin::Begin);
			}

			stream.write(test->write_ptr, test->write_count);

			if (write_report != test->exp_write_report)
			{
				throw tc::Exception(fmt::format("Test \"{}\" Failed: .write() issued to base streams were not as expected", test->test_name));
			}

			if (seek_report != test->exp_seek_report)
			{
				throw tc::Exception(fmt::format("Test \"{}\" Failed: .seek() issued to base streams were not as expected", test->test_name));
			}
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_write_CanWriteFromMultipleStreamWithSeekSupport()
{
	fmt::print("[tc::io::ConcatenatedStream] test_write_CanWriteFromMultipleStreamWithSeekSupport : ");

	class ReportDummyStream : public StreamTestUtil::DummyStreamBase
	{
	public:
		struct SeekReport
		{
			int64_t seek_offset;
			size_t stream_id;

			bool operator==(const SeekReport& other) const
			{
				return seek_offset == other.seek_offset \
					&& stream_id == other.stream_id;
			}
		};

		struct WriteReport
		{
			const byte_t* write_ptr;
			size_t write_count;
			size_t stream_id;

			bool operator==(const WriteReport& other) const
			{
				return write_ptr == other.write_ptr \
					&& write_count == other.write_count \
					&& stream_id == other.stream_id;
			}
		};

		ReportDummyStream(std::vector<WriteReport>& write_report, std::vector<SeekReport>& seek_report, size_t stream_id) :
			DummyStreamBase(0x100, true, true, true, false, false),
			mWriteReport(write_report),
			mSeekReport(seek_report),
			mStreamId(stream_id)
		{}

		size_t write(const byte_t* ptr, size_t count)
		{
			mWriteReport.push_back({ptr, count, mStreamId});

			size_t writable_count = tc::io::IOUtil::getWritableCount(DummyStreamBase::length(), DummyStreamBase::position(), count);

			// update stream position
			DummyStreamBase::seek(int64_t(writable_count), tc::io::SeekOrigin::Current);

			return writable_count;
		}

		int64_t seek(int64_t offset, tc::io::SeekOrigin origin)
		{	
			SeekReport seek_report = {DummyStreamBase::seek(offset, origin), mStreamId};

			mSeekReport.push_back(seek_report);

			return seek_report.seek_offset;
		}
	private:
		std::vector<WriteReport>& mWriteReport;
		std::vector<SeekReport>& mSeekReport;
		size_t mStreamId;
	};

	try
	{
		// declare report logs
		std::vector<ReportDummyStream::WriteReport> write_report;
		std::vector<ReportDummyStream::SeekReport> seek_report;

		// create stream list with links to report logs
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 0)), // 0x000 - 0x0ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 1)), // 0x100 - 0x1ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 2)), // 0x200 - 0x2ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 3)), // 0x300 - 0x3ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 4)), // 0x400 - 0x4ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 5)), // 0x500 - 0x5ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 6)), // 0x600 - 0x6ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 7)), // 0x700 - 0x7ff
		};

		// create concatenated stream
		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		// define test
		struct Test {
			// test name
			std::string test_name;

			// these seeks are done then seek log is cleared
			std::vector<int64_t> unlogged_seeks;

			// these seeks are done before writeing but will be logged
			std::vector<int64_t> logged_seeks;
			
			// write param
			byte_t* write_ptr;
			size_t write_count;

			// expected log reports
			std::vector<ReportDummyStream::WriteReport> exp_write_report;
			std::vector<ReportDummyStream::SeekReport> exp_seek_report;
		};

		// create tests
		std::vector<Test> tests {
			{"WriteAllOfStream 0-7 (stream 0x0 positions)", {0x000, 0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x000}, {}, (byte_t*)0x1000, 0x800, {{(byte_t*)0x1000, 0x100, 0}, {(byte_t*)0x1100, 0x100, 1}, {(byte_t*)0x1200, 0x100, 2}, {(byte_t*)0x1300, 0x100, 3}, {(byte_t*)0x1400, 0x100, 4}, {(byte_t*)0x1500, 0x100, 5}, {(byte_t*)0x1600, 0x100, 6}, {(byte_t*)0x1700, 0x100, 7}}, {}},
			{"WriteAllOfStream 0-7 (stream 0x80 positions)", {0x080, 0x180, 0x280, 0x380, 0x480, 0x580, 0x680, 0x780, 0x000}, {}, (byte_t*)0x1000, 0x800, {{(byte_t*)0x1000, 0x100, 0}, {(byte_t*)0x1100, 0x100, 1}, {(byte_t*)0x1200, 0x100, 2}, {(byte_t*)0x1300, 0x100, 3}, {(byte_t*)0x1400, 0x100, 4}, {(byte_t*)0x1500, 0x100, 5}, {(byte_t*)0x1600, 0x100, 6}, {(byte_t*)0x1700, 0x100, 7}}, {{0x0, 1}, {0x0, 2}, {0x0, 3}, {0x0, 4}, {0x0, 5}, {0x0, 6}, {0x0, 7}}},
			{"WriteAllOfStream 0-7 (stream 0x00 or 0x80 positions)", {0x080, 0x100, 0x280, 0x300, 0x480, 0x500, 0x680, 0x700, 0x000}, {}, (byte_t*)0x1000, 0x800, {{(byte_t*)0x1000, 0x100, 0}, {(byte_t*)0x1100, 0x100, 1}, {(byte_t*)0x1200, 0x100, 2}, {(byte_t*)0x1300, 0x100, 3}, {(byte_t*)0x1400, 0x100, 4}, {(byte_t*)0x1500, 0x100, 5}, {(byte_t*)0x1600, 0x100, 6}, {(byte_t*)0x1700, 0x100, 7}}, {{0x0, 2}, {0x0, 4}, {0x0, 6}}},
			{"WriteAllOfStream 3-6 (stream 0x0 positions)", {0x000, 0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x300}, {}, (byte_t*)0x1000, 0x400, {{(byte_t*)0x1000, 0x100, 3}, {(byte_t*)0x1100, 0x100, 4}, {(byte_t*)0x1200, 0x100, 5}, {(byte_t*)0x1300, 0x100, 6}}, {}},
			{"WriteAllOfStream 3-6 (stream 0x80 positions)", {0x080, 0x180, 0x280, 0x380, 0x480, 0x580, 0x680, 0x780, 0x300}, {}, (byte_t*)0x1000, 0x400, {{(byte_t*)0x1000, 0x100, 3}, {(byte_t*)0x1100, 0x100, 4}, {(byte_t*)0x1200, 0x100, 5}, {(byte_t*)0x1300, 0x100, 6}}, {{0x0, 4}, {0x0, 5}, {0x0, 6}}},
			{"WriteStream (partial)3,4-5,(partial)6 (stream 0x0 positions)", {0x000, 0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x350}, {}, (byte_t*)0x1000, 0x300, {{(byte_t*)0x1000, 0xB0, 3}, {(byte_t*)0x10B0, 0x100, 4}, {(byte_t*)0x11B0, 0x100, 5}, {(byte_t*)0x12B0, 0x50, 6}}, {}},
			{"WriteStream (partial)3,4-5,(partial)6 (stream 0x80 positions)", {0x080, 0x180, 0x280, 0x380, 0x480, 0x580, 0x680, 0x780, 0x350}, {}, (byte_t*)0x1000, 0x300, {{(byte_t*)0x1000, 0xB0, 3}, {(byte_t*)0x10B0, 0x100, 4}, {(byte_t*)0x11B0, 0x100, 5}, {(byte_t*)0x12B0, 0x50, 6}}, {{0x0, 4}, {0x0, 5}, {0x0, 6}}},
		};

		// run tests
		for (auto test = tests.begin(); test != tests.end(); test++)
		{
			for (auto itr = test->unlogged_seeks.begin(); itr != test->unlogged_seeks.end(); itr++)
			{
				stream.seek(*itr, tc::io::SeekOrigin::Begin);
			}

			write_report.clear();
			seek_report.clear();

			for (auto itr = test->logged_seeks.begin(); itr != test->logged_seeks.end(); itr++)
			{
				stream.seek(*itr, tc::io::SeekOrigin::Begin);
			}

			stream.write(test->write_ptr, test->write_count);

			if (write_report != test->exp_write_report)
			{
				throw tc::Exception(fmt::format("Test \"{}\" Failed: .write() issued to base streams were not as expected", test->test_name));
			}

			if (seek_report != test->exp_seek_report)
			{
				throw tc::Exception(fmt::format("Test \"{}\" Failed: .seek() issued to base streams were not as expected", test->test_name));
			}
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_write_CanWriteFromMultipleStreamWithNoSeekSupport()
{
	fmt::print("[tc::io::ConcatenatedStream] test_write_CanWriteFromMultipleStreamWithNoSeekSupport : ");

	class ReportDummyStream : public StreamTestUtil::DummyStreamBase
	{
	public:
		struct SeekReport
		{
			int64_t seek_offset;
			size_t stream_id;

			bool operator==(const SeekReport& other) const
			{
				return seek_offset == other.seek_offset \
					&& stream_id == other.stream_id;
			}
		};

		struct WriteReport
		{
			const byte_t* write_ptr;
			size_t write_count;
			size_t stream_id;

			bool operator==(const WriteReport& other) const
			{
				return write_ptr == other.write_ptr \
					&& write_count == other.write_count \
					&& stream_id == other.stream_id;
			}
		};

		ReportDummyStream(std::vector<WriteReport>& write_report, std::vector<SeekReport>& seek_report, size_t stream_id) :
			DummyStreamBase(0x100, true, true, false, false, false),
			mWriteReport(write_report),
			mSeekReport(seek_report),
			mStreamId(stream_id)
		{}

		size_t write(const byte_t* ptr, size_t count)
		{
			mWriteReport.push_back({ptr, count, mStreamId});

			size_t writable_count = tc::io::IOUtil::getWritableCount(DummyStreamBase::length(), DummyStreamBase::position(), count);

			// update stream position
			DummyStreamBase::seek(int64_t(writable_count), tc::io::SeekOrigin::Current);

			return writable_count;
		}

		int64_t seek(int64_t offset, tc::io::SeekOrigin origin)
		{	
			SeekReport seek_report = {DummyStreamBase::seek(offset, origin), mStreamId};

			mSeekReport.push_back(seek_report);

			return seek_report.seek_offset;
		}
	private:
		std::vector<WriteReport>& mWriteReport;
		std::vector<SeekReport>& mSeekReport;
		size_t mStreamId;
	};

	try
	{
		// declare report logs
		std::vector<ReportDummyStream::WriteReport> write_report;
		std::vector<ReportDummyStream::SeekReport> seek_report;

		// create stream list with links to report logs
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 0)), // 0x000 - 0x0ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 1)), // 0x100 - 0x1ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 2)), // 0x200 - 0x2ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 3)), // 0x300 - 0x3ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 4)), // 0x400 - 0x4ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 5)), // 0x500 - 0x5ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 6)), // 0x600 - 0x6ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 7)), // 0x700 - 0x7ff
		};

		// create concatenated stream
		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		// define test
		struct Test {
			// test name
			std::string test_name;

			// these seeks are done then seek log is cleared
			std::vector<int64_t> unlogged_seeks;

			// these seeks are done before writeing but will be logged
			std::vector<int64_t> logged_seeks;
			
			// write param
			byte_t* write_ptr;
			size_t write_count;

			// expected log reports
			std::vector<ReportDummyStream::WriteReport> exp_write_report;
			std::vector<ReportDummyStream::SeekReport> exp_seek_report;
		};

		// create tests
		std::vector<Test> tests {
			{"WriteStream (partial)0", {}, {}, (byte_t*)0x1000, 0xA0, {{(byte_t*)0x1000, 0xA0, 0}}, {}},
			{"WriteStream (partial)0,1-4", {}, {}, (byte_t*)0x1000, 0x460, {{(byte_t*)0x1000, 0x60, 0}, {(byte_t*)0x1060, 0x100, 1}, {(byte_t*)0x1160, 0x100, 2}, {(byte_t*)0x1260, 0x100, 3}, {(byte_t*)0x1360, 0x100, 4}}, {}},
			{"WriteStream 5-7", {}, {}, (byte_t*)0x1000, 0x300, {{(byte_t*)0x1000, 0x100, 5}, {(byte_t*)0x1100, 0x100, 6}, {(byte_t*)0x1200, 0x100, 7}}, {}},
		};

		// run tests
		for (auto test = tests.begin(); test != tests.end(); test++)
		{
			for (auto itr = test->unlogged_seeks.begin(); itr != test->unlogged_seeks.end(); itr++)
			{
				stream.seek(*itr, tc::io::SeekOrigin::Begin);
			}

			write_report.clear();
			seek_report.clear();

			for (auto itr = test->logged_seeks.begin(); itr != test->logged_seeks.end(); itr++)
			{
				stream.seek(*itr, tc::io::SeekOrigin::Begin);
			}

			stream.write(test->write_ptr, test->write_count);

			if (write_report != test->exp_write_report)
			{
				fmt::print("\n");
				fmt::print("WriteLog:\n");
				for (auto itr = write_report.begin(); itr != write_report.end(); itr++)
				{
					fmt::print("WriteReport (ptr: 0x{:x}, count: 0x{:x}, stream: {:d})\n", (size_t)itr->write_ptr, itr->write_count, itr->stream_id);
				}
				fmt::print("ExpWriteLog:\n");
				for (auto itr = test->exp_write_report.begin(); itr != test->exp_write_report.end(); itr++)
				{
					fmt::print("WriteReport (ptr: 0x{:x}, count: 0x{:x}, stream: {:d})\n", (size_t)itr->write_ptr, itr->write_count, itr->stream_id);
				}
				throw tc::Exception(fmt::format("Test \"{}\" Failed: .write() issued to base streams were not as expected", test->test_name));
			}

			if (seek_report != test->exp_seek_report)
			{
				throw tc::Exception(fmt::format("Test \"{}\" Failed: .seek() issued to base streams were not as expected", test->test_name));
			}
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_write_WriteFromMultiStream_NoSeekSupport_ThrowsOnSeekRequired()
{
	fmt::print("[tc::io::ConcatenatedStream] test_write_WriteFromMultiStream_NoSeekSupport_ThrowsOnSeekRequired : ");

	class ReportDummyStream : public StreamTestUtil::DummyStreamBase
	{
	public:
		struct SeekReport
		{
			int64_t seek_offset;
			size_t stream_id;

			bool operator==(const SeekReport& other) const
			{
				return seek_offset == other.seek_offset \
					&& stream_id == other.stream_id;
			}
		};

		struct WriteReport
		{
			const byte_t* write_ptr;
			size_t write_count;
			size_t stream_id;

			bool operator==(const WriteReport& other) const
			{
				return write_ptr == other.write_ptr \
					&& write_count == other.write_count \
					&& stream_id == other.stream_id;
			}
		};

		ReportDummyStream(std::vector<WriteReport>& write_report, std::vector<SeekReport>& seek_report, size_t stream_id) :
			DummyStreamBase(0x100, 0x80, true, true, false, false, false), // the initial position is 0x80
			mWriteReport(write_report),
			mSeekReport(seek_report),
			mStreamId(stream_id)
		{}

		size_t write(const byte_t* ptr, size_t count)
		{
			mWriteReport.push_back({ptr, count, mStreamId});

			size_t writable_count = tc::io::IOUtil::getWritableCount(DummyStreamBase::length(), DummyStreamBase::position(), count);

			// update stream position
			DummyStreamBase::seek(int64_t(writable_count), tc::io::SeekOrigin::Current);

			return writable_count;
		}

		int64_t seek(int64_t offset, tc::io::SeekOrigin origin)
		{	
			SeekReport seek_report = {DummyStreamBase::seek(offset, origin), mStreamId};

			mSeekReport.push_back(seek_report);

			return seek_report.seek_offset;
		}
	private:
		std::vector<WriteReport>& mWriteReport;
		std::vector<SeekReport>& mSeekReport;
		size_t mStreamId;
	};

	try
	{
		// declare report logs
		std::vector<ReportDummyStream::WriteReport> write_report;
		std::vector<ReportDummyStream::SeekReport> seek_report;

		// create stream list with links to report logs
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 0)), // 0x000 - 0x0ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 1)), // 0x100 - 0x1ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 2)), // 0x200 - 0x2ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 3)), // 0x300 - 0x3ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 4)), // 0x400 - 0x4ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 5)), // 0x500 - 0x5ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 6)), // 0x600 - 0x6ff
			std::make_shared<ReportDummyStream>(ReportDummyStream(write_report, seek_report, 7)), // 0x700 - 0x7ff
		};

		// create concatenated stream
		tc::io::ConcatenatedStream stream = tc::io::ConcatenatedStream(streams);

		try
		{
			stream.write((byte_t*)0x1000, 0x200);
			throw tc::Exception(".write() did not throw tc::io::IOException where stream required seeking to begining but did not support seeking.");
		}
		catch (const tc::io::IOException&)
		{
			// do nothing
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_MoveOperator_MoveDisposedToDisposed()
{
	fmt::print("[tc::io::ConcatenatedStream] test_MoveOperator_MoveDisposedToDisposed : ");
	try
	{
		// create streams a and b (both disposed)
		tc::io::ConcatenatedStream stream_a;
		tc::io::ConcatenatedStream stream_b;

		// ensure stream a had valid properties to begin with
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_a, 0x0, 0x0, false, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_a had wrong properies after default .ctor() ({})", e.error()));
		}
		try
		{
			stream_a.seek(0, tc::io::SeekOrigin::Current);
			throw tc::Exception("stream_a was disposed upon construction, but failed throw tc::ObjectDisposedException when seek() was called");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		// ensure stream b had valid properties to begin with
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_b, 0x0, 0x0, false, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_b had wrong properies after default .ctor() ({})", e.error()));
		}
		try
		{
			stream_b.seek(0, tc::io::SeekOrigin::Current);
			throw tc::Exception("stream_b was disposed upon construction, but failed throw tc::ObjectDisposedException when seek() was called");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		// move stream_a to stream_b
		stream_b = std::move(stream_a);

		// ensure stream a had valid properties after being moved from
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_a, 0x0, 0x0, false, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_a had wrong properies after it was move assigned to stream_b ({})", e.error()));
		}
		try
		{
			stream_a.seek(0, tc::io::SeekOrigin::Current);
			throw tc::Exception("stream_a was disposed upon being move assigned to stream_b, but failed throw tc::ObjectDisposedException when seek() was called");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		// ensure stream b had valid properties after being moved to
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_b, 0x0, 0x0, false, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_b has wrong properties after being move assigned from stream_a ({})", e.error()));
		}
		try
		{
			stream_b.seek(0, tc::io::SeekOrigin::Current);
			throw tc::Exception("stream_b was disposed upon move assignment from stream_a, but failed throw tc::ObjectDisposedException when seek() was called");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_MoveOperator_MoveInitializedToDisposed()
{
	fmt::print("[tc::io::ConcatenatedStream] test_MoveOperator_MoveInitializedToDisposed : ");
	try
	{
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false))
		};

		// create streams a and b
		tc::io::ConcatenatedStream stream_a(streams);
		tc::io::ConcatenatedStream stream_b;

		// ensure stream a had valid properties to begin with
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_a, 0x300, 0x0, true, true, true);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_a had wrong properies after create .ctor() ({})", e.error()));
		}
		try
		{
			stream_a.seek(0, tc::io::SeekOrigin::Current);
		}
		catch (const tc::ObjectDisposedException&)
		{
			throw tc::Exception("stream_a was initialized upon construction, but threw tc::ObjectDisposedException when seek() was called");
		}

		// ensure stream b had valid properties to begin with
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_b, 0x0, 0x0, false, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_b had wrong properies after default .ctor() ({})", e.error()));
		}
		try
		{
			stream_b.seek(0, tc::io::SeekOrigin::Current);
			throw tc::Exception("stream_b was disposed upon construction, but failed throw tc::ObjectDisposedException when seek() was called");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		// move stream_a to stream_b
		stream_b = std::move(stream_a);

		// ensure stream a had valid properties after being moved from
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_a, 0x0, 0x0, false, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_a had wrong properies after it was move assigned to stream_b ({})", e.error()));
		}
		try
		{
			stream_a.seek(0, tc::io::SeekOrigin::Current);
			throw tc::Exception("stream_a was disposed upon being move assigned to stream_b, but failed throw tc::ObjectDisposedException when seek() was called");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		// ensure stream b had valid properties after being moved to
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_b, 0x300, 0x0, true, true, true);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_b has wrong properties after being move assigned from stream_a ({})", e.error()));
		}
		try
		{
			stream_b.seek(0, tc::io::SeekOrigin::Current);
			
		}
		catch (const tc::ObjectDisposedException&)
		{
			throw tc::Exception("stream_b was initialized upon move assignment from stream_a, but threw tc::ObjectDisposedException when seek() was called");
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_MoveOperator_MoveDisposedToInitialized()
{
	fmt::print("[tc::io::ConcatenatedStream] test_MoveOperator_MoveDisposedToInitialized : ");
	try
	{
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false))
		};

		// create streams a and b
		tc::io::ConcatenatedStream stream_a;
		tc::io::ConcatenatedStream stream_b(streams);

		// ensure stream b had valid properties to begin with
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_a, 0x0, 0x0, false, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_a had wrong properies after default .ctor() ({})", e.error()));
		}
		try
		{
			stream_a.seek(0, tc::io::SeekOrigin::Current);
			throw tc::Exception("stream_a was disposed upon construction, but failed throw tc::ObjectDisposedException when seek() was called");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		// ensure stream a had valid properties to begin with
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_b, 0x300, 0x0, true, true, true);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_b had wrong properies after create .ctor() ({})", e.error()));
		}
		try
		{
			stream_b.seek(0, tc::io::SeekOrigin::Current);
		}
		catch (const tc::ObjectDisposedException&)
		{
			throw tc::Exception("stream_b was initialized upon construction, but threw tc::ObjectDisposedException when seek() was called");
		}

		// move stream_a to stream_b
		stream_b = std::move(stream_a);

		// ensure stream a had valid properties after being moved from
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_a, 0x0, 0x0, false, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_a had wrong properies after it was move assigned to stream_b ({})", e.error()));
		}
		try
		{
			stream_a.seek(0, tc::io::SeekOrigin::Current);
			throw tc::Exception("stream_a was disposed upon being move assigned to stream_b, but failed throw tc::ObjectDisposedException when seek() was called");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		// ensure stream b had valid properties after being moved to
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_b, 0x0, 0x0, false, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_b has wrong properties after being move assigned from stream_a ({})", e.error()));
		}
		try
		{
			stream_b.seek(0, tc::io::SeekOrigin::Current);
			throw tc::Exception("stream_b was disposed upon move assignment from stream_a, but failed throw tc::ObjectDisposedException when seek() was called");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_MoveOperator_MoveInitializedToInitialized()
{
	fmt::print("[tc::io::ConcatenatedStream] test_MoveOperator_MoveInitializedToInitialized : ");
	try
	{
		std::vector<std::shared_ptr<tc::io::IStream>> streams_a {
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false))
		};

		std::vector<std::shared_ptr<tc::io::IStream>> streams_b {
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x200, true, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x200, true, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x200, true, true, true, false, false))
		};

		// create streams a and b
		tc::io::ConcatenatedStream stream_a(streams_a);
		tc::io::ConcatenatedStream stream_b(streams_b);

		// ensure stream a had valid properties to begin with
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_a, 0x300, 0x0, true, true, true);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_a had wrong properies after create .ctor() ({})", e.error()));
		}
		try
		{
			stream_a.seek(0, tc::io::SeekOrigin::Current);
		}
		catch (const tc::ObjectDisposedException&)
		{
			throw tc::Exception("stream_a was initialized upon construction, but threw tc::ObjectDisposedException when seek() was called");
		}

		// ensure stream a had valid properties to begin with
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_b, 0x600, 0x0, true, true, true);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_b had wrong properies after create .ctor() ({})", e.error()));
		}
		try
		{
			stream_b.seek(0, tc::io::SeekOrigin::Current);
		}
		catch (const tc::ObjectDisposedException&)
		{
			throw tc::Exception("stream_b was initialized upon construction, but threw tc::ObjectDisposedException when seek() was called");
		}

		// move stream_a to stream_b
		stream_b = std::move(stream_a);

		// ensure stream a had valid properties after being moved from
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_a, 0x0, 0x0, false, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_a had wrong properies after it was move assigned to stream_b ({})", e.error()));
		}
		try
		{
			stream_a.seek(0, tc::io::SeekOrigin::Current);
			throw tc::Exception("stream_a was disposed upon being move assigned to stream_b, but failed throw tc::ObjectDisposedException when seek() was called");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		// ensure stream b had valid properties after being moved to
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_b, 0x300, 0x0, true, true, true);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_b has wrong properties after being move assigned from stream_a ({})", e.error()));
		}
		try
		{
			stream_b.seek(0, tc::io::SeekOrigin::Current);
			
		}
		catch (const tc::ObjectDisposedException&)
		{
			throw tc::Exception("stream_b was initialized upon move assignment from stream_a, but threw tc::ObjectDisposedException when seek() was called");
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_MoveConstructor_MoveDisposed()
{
	fmt::print("[tc::io::ConcatenatedStream] test_MoveConstructor_MoveDisposed : ");
	try
	{
		// create stream a
		tc::io::ConcatenatedStream stream_a;

		// ensure stream a had valid properties to begin with
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_a, 0x0, 0x0, false, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_a had wrong properies after default .ctor() ({})", e.error()));
		}
		try
		{
			stream_a.seek(0, tc::io::SeekOrigin::Current);
			throw tc::Exception("stream_a was disposed upon construction, but failed throw tc::ObjectDisposedException when seek() was called");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		// move stream_a to stream_b
		tc::io::ConcatenatedStream stream_b(std::move(stream_a));

		// ensure stream a had valid properties after being moved from
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_a, 0x0, 0x0, false, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_a had wrong properies after it was move assigned to stream_b ({})", e.error()));
		}
		try
		{
			stream_a.seek(0, tc::io::SeekOrigin::Current);
			throw tc::Exception("stream_a was disposed upon being move assigned to stream_b, but failed throw tc::ObjectDisposedException when seek() was called");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		// ensure stream b had valid properties after being moved to
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_b, 0x0, 0x0, false, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_b has wrong properties after being move assigned from stream_a ({})", e.error()));
		}
		try
		{
			stream_b.seek(0, tc::io::SeekOrigin::Current);
			throw tc::Exception("stream_b was disposed upon move assignment from stream_a, but failed throw tc::ObjectDisposedException when seek() was called");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}

void io_ConcatenatedStream_TestClass::test_MoveConstructor_MoveInitialized()
{
	fmt::print("[tc::io::ConcatenatedStream] test_MoveConstructor_MoveInitialized : ");
	try
	{
		std::vector<std::shared_ptr<tc::io::IStream>> streams {
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false)),
			std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0x100, true, true, true, false, false))
		};

		// create stream a
		tc::io::ConcatenatedStream stream_a(streams);

		// ensure stream a had valid properties to begin with
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_a, 0x300, 0x0, true, true, true);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_a had wrong properies after create .ctor() ({})", e.error()));
		}
		try
		{
			stream_a.seek(0, tc::io::SeekOrigin::Current);
		}
		catch (const tc::ObjectDisposedException&)
		{
			throw tc::Exception("stream_a was initialized upon construction, but threw tc::ObjectDisposedException when seek() was called");
		}

		// move stream_a to stream_b
		tc::io::ConcatenatedStream stream_b(std::move(stream_a));

		// ensure stream a had valid properties after being moved from
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_a, 0x0, 0x0, false, false, false);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_a had wrong properies after it was move assigned to stream_b ({})", e.error()));
		}
		try
		{
			stream_a.seek(0, tc::io::SeekOrigin::Current);
			throw tc::Exception("stream_a was disposed upon being move assigned to stream_b, but failed throw tc::ObjectDisposedException when seek() was called");
		}
		catch (const tc::ObjectDisposedException&)
		{
			// do nothing
		}

		// ensure stream b had valid properties after being moved to
		try
		{
			StreamTestUtil::constructor_TestHelper(stream_b, 0x300, 0x0, true, true, true);
		}
		catch (const tc::Exception& e)
		{
			throw tc::Exception(fmt::format("stream_b has wrong properties after being move assigned from stream_a ({})", e.error()));
		}
		try
		{
			stream_b.seek(0, tc::io::SeekOrigin::Current);
			
		}
		catch (const tc::ObjectDisposedException&)
		{
			throw tc::Exception("stream_b was initialized upon move assignment from stream_a, but threw tc::ObjectDisposedException when seek() was called");
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL (unhandled exception) ({})\n", e.what());
	}
}