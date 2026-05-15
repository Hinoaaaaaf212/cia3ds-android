#include "SourceTestUtil.h"
#include <sstream>

void SourceTestUtil::testSourceLength(tc::io::ISource& source, int64_t expected_len)
{
	std::stringstream error_ss;
	int64_t actual_len = source.length();
	if (actual_len != expected_len)
	{
		error_ss << "length() returned: " << actual_len << ", when it should have been " << expected_len << ".";
		throw tc::Exception(error_ss.str());
	}
}

void SourceTestUtil::pullTestHelper(tc::io::ISource& source, int64_t offset, size_t len, size_t expected_len, const byte_t* expected_data)
{
	std::stringstream error_ss;

	tc::ByteData data = source.pullData(offset, len);

	if (data.size() != expected_len)
	{
		error_ss << "pullData(offset: " << offset << ", len:" << len << ") returned ByteData with size(): " << data.size() << ", when it should have been " << expected_len;
		throw tc::Exception(error_ss.str());
	}

	if (expected_data != nullptr && memcmp(data.data(), expected_data, expected_len) != 0)
	{
		error_ss << "pullData(offset: " << offset << ", len:" << len << ") returned ByteData with incorrect layout";
		throw tc::Exception(error_ss.str());
	}
}