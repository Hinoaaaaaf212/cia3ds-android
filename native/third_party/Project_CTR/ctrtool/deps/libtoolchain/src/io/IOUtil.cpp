#include <tc/io/IOUtil.h>

int64_t tc::io::IOUtil::castSizeToInt64(size_t size)
{
	if (std::numeric_limits<size_t>::digits > std::numeric_limits<int64_t>::digits)
		size = std::min<size_t>(size, size_t(std::numeric_limits<int64_t>::max()));

	return int64_t(size);
}

size_t tc::io::IOUtil::castInt64ToSize(int64_t length)
{
	if (length < 0)
		return 0;

	if (std::numeric_limits<size_t>::digits < std::numeric_limits<int64_t>::digits)
		length = std::min<int64_t>(length, int64_t(std::numeric_limits<size_t>::max()));

	return size_t(length);
}

size_t tc::io::IOUtil::getAvailableSize(int64_t data_length, int64_t data_offset)
{
	if (data_length < 0 || data_offset < 0)
		return 0;

	int64_t readable_length = (data_offset < data_length) ? (data_length - data_offset) : 0;

	return castInt64ToSize(readable_length);
}

size_t tc::io::IOUtil::getReadableCount(int64_t data_length, int64_t data_offset, size_t requested_read_count)
{
	return std::min<size_t>(getAvailableSize(data_length, data_offset), requested_read_count);
}

size_t tc::io::IOUtil::getWritableCount(int64_t data_length, int64_t data_offset, size_t requested_write_count)
{
	return getReadableCount(data_length, data_offset, requested_write_count);
}