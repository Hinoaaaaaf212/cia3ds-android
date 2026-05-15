#include <tc/io/StreamSink.h>

const std::string tc::io::StreamSink::kClassName = "tc::io::StreamSink";

tc::io::StreamSink::StreamSink() :
	mBaseStream(nullptr)
{
}

tc::io::StreamSink::StreamSink(const std::shared_ptr<tc::io::IStream>& stream) :
	mBaseStream(stream)
{
	if (mBaseStream == nullptr)
	{
		throw tc::ArgumentNullException(kClassName, "The base stream is null.");
	}

	if (mBaseStream->canWrite() == false)
	{
		throw tc::NotSupportedException(kClassName, "The base stream does not support writing.");
	}

	if (mBaseStream->canSeek() == false)
	{
		throw tc::NotSupportedException(kClassName, "The base stream does not support seeking.");
	}
}

int64_t tc::io::StreamSink::length()
{
	return mBaseStream == nullptr ? 0 :mBaseStream->length();
}

void tc::io::StreamSink::setLength(int64_t length)
{
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::setLength()", "The base stream was not initialized.");
	}

	mBaseStream->setLength(length);
}

size_t tc::io::StreamSink::pushData(const tc::ByteData& data, int64_t offset)
{
	if (mBaseStream == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::pushData()", "The base stream was not initialized.");
	}

	mBaseStream->seek(offset, tc::io::SeekOrigin::Begin);

	return mBaseStream->write(data.data(), data.size());
}