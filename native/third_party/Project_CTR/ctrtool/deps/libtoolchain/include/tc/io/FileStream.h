	/**
	 * @file FileStream.h
	 * @brief Declaration of tc::io::FileStream
	 * @author Jack (jakcron)
	 * @version	0.4
	 * @date 2020/01/23
	 **/
#pragma once
#include <tc/io/IStream.h>
#include <tc/io/Path.h>
#include <tc/io/FileMode.h>
#include <tc/io/FileAccess.h>

#include <tc/AccessViolationException.h>
#include <tc/ArgumentNullException.h>
#include <tc/ArgumentOutOfRangeException.h>
#include <tc/NotSupportedException.h>
#include <tc/NotImplementedException.h>
#include <tc/ObjectDisposedException.h>
#include <tc/OverflowException.h>
#include <tc/UnauthorisedAccessException.h>
#include <tc/SecurityException.h>
#include <tc/io/IOException.h>
#include <tc/io/FileExistsException.h>
#include <tc/io/FileNotFoundException.h>
#include <tc/io/PathTooLongException.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <cstdio>
#endif

namespace tc { namespace io {

	/**
	 * @class FileStream
	 * @brief An implementation of IStream as a wrapper to local OS file access functions.
	 **/
class FileStream : public tc::io::IStream
{
public:
		/** 
		 * @brief Default constuctor
		 **/
	FileStream();

		/**
		 * @brief Move constructor
		 **/
	FileStream(FileStream&& other);

		/** 
		 * @brief Open file stream
		 * 
		 * @param[in] path A relative or absolute path for the file that the current FileStream object will encapsulate.
		 * @param[in] mode One of the enumeration values that determines how to open or create the file.
		 * @param[in] access One of the enumeration values that determines how the file can be accessed by the FileStream object. This also determines the values returned by the @ref canRead and @ref canWrite methods of the FileStream object. @ref canSeek is true if path specifies a disk file.
		 *
		 * @throw tc::ArgumentException @p path contains invalid characters or is empty.
		 * @throw tc::NotSupportedException @p path refers to an unsupported non-file device.
		 * @throw tc::io::IOException An I/O error, such as specifying @p mode @a CreateNew when the file specified by @p path already exists, occurred. Or the stream has been closed.
		 * @throw tc::SecurityException The caller does not have the required permission.
		 * @throw tc::io::DirectoryNotFoundException The specified path is invalid, such as being on an unmapped drive.
		 * @throw tc::UnauthorisedAccessException The @p access requested is not permitted by the operating system for the specified @p path, such as when @p access is @a Write or @a ReadWrite and the file or directory is set for read-only access.
		 * @throw tc::io::PathTooLongException The specified @p path, file name, or both exceed the system-defined maximum length.
		 * @throw tc::ArgumentOutOfRangeException @p mode contains an invalid value.
		 * @throw tc::ArgumentOutOfRangeException @p access contains an invalid value.
		 **/
	FileStream(const tc::io::Path& path, FileMode mode, FileAccess access);

		/**
		 * @brief Move assignment
		 **/
	FileStream& operator=(FileStream&& other);

		/**
		 * @brief Indicates whether the current stream supports reading.
		 **/ 
	bool canRead() const;

		/**
		 * @brief Indicates whether the current stream supports writing.
		 **/
	bool canWrite() const;

		/**
		 * @brief Indicates whether the current stream supports seeking.
		 **/
	bool canSeek() const;

		/**
		 * @brief Gets the length in bytes of the stream.
		 **/
	int64_t length();

		/** 
		 * @brief Gets the position within the current stream. 
		 * 
		 * @return This is returns the result of seek(0, SeekOrigin::Current);
		 * 
		 * @throw tc::NotSupportedException The stream does not support seeking.
		 **/
	int64_t position();

		/**
		 * @brief Reads a sequence of bytes from the current stream and advances the position within the stream by the number of bytes read.
		 * 
		 * @param[out] ptr Pointer to an array of bytes. When this method returns, @p ptr contains the specified byte array with the values between 0 and (@p count - 1) replaced by the bytes read from the current source.
		 * @param[in] count The maximum number of bytes to be read from the current stream.
		 * 
		 * @return The total number of bytes read into @p ptr. This can be less than the number of bytes requested if that many bytes are not currently available, or zero (0) if the end of the stream has been reached.
		 * 
		 * @pre A stream must support reading for @ref read to work. 
		 * @note Use @ref canRead to determine if this stream supports reading.
		 * 
		 * @throw tc::AccessViolation @p ptr refers to inaccessible/protected memory.
		 * @throw tc::ArgumentNullException @p ptr is @a nullptr.
		 * @throw tc::ArgumentOutOfRangeException @p count is negative.
		 * @throw tc::io::IOException An I/O error occurred.
		 * @throw tc::NotSupportedException The stream does not support reading.
		 * @throw tc::ObjectDisposedException Methods were called after the stream was closed.
		 **/
	size_t read(byte_t* ptr, size_t count);

