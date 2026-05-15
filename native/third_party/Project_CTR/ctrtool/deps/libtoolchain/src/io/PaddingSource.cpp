#include <tc/io/PaddingSource.h>
#include <tc/io/IOUtil.h>

const std::string tc::io::PaddingSource::kClassName = "tc::io::PaddingSource";

tc::io::PaddingSource::PaddingSource() :
	mSourceLength(0),
	mPaddingByte(0)
{
}

tc::io::PaddingSource::PaddingSource(byte_t padding_byte, int64_t size) :
	mSourceLength(size),
	mPaddingByte(padding_byte)
{
	if (size < 0)
	{
		throw tc::ArgumentOutOfRangeException(kClassName, "length is negative");
	}
}

int64_t tc::io::PaddingSource::length()
{
	return mSourceLength;
}

tc::ByteData tc::io::PaddingSource::pullData(int64_t offset, size_t count)
{
	tc::ByteData data(IOUtil::getReadableCount(mSourceLength, offset, count));

	memset(data.data(), mPaddingByte, data.size());
	
	return data;
}