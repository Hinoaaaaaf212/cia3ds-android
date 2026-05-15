#pragma once
#include "ITestClass.h"

#include <tc/io/ConcatenatedStream.h>

class io_ConcatenatedStream_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	/*
	test plan:
	1) Ensure default constructor creates an object that return 0/false for property methods and throw tc::ObjectDisposedException for actions other than dispose
	2) Ensure create constructor throw exceptions for all variations of bad input (empty list, a stream is null, the combination of streams did not at least all support read or write)
	3) Ensure create constructor works with valid combinations of input (and stream properties are correct)
	4) Ensure that even valid streams, calling setLength() will throw tc::NotImplementedException
	5) For streams that have unsupported features (as indicated with false results from canSeek() & canRead() & canWrite()), make sure the related methods (seek() read() write()) throw tc::NotSupportedException when called if unsupported
	6) For streams that support seek(), seek is quite vital for the performance for ConcatenatedStream as it may do a map lookup to find the correct stream, but also has edge cases. This needs to gracefully handle unexpected errors and edge cases.
	7) For readable streams (that support seek), ensure read works for various kinds of ConcatenatedStreams (including base streams that do not are not at position==0 when they are iterated too)
	8) For readable streams (that do not support seek), ensure read works for various kinds of ConcatenatedStreams (including base streams that do not are not at position==0 when they are iterated too, this is an error state)
	9) For writable streams (that support seek), ensure read works for various kinds of ConcatenatedStreams (including base streams that do not are not at position==0 when they are iterated too)
	10) For writable streams (that do not support seek), ensure read works for various kinds of ConcatenatedStreams (including base streams that do not are not at position==0 when they are iterated too, this is an error state)
	11) Quality of life: The intended use case for ConcatenatedStream is when stored in std::shared_ptr<tc::io::IStream>, if copy/assignment/move operators/constructors aren't deleted, do they work as intended, if not fix behaviour or delete method...
	*/

	void test_DefaultConstructor(); // 1
	void test_CreateConstructor_ThrowsOnBadInput(); // 2
	void test_CreateConstructor_SetsCorrectStreamState(); // 3
	void test_setLength_ThrowsOnUse(); // 4
	void test_read_ThrowsOnUnsupported(); // 5
	void test_write_ThrowsOnUnsupported(); // 5
	void test_seek_ThrowsOnUnsupported(); // 5
	void test_seek_SeeksToBeginOnNegativeSeek(); // 6
	void test_seek_SeeksToEndOnTooLargeSeek(); // 6
	void test_seek_CanFindCorrectStreamForSeek(); // 6
	void test_read_CanReadFromSingleStream(); // 7
	void test_read_CanReadFromMultipleStreamWithSeekSupport(); // 7
	void test_read_CanReadFromMultipleStreamWithNoSeekSupport(); // 8
	void test_read_ReadFromMultiStream_NoSeekSupport_ThrowsOnSeekRequired(); // 8
	void test_write_CanWriteFromSingleStream(); // 9
	void test_write_CanWriteFromMultipleStreamWithSeekSupport(); // 9
	void test_write_CanWriteFromMultipleStreamWithNoSeekSupport(); // 10
	void test_write_WriteFromMultiStream_NoSeekSupport_ThrowsOnSeekRequired(); // 10
	void test_MoveOperator_MoveDisposedToDisposed(); // 11
	void test_MoveOperator_MoveInitializedToDisposed(); // 11
	void test_MoveOperator_MoveDisposedToInitialized(); // 11
	void test_MoveOperator_MoveInitializedToInitialized(); // 11
	void test_MoveConstructor_MoveDisposed(); // 11
	void test_MoveConstructor_MoveInitialized(); // 11
};
