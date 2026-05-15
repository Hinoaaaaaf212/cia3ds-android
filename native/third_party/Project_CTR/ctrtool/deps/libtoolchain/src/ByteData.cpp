#include <tc/ByteData.h>

const std::string tc::ByteData::kClassName = "tc::ByteData";

tc::ByteData::ByteData() :
	ByteData(0)
{}

tc::ByteData::ByteData(const tc::ByteData& other) :
	ByteData(other.mPtr.get(), other.mSize)
{}

tc::ByteData::ByteData(tc::ByteData&& other) :
	mSize(other.mSize),
	mPtr(std::move(other.mPtr))
{
	other.mSize = 0;
}

tc::ByteData::ByteData(std::initializer_list<byte_t> l) :
	ByteData(l.size(), false)
{
	size_t i = 0;
	for (auto itr = l.begin(); itr != l.end(); itr++, i++)
	{
		mPtr.get()[i] = *itr;
	}
}

tc::ByteData::ByteData(size_t size, bool clear_memory)
{
	if (size == 0)
	{
		mPtr.reset();
	}
	else 
	{
		try
		{
			mPtr = std::unique_ptr<byte_t>(new byte_t[size]);
		} 
		catch (const std::bad_alloc&) 
		{
			throw tc::OutOfMemoryException(kClassName, "std::bad_alloc thrown");
		}
		
		if (mPtr == nullptr)
		{
			throw tc::OutOfMemoryException(kClassName, "Failed to allocate memory");
		}
	}
	
	mSize = size;

	if (clear_memory == true)
	{
		memset(mPtr.get(), 0, mSize);
	}
}

tc::ByteData::ByteData(const byte_t* data, size_t size) :
	ByteData(size, false)
{
	memcpy(mPtr.get(), data, mSize);
}

tc::ByteData& tc::ByteData::operator=(const ByteData& other)
{
	*this = tc::ByteData(other);
	return *this;
}

tc::ByteData& tc::ByteData::operator=(ByteData&& other)
{
	this->mPtr = std::move(other.mPtr);
	this->mSize = other.mSize;
	other.mSize = 0;
	
	return *this;
}

byte_t& tc::ByteData::operator[](size_t index)
{
	return mPtr.get()[index];
}

byte_t tc::ByteData::operator[](size_t index) const
{
	return mPtr.get()[index];
}

bool tc::ByteData::operator==(const ByteData& other) const
{
	return (this->mSize == other.mSize && memcmp(this->mPtr.get(), other.mPtr.get(), this->mSize) == 0);
}

bool tc::ByteData::operator!=(const ByteData& other) const
{
	return !(*this == other);
}

byte_t* tc::ByteData::data() const
{
	return mPtr.get();
}

size_t tc::ByteData::size() const
{
	return mPtr == nullptr ? 0 : mSize;
}