#pragma once
#include "ITestClass.h"

#include <tc/io/ISink.h>
#include <tc/io/IStream.h>

class io_StreamSink_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void testDefaultConstructor();
	void testCreateConstructor();
	void testCreateFromNullStream();
	void testCreateFromStreamWithoutSeek();
	void testCreateFromStreamWithoutRead();
	void testCreateFromStreamWithoutWrite();
	void testSetLengthOnDisposedBase();
	void testPushDataOnDisposedBase();
	void testPushDataOutsideOfBaseRange();

	void pushTestHelper(tc::io::ISink& sink, const std::shared_ptr<tc::io::IStream>& base_stream, tc::ByteData& expected_data, int64_t push_offset);
};
