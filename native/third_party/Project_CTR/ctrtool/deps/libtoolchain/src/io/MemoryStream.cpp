#include <tc/io/MemoryStream.h>

#include <limits>
#include <tc/io/StreamUtil.h>
#include <tc/io/IOUtil.h>

const std::string tc::io::MemoryStream::kClassName = "tc::io::MemoryStream";

tc::io::MemoryStream::MemoryStream() :
	MemoryStream(0)
{}

tc::io::MemoryStream::MemoryStream(size_t length) :
	mData(),
	mPosition(0)
{
	setLength(length);
}

tc::io::MemoryStream::MemoryStream(const tc::ByteData& byte_data) :
	mData(byte_data),
	mPosition(0)
{

}

tc::io::MemoryStream::MemoryStream(tc::ByteData&& byte_data) :
	mData(std::move(byte_data)),
	mPosition(0)
{

}

tc::io::MemoryStream::MemoryStream(const byte_t* data, size_t len) :
	mData(data, len),
	mPosition(0)
{
}

bool tc::io::MemoryStream::canRead() const 
{
	return true;
}

bool tc::io::MemoryStream::canWrite() const 
{
	return true;
}
	
bool tc::io::MemoryStream::canSeek() const 
{
	return true;
}

int64_t tc::io::MemoryStream::length() 
{
	return mData.size();
}

int64_t tc::io::MemoryStream::position() 
{
	return mPosition;
}

size_t tc::io::MemoryStream::read(byte_t* ptr, size_t count) 
{
	if (ptr == nullptr)
	{
		throw tc::ArgumentNullException(kClassName+"::read()", "ptr is null.");
	}

	count = IOUtil::getReadableCount(IOUtil::castSizeToInt64(mData.size()), mPosition, count);

	memcpy(ptr, mData.data() + mPosition, count);

	mPosition += IOUtil::castSizeToInt64(count);

	return count;
}

size_t tc::io::MemoryStream::write(const byte_t* ptr, size_t count) 
{
	if (ptr == nullptr)
	{
		throw tc::ArgumentNullException(kClassName+"::write()", "ptr is null.");
	}

	// check if the position is past the end of stream, enlarge stream in this case
	if ((IOUtil::castInt64ToSize(mPosition) + count) > mData.size())
	{
		setLength(mPosition + IOUtil::castSizeToInt64(count));
	}

	count = IOUtil::getWritableCount(IOUtil::castSizeToInt64(mData.size()), mPosition, count);

	memcpy(mData.data() + mPosition, ptr, count);

	mPosition += IOUtil::castSizeToInt64(count);

	return count;
}

int64_t tc::io::MemoryStream::seek(int64_t offset, SeekOrigin origin) 
{
	int64_t new_pos = StreamUtil::getSeekResult(offset, origin, mPosition, (int64_t)mData.size());

	if (new_pos < 0)
	{
		throw tc::ArgumentOutOfRangeException(kClassName+"::seek()", "New position is negative.");
	}

	mPosition = new_pos;

	return mPosition;
}

void tc::io::MemoryStream::setLength(int64_t length) 
{
	// check length isn't too large (int64_t could be larger than size_t)
	if (IOUtil::castInt64ToSize(length) > std::numeric_limits<size_t>::max())
	{
		throw tc::ArgumentOutOfRangeException(kClassName+"::setLength()", "Length greater than maxium possible length for MemoryStream");
	}

	// check length isn't negative
	if (length < 0)
	{
		throw tc::ArgumentOutOfRangeException(kClassName+"::setLength()", "Length is negative.");
	}

	// create new ByteData
	ByteData data(IOUtil::castInt64ToSize(length));

	// determine copy length (between old and new ByteData)
	size_t copy_len = std::min<size_t>(data.size(), mData.size());

	// copy from old to new ByteData
	memcpy(data.data(), mData.data(), copy_len);

	// re-assign mData (this frees the old mData)
	mData = data;

	// reduce position if shrunk
	mPosition = std::min<int64_t>(mPosition, int64_t(mData.size()));
}

void tc::io::MemoryStream::flush() 
{
	// do nothing
}

void tc::io::MemoryStream::dispose() 
{
	mData = ByteData();
}