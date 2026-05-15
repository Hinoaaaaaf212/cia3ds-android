#include "StreamTestUtil.h"
#include <fmt/core.h>
#include <tc/cli.h>

const std::string StreamTestUtil::DummyStreamBase::kClassName = "DummyStreamBase";

void StreamTestUtil::constructor_TestHelper(tc::io::IStream& stream, int64_t stream_length, int64_t exp_pos_res, bool exp_canread_res, bool exp_canwrite_res, bool exp_canseek_res)
{
	int64_t length_res = stream.length();
	int64_t pos_res = stream.position();
	bool can_read = stream.canRead();
	bool can_write = stream.canWrite();
	bool can_seek = stream.canSeek();

	if (length_res != stream_length)
	{		
		throw tc::Exception(fmt::format("Stream did not have length {:d} (actual {:d})", stream_length, length_res));
	}

	if (pos_res != exp_pos_res)
	{
		throw tc::Exception(fmt::format("Stream did not have position {:d} (actual {:d})", exp_pos_res, pos_res));
	}

	if (can_read != exp_canread_res)
	{
		throw tc::Exception(fmt::format("Stream property canRead() was not {} (actual {})", exp_canread_res, can_read));
	}

	if (can_write != exp_canwrite_res)
	{
		throw tc::Exception(fmt::format("Stream property canWrite() was not {} (actual {})", exp_canwrite_res, can_write));
	}

	if (can_seek != exp_canseek_res)
	{
		throw tc::Exception(fmt::format("Stream property canSeek() was not {} (actual {})", exp_canseek_res, can_seek));
	}
}

void StreamTestUtil::seek_TestHelper(tc::io::IStream& stream, int64_t seek_offset, tc::io::SeekOrigin seek_origin, int64_t exp_seek_res, int64_t exp_pos_res)
{
	int64_t seek_res = stream.seek(seek_offset, seek_origin);
	int64_t pos_res = stream.position();

	if (seek_res != exp_seek_res)
	{
		throw tc::Exception(fmt::format("Stream did not return position from seek {:d} (actual {:d})", exp_seek_res, seek_res));
	}

	if (pos_res != exp_pos_res)
	{
		throw tc::Exception(fmt::format("Stream did not have position {:d} (actual {:d})", exp_pos_res, pos_res));
	}
}

void StreamTestUtil::read_TestHelper(tc::io::IStream& stream, int64_t seek_offset, tc::io::SeekOrigin seek_origin, size_t dst_size, size_t read_count, size_t exp_read_res, int64_t exp_pos_res, const byte_t* expected_data)
{
	tc::ByteData data(dst_size);

	// offset the position
	stream.seek(seek_offset, seek_origin);

	// read data
	size_t read_res = stream.read(data.data(), read_count);

	// check position
	int64_t pos_res = stream.position();

	// validate read result
	if (read_res != exp_read_res)
	{
		throw tc::Exception(fmt::format("Stream did not read expected number of bytes {:d} (actual {:d})", exp_read_res, read_res));
	}
	
	// validate read data
	if (expected_data != nullptr && memcmp(data.data(), expected_data, exp_read_res) != 0)
	{
		throw tc::Exception(fmt::format("Stream did not read expected bytes (read: \"{}\", expected: \"{}\"", tc::cli::FormatUtil::formatBytesAsString(data.data(), data.size(), true, ""), tc::cli::FormatUtil::formatBytesAsString(expected_data, exp_read_res, true, "")));
	}

	// validate pos result
	if (pos_res != exp_pos_res)
	{
		throw tc::Exception(fmt::format("Stream did not have position {:d} (actual {:d})", exp_pos_res, pos_res));
	}
}

void StreamTestUtil::write_TestHelper(tc::io::IStream& stream, int64_t seek_offset, tc::io::SeekOrigin seek_origin, tc::ByteData& data, int64_t exp_pos_res)
{
	write_TestHelper(stream, seek_offset, seek_origin, data.data(), data.size(), exp_pos_res);
}

void StreamTestUtil::write_TestHelper(tc::io::IStream& stream, int64_t seek_offset, tc::io::SeekOrigin seek_origin, tc::ByteData& data, int64_t exp_pos_res, int64_t exp_length_res)
{
	write_TestHelper(stream, seek_offset, seek_origin, data.data(), data.size(), exp_pos_res, exp_length_res);
}

void StreamTestUtil::write_TestHelper(tc::io::IStream& stream, int64_t seek_offset, tc::io::SeekOrigin seek_origin, const byte_t* data, size_t data_size, int64_t exp_pos_res)
{
	// offset the position
	stream.seek(seek_offset, seek_origin);

	stream.write(data, data_size);

	int64_t pos_res = stream.position();

	// validate pos result
	if (pos_res != exp_pos_res)
	{
		throw tc::Exception(fmt::format("Stream did not have position {:d} (actual {:d})", exp_pos_res, pos_res));
	}
}

void StreamTestUtil::write_TestHelper(tc::io::IStream& stream, int64_t seek_offset, tc::io::SeekOrigin seek_origin, const byte_t* data, size_t data_size, int64_t exp_pos_res, int64_t exp_length_res)
{
	write_TestHelper(stream, seek_offset, seek_origin, data, data_size, exp_pos_res);

	// validate length result
	int64_t length_res = stream.length();
	if (length_res != exp_length_res)
	{
		throw tc::Exception(fmt::format("Stream did not have length {:d} (actual {:d})", exp_length_res, length_res));
	}
}