#include <tc/io/SubSink.h>
#include <tc/io/IOUtil.h>

const std::string tc::io::SubSink::kClassName = "tc::io::SubSink";

tc::io::SubSink::SubSink() :
	mBaseSink(nullptr),
	mBaseSinkOffset(0),
	mSubSinkLength(0)
{

}

tc::io::SubSink::SubSink(const std::shared_ptr<tc::io::ISink>& sink, int64_t offset, int64_t length) :
	SubSink()
{
	mBaseSink = sink;

	// validate arguments
	if (mBaseSink == nullptr)
	{
		throw tc::ArgumentNullException(kClassName, "The base sink is null.");
	}

	if (offset < 0)
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "offset is negative");
	}
	if (length < 0)
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "length is negative");
	}

	int64_t base_length = mBaseSink->length();

	// validate arguments against sink length
	// sub sink length should not be greater than the base sink length
	if (length > base_length)
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "The sub sink length is greater than base sink length.");
	}
	// Base length - length is the maximum possible offset for the sub sink
	if (offset > (base_length - length))
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "The sub sink offset is greater than the maximum possible offset given the base sink length and sub sink length.");
	}
	
	// set class state
	mBaseSinkOffset = offset;
	mSubSinkLength = length;
}

int64_t tc::io::SubSink::length()
{
	return mBaseSink == nullptr ? 0 : mSubSinkLength;
}

void tc::io::SubSink::setLength(int64_t length)
{
	throw tc::NotImplementedException(kClassName+"::setLength()", "setLength is not implemented for SubSink.");
}

size_t tc::io::SubSink::pushData(const tc::ByteData& data, int64_t offset)
{
	if (mBaseSink == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::pushData()", "Failed to push data (no base sink)");
	}

	auto new_data = tc::ByteData(data.data(), IOUtil::getWritableCount(mSubSinkLength, offset, data.size()));

	return mBaseSink->pushData(new_data, mBaseSinkOffset + offset);
}