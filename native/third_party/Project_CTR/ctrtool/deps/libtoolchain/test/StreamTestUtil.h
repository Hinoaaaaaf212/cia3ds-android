#pragma once

#include <tc/io/IStream.h>
#include <tc/ByteData.h>
#include <tc/NotImplementedException.h>
#include <tc/ArgumentOutOfRangeException.h>

class StreamTestUtil
{
public:
	static void constructor_TestHelper(tc::io::IStream& stream, int64_t stream_length, int64_t exp_pos_res, bool exp_canread_res, bool exp_canwrite_res, bool exp_canseek_res);
	static void seek_TestHelper(tc::io::IStream& stream, int64_t seek_offset, tc::io::SeekOrigin seek_origin, int64_t exp_seek_res, int64_t exp_pos_res);
	static void read_TestHelper(tc::io::IStream& stream, int64_t seek_offset, tc::io::SeekOrigin seek_origin, size_t dst_size, size_t read_count, size_t exp_read_res, int64_t exp_pos_res, const byte_t* expected_data = nullptr);
	static void write_TestHelper(tc::io::IStream& stream, int64_t seek_offset, tc::io::SeekOrigin seek_origin, tc::ByteData& data, int64_t exp_pos_res);
	static void write_TestHelper(tc::io::IStream& stream, int64_t seek_offset, tc::io::SeekOrigin seek_origin, tc::ByteData& data, int64_t exp_pos_res, int64_t exp_length_res);
	static void write_TestHelper(tc::io::IStream& stream, int64_t seek_offset, tc::io::SeekOrigin seek_origin, const byte_t* data, size_t data_size, int64_t exp_pos_res);
	static void write_TestHelper(tc::io::IStream& stream, int64_t seek_offset, tc::io::SeekOrigin seek_origin, const byte_t* data, size_t data_size, int64_t exp_pos_res, int64_t exp_length_res);

	class DummyStreamBase : public tc::io::IStream
	{
	public:
		DummyStreamBase() :
			DummyStreamBase(0x10000000)
		{
		}

		DummyStreamBase(int64_t length) :
			DummyStreamBase(length, true, true, true, true, true)
		{
		}

		DummyStreamBase(int64_t length, bool canRead, bool canWrite, bool canSeek, bool canSeekOnlyFromBegin, bool canSetLength)
		{
			init(length, canRead, canWrite, canSeek, canSeekOnlyFromBegin, canSetLength);
		}

		DummyStreamBase(int64_t length, int64_t position, bool canRead, bool canWrite, bool canSeek, bool canSeekOnlyFromBegin, bool canSetLength)
		{
			init(length, position, canRead, canWrite, canSeek, canSeekOnlyFromBegin, canSetLength);
		}

		void init(int64_t length, bool canRead, bool canWrite, bool canSeek, bool canSeekOnlyFromBegin, bool canSetLength)
		{
			init(length, 0, canRead, canWrite, canSeek, canSeekOnlyFromBegin, canSetLength);
		}

		void init(int64_t length, int64_t position, bool canRead, bool canWrite, bool canSeek, bool canSeekOnlyFromBegin, bool canSetLength)
		{
			mCanRead = canRead;
			mCanWrite = canWrite;
			mCanSeek = canSeek;
			mCanSeekOnlyFromBegin = canSeekOnlyFromBegin;
			mPosition = position;
			mLength = length;
		}

		bool canRead() const
		{
			return mCanRead;
		}

		bool canWrite() const
		{
			return mCanWrite;
		}

		bool canSeek() const
		{
			return mCanSeek;
		}

		int64_t length()
		{
			return mLength;
		}

		int64_t position()
		{
			return mPosition;
		}

		virtual size_t read(byte_t* ptr, size_t count)
		{
			throw tc::NotImplementedException(kClassName, "read() not implemented");
		}

		virtual size_t write(const byte_t* ptr, size_t count)
		{
			throw tc::NotImplementedException(kClassName, "write() not implemented");
		}

		int64_t seek(int64_t offset, tc::io::SeekOrigin origin)
		{
			if (origin != tc::io::SeekOrigin::Begin && mCanSeekOnlyFromBegin)
				throw tc::ArgumentOutOfRangeException(kClassName, "Should not be passing seek origin values that are not SeekOrigin::Begin to the base stream");
			
			switch (origin)
			{
			case (tc::io::SeekOrigin::Begin):
				mPosition = offset;
				break;
			case (tc::io::SeekOrigin::Current):
				mPosition += offset;
				break;
			case (tc::io::SeekOrigin::End):
				mPosition = mLength + offset;
				break;
			default:
				throw tc::ArgumentOutOfRangeException(kClassName, "Illegal seek origin"); 
			}

			if (mPosition > mLength)
			{
				throw tc::ArgumentOutOfRangeException(kClassName, "Illegal seek position"); 
			}

			return mPosition;
		}

		void setLength(int64_t length)
		{
			if (mCanSetLength == false)
				throw tc::NotImplementedException(kClassName, "setLength() is not implemented");

			mLength = length;
		}

		void flush()
		{
			// nothing
		}

		void dispose()
		{
			flush();
			mCanRead = false;
			mCanWrite = false;
			mCanSeek = false;
			mCanSetLength = false;
			mPosition = 0;
			mLength = 0;
		}
	private:
		static const std::string kClassName;
		bool mCanRead;
		bool mCanWrite;
		bool mCanSeek;
		bool mCanSeekOnlyFromBegin;
		bool mCanSetLength;
		int64_t mPosition;
		int64_t mLength;
	};
	
};