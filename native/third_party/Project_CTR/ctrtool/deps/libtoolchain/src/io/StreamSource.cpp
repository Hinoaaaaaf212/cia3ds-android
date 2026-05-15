#include <tc/io/StreamSource.h>
#include <tc/io/IOUtil.h>

const std::string tc::io::StreamSource::kClassName = "tc::io::StreamSource";

tc::io::StreamSource::StreamSource() :
	mBaseStream(nullptr)
{
}

tc::io::StreamSource::StreamSource(const std::shared_ptr<tc::io::IStream>& stream) :
	mBaseStream(stream)
{
	if (mBaseStream == nullptr)
	{
		throw tc::ArgumentNullException(kClassName, "The base stream is null.");
	}

	if (mBaseStream->canRead() == false)
	{
		throw tc::NotSupportedException(kClassName, "The base stream does not support reading.");
	}

	if (mBaseStream->canSeek() == false)
	{
		throw tc::NotSupportedException(kClassName, "The base stream does not support seeking.");
	}
}

int64_t tc::io::StreamSource::length()
{
	return mBaseStream == nullptr ? 0 : mBaseStream->length();
}

tc::ByteData tc::io::StreamSource::pullData(int64_t offset, size_t count)
{
	// get readable count
	size_t read_count = IOUtil::getReadableCount(this->length(), offset, count);

	// return if nothing is to be read
	if (read_count == 0)
	{
		return tc::ByteData();
	}
	
	// allocate ByteData
	ByteData data(read_count);

	// read from stream (note this will not be called if mBaseStream is null, as in that case read_count == 0, and this code won't be reached)
	mBaseStream->seek(offset, tc::io::SeekOrigin::Begin);
	mBaseStream->read(data.data(), data.size());

	// return populated ByteData
	return data;
}