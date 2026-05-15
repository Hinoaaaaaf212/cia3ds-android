#include <tc/io/ConcatenatedStream.h>

#include <tc/io/StreamUtil.h>
#include <tc/io/IOUtil.h>

const std::string tc::io::ConcatenatedStream::kClassName = "tc::io::ConcatenatedStream";

tc::io::ConcatenatedStream::ConcatenatedStream() :
	mStreamList(),
	mStreamListMap(),
	mCurrentStream(),
	mCanRead(false),
	mCanWrite(false),
	mCanSeek(false),
	mStreamLength(0)
{}

tc::io::ConcatenatedStream::ConcatenatedStream(ConcatenatedStream&& other) :
	ConcatenatedStream()
{
	*this = std::move(other);
}

tc::io::ConcatenatedStream::ConcatenatedStream(const std::vector<std::shared_ptr<tc::io::IStream>>& stream_list) :
	ConcatenatedStream()
{
	// track the overall stream properties
	bool can_read = true;
	bool can_write = true;
	bool can_seek = true;

	// reset stream length
	mStreamLength = 0;

	// process stream list
	for (auto itr = stream_list.begin(); itr != stream_list.end(); itr++)
	{
		// skip null streams
		if (*itr == nullptr)
			continue;
		// skip empty streams
		if ((*itr)->length() == 0)
			continue;
		// skip streams that can't be read or writen to (so it's useless)
		if ((*itr)->canRead() == false && (*itr)->canWrite() == false)
			continue;

		// create stream info for the input stream
		StreamInfo info;
		// range is from the current concatenated stream position for the length of the input stream
		info.range = StreamRange(mStreamLength, (*itr)->length());
		info.stream = *itr;

		// throw an exception if the input stream range overlaps with an existing range (which shouldn't be possible)
		if (mStreamListMap.find(info.range) != mStreamListMap.end())
		{
			throw tc::Exception(kClassName, "Poor state management detected.");
		}

		if (info.stream->canRead() == false)
			can_read = false;
		if (info.stream->canWrite() == false)
			can_write = false;
		if (info.stream->canSeek() == false)
			can_seek = false;

		mStreamListMap.insert(std::pair<StreamRange, size_t>(info.range, mStreamList.size()));
		mStreamList.push_back(info);

		mStreamLength += info.range.length;
	}

	// check the stream is usable
	if (!can_read && !can_write)
	{
		throw tc::NotSupportedException(kClassName, "Stream does not support read or write.");
	}
	// check the stream is usable
	if (mStreamLength == 0)
	{
		throw tc::NotSupportedException(kClassName, "Stream had no length.");
	}

	// save iterator to current stream
	mCurrentStream = mStreamList.begin();

	// save properties
	mCanRead = can_read;
	mCanWrite = can_write;
	mCanSeek = can_seek;
}

tc::io::ConcatenatedStream::~ConcatenatedStream()
{
	dispose();
}

tc::io::ConcatenatedStream& tc::io::ConcatenatedStream::operator=(tc::io::ConcatenatedStream&& other)
{
	mStreamList = std::move(other.mStreamList);
	mStreamListMap = std::move(other.mStreamListMap);
	mCurrentStream = std::move(other.mCurrentStream);
	mCanRead = other.mCanRead;
	mCanWrite = other.mCanWrite;
	mCanSeek = other.mCanSeek;
	mStreamLength = other.mStreamLength;
	
	// clear other state
	other.mStreamList.clear();
	other.mStreamListMap.clear();
	other.mCurrentStream.makeNull();
	other.mCanRead = false;
	other.mCanWrite = false;
	other.mCanSeek = false;
	other.mStreamLength = 0;

	return *this;
}

bool tc::io::ConcatenatedStream::canRead() const
{
	return isStreamDisposed() ? false : mCanRead;
}

bool tc::io::ConcatenatedStream::canWrite() const
{
	return isStreamDisposed() ? false : mCanWrite;
}

bool tc::io::ConcatenatedStream::canSeek() const
{
	return isStreamDisposed() ? false : mCanSeek;
}

int64_t tc::io::ConcatenatedStream::length()
{
	return isStreamDisposed() ? 0 : mStreamLength;
}

