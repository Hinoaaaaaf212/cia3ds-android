#include <tc/io/SubSource.h>
#include <tc/io/IOUtil.h>

const std::string tc::io::SubSource::kClassName = "tc::io::SubSource";

tc::io::SubSource::SubSource() :
	mBaseSource(nullptr),
	mBaseSourceOffset(0),
	mSubSourceLength(0)
{

}

tc::io::SubSource::SubSource(const std::shared_ptr<tc::io::ISource>& source, int64_t offset, int64_t length) :
	SubSource()
{
	mBaseSource = source;

	// validate arguments
	if (mBaseSource == nullptr)
	{
		throw tc::ArgumentNullException(kClassName, "source is null");
	}

	if (offset < 0)
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "offset is negative");
	}
	if (length < 0)
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "length is negative");
	}

	int64_t base_length = mBaseSource->length();

	// validate arguments against source length
	// sub source length should not be greater than the base source length
	if (length > base_length)
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "sub source length is greater than base source length");
	}
	// Base length - length is the maximum possible offset for the sub source
	if (offset > (base_length - length))
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "sub source offset is greater than the maximum possible offset given the base source size and sub source size");
	}
	
	// set class state
	mBaseSourceOffset = offset;
	mSubSourceLength = length;
}

int64_t tc::io::SubSource::length()
{
	return mBaseSource == nullptr ? 0 : mSubSourceLength;
}

tc::ByteData tc::io::SubSource::pullData(int64_t offset, size_t count)
{
	size_t pull_count = IOUtil::getReadableCount(length(), offset, count);

	if (pull_count == 0)
		return tc::ByteData();

	return mBaseSource->pullData(mBaseSourceOffset + offset, pull_count);
}