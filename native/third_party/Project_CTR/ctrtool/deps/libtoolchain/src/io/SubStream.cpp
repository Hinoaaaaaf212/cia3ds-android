#include <tc/io/SubStream.h>
#include <tc/io/IOUtil.h>
#include <tc/io/StreamUtil.h>
#include <algorithm>

const std::string tc::io::SubStream::kClassName = "tc::io::SubStream";

tc::io::SubStream::SubStream() :
	mBaseStream(),
	mBaseStreamOffset(0),
	mSubStreamLength(0),
	mSubStreamPosition(0)
{}
	

tc::io::SubStream::SubStream(const std::shared_ptr<tc::io::IStream>& stream, int64_t offset, int64_t length) :
	SubStream()
{
	// copy stream
	mBaseStream = stream;

	// validate the stream exists
	if (mBaseStream == nullptr)
	{
		throw tc::ArgumentNullException(kClassName, "stream is null");
	}

	// check if the stream supports seeking
	if (mBaseStream->canSeek() == false)
	{
		tc::NotSupportedException(kClassName, "Streams that do not support seeking are not supported");
	}

	
	// validate arguments
	if (offset < 0)
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "offset is negative");
	}
	if (length < 0)
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "length is negative");
	}

	int64_t base_length = mBaseStream->length();

	// validate arguments against stream length
	// substream length should not be greater than the base stream length
	if (length > base_length)
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "SubStream length is greater than base stream length");
	}
	// Base length - length is the maximum possible offset for the substream
	if (offset > (base_length - length))
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "SubStream offset is greater than the maximum possible offset given the base stream size and SubStream size");
	}
	
	// set class state
	mBaseStreamOffset = offset;
	mSubStreamLength = length;
	mSubStreamPosition = 0;
}

bool tc::io::SubStream::canRead() const
{
	return mBaseStream == nullptr ? false : mBaseStream->canRead();
}

bool tc::io::SubStream::canWrite() const
{
	return mBaseStream == nullptr ? false : mBaseStream->canWrite();
}
bool tc::io::SubStream::canSeek() const
{
	return mBaseStream == nullptr ? false : mBaseStream->canSeek();
}

int64_t tc::io::SubStream::length()
{
	return mBaseStream == nullptr ? 0 : mSubStreamLength;
}

int64_t tc::io::SubStream::position()
{
	return mBaseStream == nullptr ? 0 : mSubStreamPosition;
}

size_t tc::io::SubStream::read(byte_t* ptr, size_t count)
{
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::read()", "Failed to read from stream (stream is disposed)");
	}

	count = IOUtil::getReadableCount(mSubStreamLength, mSubStreamPosition, count);

	// assert proper position in file
	mBaseStream->seek(mBaseStreamOffset + mSubStreamPosition, SeekOrigin::Begin);

	// read data
	size_t data_read_size = mBaseStream->read(ptr, count);

	// update sub stream position
	seek(data_read_size, SeekOrigin::Current);

	return data_read_size;
}

size_t tc::io::SubStream::write(const byte_t* ptr, size_t count)
{
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::write()", "Failed to write to stream (stream is disposed)");
	}

	count = IOUtil::getWritableCount(mSubStreamLength, mSubStreamPosition, count);

	// assert proper position in file
	mBaseStream->seek(mBaseStreamOffset + mSubStreamPosition, SeekOrigin::Begin);

	// write data
	size_t data_written_size = mBaseStream->write(ptr, count);

	// update sub stream position
	seek(data_written_size, SeekOrigin::Current);

	return data_written_size;
}

int64_t tc::io::SubStream::seek(int64_t offset, SeekOrigin origin)
{
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::seek()", "Failed to set stream position (stream is disposed)");
	}

	mSubStreamPosition = StreamUtil::getSeekResult(offset, origin, mSubStreamPosition, mSubStreamLength);

	if (mSubStreamPosition < 0)
	{
		throw tc::InvalidOperationException(kClassName+"::seek()", "Negative seek result determined");
	}

	return mSubStreamPosition;
}

void tc::io::SubStream::setLength(int64_t length)
{
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::setLength()", "Failed to set stream length (stream is disposed)");
	}

	throw tc::NotImplementedException(kClassName+"::setLength()", "setLength is not implemented for SubStream");
}

void tc::io::SubStream::flush()
{
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::seek()", "Failed to flush stream (stream is disposed)");
	}

	mBaseStream->flush();
}

void tc::io::SubStream::dispose()
{
	if (mBaseStream.get() != nullptr)
	{
		// dispose base stream
		mBaseStream->dispose();

		// release ptr
		mBaseStream.reset();
	}
	
	// clear state
	mBaseStreamOffset = 0;
	mSubStreamLength = 0;
	mSubStreamPosition = 0;
}