int64_t tc::io::ConcatenatedStream::position()
{
	return isStreamDisposed() ? 0 : (mCurrentStream.get()->range.offset + mCurrentStream.get()->stream->position());
}

size_t tc::io::ConcatenatedStream::read(byte_t* ptr, size_t count)
{
	if (isStreamDisposed())
	{
		throw tc::ObjectDisposedException(kClassName+"read()", "Stream wasd.");
	}
	if (mCanRead == false)
	{
		throw tc::NotSupportedException(kClassName+"read()", "Stream does not support reading.");
	}

	// read
	size_t readable_count = IOUtil::getReadableCount(mStreamLength, position(), count);
	size_t remaining_readable_count = readable_count;
	do {
		// determine expected readable data count for the current stream
		size_t readable_count_for_current_stream = IOUtil::getReadableCount(mCurrentStream.get()->range.length, mCurrentStream.get()->stream->position(), remaining_readable_count);

		if (readable_count_for_current_stream != 0)
		{
			// read data and throw exception if unexpected read count is returned
			if (readable_count_for_current_stream != mCurrentStream.get()->stream->read(ptr + (readable_count - remaining_readable_count), readable_count_for_current_stream))
			{
				throw tc::io::IOException(kClassName+"read()", "Reading from one of the base streams returned less data than expected.");
			}

			// decrement the remaining readable count
			remaining_readable_count -= readable_count_for_current_stream;
		}

		// if there is more data to be read, increment the current stream
		if (remaining_readable_count != 0)
		{
			updateCurrentStream(mCurrentStream.get() + 1);

			// make sure we haven't somehow reached the end before we expected
			if (mCurrentStream.get() == mStreamList.end())
			{
				throw tc::io::IOException(kClassName+"read()", "More data was expected to be readable but end of stream list was reached.");
			}

			// correct the position the 0x0 if not already
			if (mCurrentStream.get()->stream->position() != 0x0)
			{
				if (!mCanSeek)
				{
					throw tc::io::IOException(kClassName+"read()", "Tried to continue reading from the next stream but the position was not 0, and seek was not supported.");
				}
				
				mCurrentStream.get()->stream->seek(0, tc::io::SeekOrigin::Begin);
			}
		}
	} while (remaining_readable_count > 0);
	
	return readable_count;
}

size_t tc::io::ConcatenatedStream::write(const byte_t* ptr, size_t count)
{
	if (isStreamDisposed())
	{
		throw tc::ObjectDisposedException(kClassName+"write()", "Stream was disposed.");
	}
	if (mCanWrite == false)
	{
		throw tc::NotSupportedException(kClassName+"write()", "Stream does not support writing.");
	}

	// write
	size_t writable_count = IOUtil::getWritableCount(mStreamLength, position(), count);
	size_t remaining_writable_count = writable_count;
	do {
		// determine expected writable data count for the current stream
		size_t writable_count_for_current_stream = IOUtil::getWritableCount(mCurrentStream.get()->range.length, mCurrentStream.get()->stream->position(), remaining_writable_count);

		if (writable_count_for_current_stream != 0)
		{
			// write data and throw exception if unexpected write count is returned
			if (writable_count_for_current_stream != mCurrentStream.get()->stream->write(ptr + (writable_count - remaining_writable_count), writable_count_for_current_stream))
			{
				throw tc::io::IOException(kClassName+"write()", "Writing from one of the base streams returned less data than expected.");
			}

			// decrement the remaining writable count
			remaining_writable_count -= writable_count_for_current_stream;
		}

		// if there is more data to be write, increment the current stream
		if (remaining_writable_count != 0)
		{
			updateCurrentStream(mCurrentStream.get() + 1);

			// make sure we haven't somehow reached the end before we expected
			if (mCurrentStream.get() == mStreamList.end())
			{
				throw tc::io::IOException(kClassName+"write()", "More data was expected to be writable but end of stream list was reached.");
			}

			// correct the position the 0x0 if not already
			if (mCurrentStream.get()->stream->position() != 0x0)
			{
				if (!mCanSeek)
				{
					throw tc::io::IOException(kClassName+"write()", "Tried to continue writing from the next stream but the position was not 0, and seek was not supported.");
				}
				
				mCurrentStream.get()->stream->seek(0, tc::io::SeekOrigin::Begin);
			}
		}
	} while (remaining_writable_count > 0);
	
	return writable_count;
}

