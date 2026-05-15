#pragma once
#include "ITestClass.h"

#include <tc/io/MemoryStream.h>

class io_MemoryStream_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void testCreateEmptyStream_DefaultConstructor();
	void testCreateEmptyStream_SizedConstructor();
	void testCreatePopulatedStream();
	void testInitializeByCopyWithByteData();
	void testInitializeByMoveWithByteData();
	void testInitializeByCopyWithMemoryPointer();
	void testSeekBeginToZero();
	void testSeekBeginToMiddle();
	void testSeekBeginToEnd();
	void testSeekBeginPastEnd();
	void testSeekBeginNegative();
	void testSeekCurrentByZero();
	void testSeekCurrentToMiddle();
	void testSeekCurrentToEnd();
	void testSeekCurrentPastEnd();
	void testSeekCurrentNegative();
	void testSeekEndByZero();
	void testSeekEndPastEnd();
	void testSeekEndNegative();
	void testSeekEndTooNegative();
	void testReadAllDataAvailable();
	void testReadRequestsSubsetOfAvailableData();
	void testReadSomeDataAvailable();
	void testReadNoDataAvailable();
	void testWriteAllDataWritable();
	void testWriteToSomeOfDataAvailable();
	void testWriteSomeDataWritable();
	void testWriteNoDataWritable();
	void testWriteReadDataPersistence();
	void testResizeStreamLarger();
	void testResizeStreamSmaller();
	void testDispose();
};
