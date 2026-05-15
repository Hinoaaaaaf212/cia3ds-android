#pragma once
#include <tc/io/ISource.h>

class SourceTestUtil
{
public:
	static void testSourceLength(tc::io::ISource& source, int64_t expected_len);
	static void pullTestHelper(tc::io::ISource& source, int64_t offset, size_t len, size_t expected_len, const byte_t* expected_data);
};