int64_t tc::io::ConcatenatedStream::seek(int64_t offset, SeekOrigin origin)
{
	if (isStreamDisposed())
	{
		throw tc::ObjectDisposedException(kClassName+"seek()", "Stream was disposed.");
	}
	if (mCanSeek == false)
	{
		throw tc::NotSupportedException(kClassName+"seek()", "Stream does not support seeking.");
	}

	// seek
	int64_t absolute_seek_pos = StreamUtil::getSeekResult(offset, origin, position(), mStreamLength);

	// seek is <= 0 : we use the first stream and set the position to 0
	if (absolute_seek_pos <= 0)
	{
		updateCurrentStream(mStreamList.begin());
		if (mCurrentStream.get() == mStreamList.end())
		{
			throw tc::io::IOException(kClassName+"seek()", "Failed to seek because underlying stream could not be determined.");	
		}
		mCurrentStream.get()->stream->seek(0, tc::io::SeekOrigin::Begin);
	}
	// seek is < mStreamLength : we find the stream in the map and set the relative position 
	else if (absolute_seek_pos < mStreamLength)
	{
		// before we do a map lookup, check if the current stream has the range the offset sits in
		if ((absolute_seek_pos >= mCurrentStream.get()->range.offset) && (absolute_seek_pos < (mCurrentStream.get()->range.offset + mCurrentStream.get()->range.length)))
		{
			mCurrentStream.get()->stream->seek(absolute_seek_pos - mCurrentStream.get()->range.offset, tc::io::SeekOrigin::Begin);
		}
		// look up the correct stream in the map
		else
		{
			auto rangeItr = mStreamListMap.find(StreamRange(absolute_seek_pos));
			if (rangeItr == mStreamListMap.end())
			{
				throw tc::io::IOException(kClassName+"seek()", "Failed to seek because underlying stream could not be determined.");
			}

			if (rangeItr->second > mStreamList.size())
			{
				throw tc::io::IOException(kClassName+"seek()", "Failed to seek because underlying stream could not be determined.");
			}

			updateCurrentStream(mStreamList.begin() + rangeItr->second);
			mCurrentStream.get()->stream->seek(absolute_seek_pos - mCurrentStream.get()->range.offset, tc::io::SeekOrigin::Begin);
		}
	}
	// seek is >= mStreamLength : we use the end stream and seek to the end of it
	else
	{
		updateCurrentStream(--mStreamList.end());
		if (mCurrentStream.get() == mStreamList.end())
		{
			throw tc::io::IOException(kClassName+"seek()", "Failed to seek because underlying stream could not be determined.");	
		}
		mCurrentStream.get()->stream->seek(0, tc::io::SeekOrigin::End);
	}

	return position();
}

void tc::io::ConcatenatedStream::setLength(int64_t length)
{
	if (isStreamDisposed())
	{
		throw tc::ObjectDisposedException(kClassName+"setLength()", "Stream was disposed.");
	}

	throw tc::NotImplementedException(kClassName+"setLength()", "setLength() is not implemented for tc::io::ConcatenatedStream.");
}

void tc::io::ConcatenatedStream::flush()
{
	if (isStreamDisposed())
	{
		throw tc::ObjectDisposedException(kClassName+"flush()", "Stream was disposed.");
	}

	mCurrentStream.get()->stream->flush();
}

void tc::io::ConcatenatedStream::dispose()
{
	if (isStreamDisposed() == false)
		mCurrentStream.get()->stream->flush();

	mStreamList.clear();
	mCurrentStream.makeNull();

	mCanRead = false;
	mCanWrite = false;
	mCanSeek = false;
	mStreamLength = 0;
}

void tc::io::ConcatenatedStream::updateCurrentStream(std::vector<StreamInfo>::iterator stream_itr)
{
	if (mCurrentStream.isNull())
	{
		mCurrentStream = stream_itr;
	}
	else if (mCurrentStream.get() != stream_itr)
	{
		// if stream itr != end() flush the stream
		if (mCurrentStream.get() != mStreamList.end())
			mCurrentStream.get()->stream->flush();
		mCurrentStream = stream_itr;
	}
}