		/**
		 * @brief Writes a sequence of bytes to the current stream and advances the current position within this stream by the number of bytes written.
		 * 
		 * @param[in] ptr Pointer to an array of bytes. This method copies @p count bytes from @p ptr to the current stream.
		 * @param[in] count The number of bytes to be written to the current stream.
		 * 
		 * @return The total number of bytes written to the stream. This can be less than the number of bytes requested if that many bytes are not currently available, or zero (0) if the end of the stream has been reached.
		 * 
		 * @pre A stream must support writing for @ref write to work. 
		 * @note Use @ref canWrite to determine if this stream supports writing.
		 * 
		 * @throw tc::AccessViolation @p ptr refers to inaccessible/protected memory.
		 * @throw tc::ArgumentNullException @p ptr is @a nullptr.
		 * @throw tc::ArgumentOutOfRangeException @p count is negative.
		 * @throw tc::io::IOException An I/O error occurred.
		 * @throw tc::NotSupportedException The stream does not support writing.
		 * @throw tc::ObjectDisposedException Methods were called after the stream was closed.
		 **/
	size_t write(const byte_t* ptr, size_t count);

		/**
		 * @brief Sets the position within the current stream.
		 * 
		 * @param[in] offset A byte offset relative to the origin parameter.
		 * @param[in] origin A value of type @ref tc::io::SeekOrigin indicating the reference point used to obtain the new position.
		 * 
		 * @return The new position within the current stream.
		 * 
		 * @pre A stream must support seeking for @ref seek to work. 
		 * @note Use @ref canSeek to determine if this stream supports seeking.
		 * 
		 * @throw tc::ArgumentOutOfRangeException @p offset or @p origin contains an invalid value.
		 * @throw tc::io::IOException An I/O error occurs.
		 * @throw tc::NotSupportedException The stream does not support seeking, such as if the stream is constructed from a pipe or console output.
		 * @throw tc::ObjectDisposedException Methods were called after the stream was closed.
		 * @throw tc::OverflowException The new stream position could not be represented in int64_t without overflowing.
		 **/
	int64_t seek(int64_t offset, SeekOrigin origin);


		/**
		 * @brief Sets the length of the current stream. THIS IS NOT IMPLEMENTED FOR @ref FileStream.
		 * 
		 * @param[in] length The desired length of the current stream in bytes.
		 * 
		 * @pre A stream must support both writing and seeking for @ref setLength to work. 
		 * @note Use @ref canWrite to determine if this stream supports writing.
		 * @note Use @ref canSeek to determine if this stream supports seeking.
		 * 
		 * @throw tc::NotSupportedException The stream does not support both writing and seeking, such as if the stream is constructed from a pipe or console output.
		 * @throw tc::ObjectDisposedException Methods were called after the stream was closed.
		 **/
	void setLength(int64_t length);

		/**
		 * @brief Clears all buffers for this stream and causes any buffered data to be written to the underlying device.
		 * 
		 * @throw tc::io::IOException An I/O error occurs.
		 * @throw tc::ObjectDisposedException Methods were called after the stream was closed.
		 **/
	void flush();

	void dispose();
private:
	static const std::string kClassName;

	// delete copy constructor
	FileStream(const FileStream&);

	// delete copy assignment
	FileStream& operator=(const FileStream&);

	struct FileHandle
	{
#ifdef _WIN32
		HANDLE handle;
		FileHandle(HANDLE h) : handle(h) {}
#else
		int handle;
		FileHandle(int h) : handle(h) {}
#endif
		FileHandle() : handle(0) {}
		FileHandle(FileHandle&& other) {handle = other.handle; other.handle = 0;}
		~FileHandle();
	private:
		FileHandle(const FileHandle& other);
	};


	bool mCanRead;
	bool mCanWrite;
	bool mCanSeek;
	bool mIsAppendRestrictSeekCall;
	std::unique_ptr<tc::io::FileStream::FileHandle> mFileHandle;

#ifdef _WIN32
	void open_impl(const tc::io::Path& path, FileMode mode, FileAccess access);
	int64_t length_impl();
	size_t read_impl(byte_t* ptr, size_t count);
	size_t write_impl(const byte_t* ptr, size_t count);
	int64_t seek_impl(int64_t offset, SeekOrigin origin);
	void setLength_impl(int64_t length);
	void flush_impl();
#else
	void open_impl(const tc::io::Path& path, FileMode mode, FileAccess access);
	int64_t length_impl();
	size_t read_impl(byte_t* ptr, size_t count);
	size_t write_impl(const byte_t* ptr, size_t count);
	int64_t seek_impl(int64_t offset, SeekOrigin origin);
	void setLength_impl(int64_t length);
	void flush_impl();
#endif
};

}} // namespace tc::io