#include <tc/io/FileStream.h>
#include <tc/PlatformErrorHandlingUtil.h>

#ifdef _WIN32
#include <direct.h>
#include <cstdlib>
#else
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>   /* For open(), creat() */
#endif

const std::string tc::io::FileStream::kClassName = "tc::io::FileStream";

tc::io::FileStream::FileHandle::~FileHandle()
{
#ifdef _WIN32
	CloseHandle(handle);
#else
	::close(handle);
#endif
}

tc::io::FileStream::FileStream() :
	mCanRead(false),
	mCanWrite(false),
	mCanSeek(false),
	mIsAppendRestrictSeekCall(false),
	mFileHandle()
{}

tc::io::FileStream::FileStream(FileStream&& other) :
	FileStream()
{
	*this = std::move(other);
}

tc::io::FileStream::FileStream(const tc::io::Path& path, FileMode mode, FileAccess access) :
	FileStream()
{
	// dispose stream before opening new stream
	dispose();

	open_impl(path, mode, access);
}

tc::io::FileStream& tc::io::FileStream::operator=(tc::io::FileStream&& other)
{
	mCanRead = other.mCanRead;
	mCanWrite = other.mCanWrite;
	mCanSeek = other.mCanSeek;
	mIsAppendRestrictSeekCall = other.mIsAppendRestrictSeekCall;
	mFileHandle = std::move(other.mFileHandle);
	other.dispose();

	return *this;
}

bool tc::io::FileStream::canRead() const
{
	return mFileHandle == nullptr ? false : mCanRead;
}

bool tc::io::FileStream::canWrite() const
{
	return mFileHandle == nullptr ? false : mCanWrite;
}
bool tc::io::FileStream::canSeek() const
{
	return mFileHandle == nullptr || mIsAppendRestrictSeekCall == true ? false : mCanSeek;
}

int64_t tc::io::FileStream::length()
{
	return mFileHandle == nullptr ? 0 : length_impl();
}

int64_t tc::io::FileStream::position()
{
	if (mFileHandle == nullptr)
	{
		return 0;
	}

	if (mCanSeek == false)
	{
		throw tc::NotSupportedException(kClassName+"::position()", "This method is not supported for streams that do not support seeking");
	}

	return seek_impl(0, SeekOrigin::Current);
}

size_t tc::io::FileStream::read(byte_t* ptr, size_t count)
{
	if (mFileHandle == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::read()", "Failed to read from stream (stream is disposed)");
	}

	if (mCanRead == false)
	{
		throw tc::NotSupportedException(kClassName+"::read()", "Stream does not support reading");
	}

	if (ptr == nullptr)
	{
		throw tc::ArgumentNullException(kClassName+"::read()", "ptr was null");
	}

	if (count < 0)
	{
		throw tc::ArgumentOutOfRangeException(kClassName+"::read()", "count was negative");
	}

	return read_impl(ptr, count);
}

size_t tc::io::FileStream::write(const byte_t* ptr, size_t count)
{
	if (mFileHandle == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::write()", "Failed to write to stream (no file open)");
	}

	if (mCanWrite == false)
	{
		throw tc::NotSupportedException(kClassName+"::write()", "Stream does not support writing");
	}

	if (ptr == nullptr)
	{
		throw tc::ArgumentNullException(kClassName+"::write()", "ptr was null");
	}

	if (count < 0)
	{
		throw tc::ArgumentOutOfRangeException(kClassName+"::write()", "count was negative");
	}

	return write_impl(ptr, count);
}

int64_t tc::io::FileStream::seek(int64_t offset, SeekOrigin origin)
{
	if (mFileHandle == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::seek()", "Failed to set stream position (stream is disposed)");
	}

	if (mCanSeek == false)
	{
		throw tc::NotSupportedException(kClassName+"::seek()", "Stream does not support seeking");
	}

	if (mIsAppendRestrictSeekCall == true)
	{
		throw tc::io::IOException(kClassName+"::seek()", "Streams opened in Append mode are not allowed to change file position.");
	}

	return seek_impl(offset, origin);
}

void tc::io::FileStream::setLength(int64_t length)
{
	if (mFileHandle == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::setLength()", "Failed to set stream length (stream is disposed)");
	}

	if (mCanWrite == false || mCanSeek == false)
	{
		throw tc::NotSupportedException(kClassName+"::setLength()", "Stream does not support both writing and seeking");
	}

	setLength_impl(length);
}

void tc::io::FileStream::flush()
{
	if (mFileHandle != nullptr)
	{
		flush_impl();
	}
}

void tc::io::FileStream::dispose()
{
	if (mFileHandle.get() != nullptr)
	{
		flush();
		mFileHandle.reset();
	}
	mCanRead = false;
	mCanWrite = false;
	mCanSeek = false;
}

#ifdef _WIN32

#pragma warning(disable : 4065) // disable warning for switch case with only default case

void tc::io::FileStream::open_impl(const tc::io::Path& path, FileMode mode, FileAccess access)
{
	// convert Path to unicode string
	std::u16string unicode_path = path.to_u16string(tc::io::Path::Format::Win32);

	DWORD access_flag = 0;
	DWORD share_mode_flag = 0;
	DWORD creation_flag = 0;

	// process mode
	switch (mode) 
	{
		case (FileMode::CreateNew):
			// create file if does not exist | return error if file does not exist
			creation_flag = CREATE_NEW;
			break;
		case (FileMode::Create):
			// create file if does not exist | truncate file if it exists
			creation_flag = CREATE_ALWAYS;
			break;
		case (FileMode::Open):
			// no flags
			creation_flag = OPEN_EXISTING;
			break;
		case (FileMode::OpenOrCreate):
			// create file if does not exist 
			creation_flag = access == FileAccess::Read ? OPEN_EXISTING : OPEN_ALWAYS;
			break;
		case (FileMode::Truncate):
			// truncate file if file exists
			creation_flag = TRUNCATE_EXISTING;
			break;
		case (FileMode::Append):
			// open in append mode
			creation_flag = OPEN_ALWAYS;
			break;
		default:
			throw tc::ArgumentOutOfRangeException(kClassName+"::open()", "Illegal value for mode");
	}

	// process access
	switch (access)
	{
		case (FileAccess::Read):
			// read access
			access_flag = GENERIC_READ;
			// shared read lock
			share_mode_flag = FILE_SHARE_READ;
			break;
		case (FileAccess::Write):
			// write access
			access_flag = GENERIC_WRITE;
			// exclusive lock
			share_mode_flag = 0;
			break;
		case (FileAccess::ReadWrite):
			// read/write access
			access_flag = GENERIC_READ | GENERIC_WRITE;
			// exclusive lock
			share_mode_flag = 0;
			break;
		default:
			throw tc::ArgumentOutOfRangeException(kClassName+"::open()", "Illegal value for access");
	}
	
	// validate use of write dependent flags (open existing is the only one that supports no write flag)
	if (creation_flag != OPEN_EXISTING && !(access_flag & GENERIC_WRITE))
	{
		throw tc::ArgumentException(kClassName + "::open()", "Stream open mode requires write access, but write access was not allowed");
	}

	// append can only open in write only mode
	if (mode == tc::io::FileMode::Append && (access_flag & GENERIC_READ | GENERIC_WRITE) != GENERIC_WRITE)
	{
		throw tc::ArgumentException(kClassName + "::open()", "Stream opened in Append mode can only work with Write access. ReadWrite is not permitted");
	}

	// open file
	HANDLE file_handle = CreateFileW((LPCWSTR)unicode_path.c_str(),
							  access_flag,
							  share_mode_flag,
							  0,
							  creation_flag,
							  FILE_ATTRIBUTE_NORMAL,
							  NULL);
		
	// check file handle
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		DWORD error = GetLastError();
		switch (error)
		{
			case (ERROR_FILE_NOT_FOUND):
			case (ERROR_PATH_NOT_FOUND):
				throw tc::io::FileNotFoundException(kClassName+"::open()", PlatformErrorHandlingUtil::GetLastErrorString(error));
			case (ERROR_FILE_EXISTS):
				throw tc::io::FileExistsException(kClassName+"::open()", PlatformErrorHandlingUtil::GetLastErrorString(error));
			case (ERROR_INVALID_PARAMETER):
				throw tc::ArgumentException(kClassName + "::open()", PlatformErrorHandlingUtil::GetLastErrorString(error));
			case (ERROR_ACCESS_DENIED):
				throw tc::UnauthorisedAccessException(kClassName+"::open()", PlatformErrorHandlingUtil::GetLastErrorString(error));
			default:
				throw tc::io::IOException(kClassName+"::open()", "Failed to open file stream (" + PlatformErrorHandlingUtil::GetLastErrorString(error) + ")");
		}
	}

	// store file handle
	mFileHandle = std::unique_ptr<tc::io::FileStream::FileHandle>(new tc::io::FileStream::FileHandle(file_handle));
	
	// seek to end of file if in append mode
	if (mode == FileMode::Append)
	{
		seek_impl(0, SeekOrigin::End);
		mIsAppendRestrictSeekCall = true;
	}
		

	// set state flags
	mCanRead = (access_flag & GENERIC_READ) ? true : false;
	mCanWrite = (access_flag & GENERIC_WRITE) ? true : false;
	mCanSeek = GetFileType(mFileHandle->handle) == FILE_TYPE_DISK ? true : false;
}

int64_t tc::io::FileStream::length_impl()
{
	LARGE_INTEGER stream_length;

	if (GetFileSizeEx(mFileHandle->handle, &stream_length) == false)
	{
		DWORD error = GetLastError();
		switch (error)
		{
			// TODO: Directly handle usual errors for custom exceptions
			default:
				throw tc::io::IOException(kClassName+"::length()", "Failed to get stream length (" + PlatformErrorHandlingUtil::GetLastErrorString(error) + ")");
		}
	}
	
	return (int64_t) stream_length.QuadPart;
}

size_t tc::io::FileStream::read_impl(byte_t* ptr, size_t count)
{
	DWORD bytes_read;

	if (ReadFile(mFileHandle->handle, ptr, (DWORD)count, &bytes_read, NULL) == false)
	{
		DWORD error = GetLastError();
		switch (error)
		{
			// TODO: Directly handle usual errors for custom exceptions
			default:
				throw tc::io::IOException(kClassName+"::read()", "Failed to read from stream (" + PlatformErrorHandlingUtil::GetLastErrorString(error) + ")");
		}
	}

	return bytes_read;
}

size_t tc::io::FileStream::write_impl(const byte_t* ptr, size_t count)
{
	DWORD bytes_written;

	if (WriteFile(mFileHandle->handle, ptr, (DWORD)count, &bytes_written, NULL) == false)
	{
		DWORD error = GetLastError();
		switch (error)
		{
			// TODO: Directly handle usual errors for custom exceptions
			default:
				throw tc::io::IOException(kClassName+"::write()", "Failed to write to stream (" + PlatformErrorHandlingUtil::GetLastErrorString(error) + ")");
		}
	}

	return bytes_written;
}

int64_t tc::io::FileStream::seek_impl(int64_t offset, SeekOrigin origin)
{
	DWORD seek_flag = 0;
	switch(origin)
	{
		case (SeekOrigin::Begin):
			seek_flag = FILE_BEGIN;
			break;
		case (SeekOrigin::Current):
			seek_flag = FILE_CURRENT;
			break;
		case (SeekOrigin::End):
			seek_flag = FILE_END;
			break;
		default:
			throw tc::ArgumentOutOfRangeException(kClassName+"::seek()", "Unknown SeekOrigin value");
	}

	LARGE_INTEGER win_pos, out;
	win_pos.QuadPart = offset;
	if (SetFilePointerEx(
		mFileHandle->handle,
		win_pos,
		&out,
		seek_flag
	) == false)
	{
		DWORD error = GetLastError();
		switch (error)
		{
		case (ERROR_NEGATIVE_SEEK):
			throw tc::ArgumentOutOfRangeException(kClassName+"::seek()", PlatformErrorHandlingUtil::GetLastErrorString(error));
		default:
			throw tc::io::IOException(kClassName+"::seek()", "Failed to set stream position (" + PlatformErrorHandlingUtil::GetLastErrorString(error) + ")");
		}
	}

	return out.QuadPart;
}

void tc::io::FileStream::setLength_impl(int64_t length)
{
	seek(length, tc::io::SeekOrigin::Begin);

	if (SetEndOfFile(
		mFileHandle->handle
	) == false)
	{
		DWORD error = GetLastError();
		switch (error)
		{
		default:
			throw tc::io::IOException(kClassName+"::setLength()", "Failed to set end of file (" + PlatformErrorHandlingUtil::GetLastErrorString(error) + ")");
		}
	}
}

void tc::io::FileStream::flush_impl()
{
	if (mCanWrite)
	{
		// flush buffers only applies to written data
		FlushFileBuffers(mFileHandle->handle);
	}
}

#pragma warning(default : 4065)  // reenable warning for switch case with only default case

#else
void tc::io::FileStream::open_impl(const tc::io::Path& path, FileMode mode, FileAccess access)
{
	// convert Path to unicode string
	std::string unicode_path = path.to_string(tc::io::Path::Format::POSIX);

	// open file
	int open_flag = 0;

	// process mode
	switch (mode) 
	{
		case (FileMode::CreateNew):
			// create file if does not exist | return error if file does not exist
			open_flag |= O_CREAT | O_EXCL;
			break;
		case (FileMode::Create):
			// create file if does not exist | truncate file if it exists
			open_flag |= O_CREAT | O_TRUNC;
			break;
		case (FileMode::Open):
			// no flags
			open_flag |= 0;
			break;
		case (FileMode::OpenOrCreate):
			// create file if does not exist (however only enable create flag if write access is enabled)
			open_flag |= (access == FileAccess::ReadWrite || access == FileAccess::Write) ? O_CREAT : 0;
			break;
		case (FileMode::Truncate):
			// truncate file if file exists
			open_flag |= O_TRUNC;
			break;
		case (FileMode::Append):
			// open in append mode (create file if doesn't exist)
			open_flag |= O_APPEND | O_CREAT;
			break;
		default:
			throw tc::ArgumentOutOfRangeException(kClassName+"::open()", "Illegal value for mode");
	}

	// process access
	switch (access)
	{
		case (FileAccess::Read):
			// read access
			open_flag |= O_RDONLY;
#ifdef O_SHLOCK
			// shared lock
			open_flag |= O_SHLOCK;
#endif
			break;
		case (FileAccess::Write):
			// write access
			open_flag |= O_WRONLY;
#ifdef O_EXLOCK
			// exclusive lock
			open_flag |= O_EXLOCK;
#endif
			break;
		case (FileAccess::ReadWrite):
			// read/write access
			open_flag |= O_RDWR;
#ifdef O_EXLOCK
			// exclusive lock
			open_flag |= O_EXLOCK;
#endif
			break;
		default:
			throw tc::ArgumentOutOfRangeException(kClassName+"::open()", "Illegal value for access");
	}

	// validate use of write dependent flags
	if ((open_flag & (O_APPEND | O_TRUNC | O_CREAT)) && !(open_flag & (O_WRONLY|O_RDWR))) 
	{
		throw tc::ArgumentException(kClassName+"::open()", "Stream open mode requires write access, but write access was not allowed");
	}
	// explicitly check APPEND as being write only
	if ((open_flag & (O_APPEND)) && (open_flag & (O_RDWR))) 
	{
		throw tc::ArgumentException(kClassName+"::open()", "Stream opened in Append mode can only work with Write access. ReadWrite is not permitted");
	}

	// open file handle with Read/Write for User, Read for Group, nothing for others
	int file_handle	= ::open(unicode_path.c_str(), open_flag, S_IRUSR | S_IWUSR | S_IRGRP);

	// handle error
	if (file_handle == -1)
	{
		switch (errno) 
		{
			case (EACCES):
			case (EROFS):
				throw tc::UnauthorisedAccessException(kClassName+"::open()", PlatformErrorHandlingUtil::GetGnuErrorNumString(errno));
			case (ENAMETOOLONG):
				throw tc::io::PathTooLongException(kClassName+"::open()", PlatformErrorHandlingUtil::GetGnuErrorNumString(errno));
			case (ENOENT):
				throw tc::io::FileNotFoundException(kClassName+"::open()", PlatformErrorHandlingUtil::GetGnuErrorNumString(errno));
			case (EEXIST):
				throw tc::io::FileExistsException(kClassName+"::open()", PlatformErrorHandlingUtil::GetGnuErrorNumString(errno));
			case (EINVAL):
				throw tc::ArgumentOutOfRangeException(kClassName+"::open()", PlatformErrorHandlingUtil::GetGnuErrorNumString(errno));
			case (EFAULT):
				throw tc::AccessViolationException(kClassName+"::open()", PlatformErrorHandlingUtil::GetGnuErrorNumString(errno));
			case (EISDIR):
				throw tc::io::FileNotFoundException(kClassName+"::open()", PlatformErrorHandlingUtil::GetGnuErrorNumString(errno));
			case (EDQUOT):
			case (EFBIG):
			case (EINTR):
			case (ELOOP):
			case (EMFILE):
			case (ENFILE):
			case (ENOMEM):
			case (ENOSPC):
			case (ENXIO):
			case (EOVERFLOW):
			case (EPERM):
			case (ETXTBSY):
			case (EWOULDBLOCK):
			default:
				throw tc::io::IOException(kClassName+"::open()", "Failed to open file stream (" + PlatformErrorHandlingUtil::GetGnuErrorNumString(errno) + ")");
		}
	}

	// store file handle
	mFileHandle = std::unique_ptr<tc::io::FileStream::FileHandle>(new tc::io::FileStream::FileHandle(file_handle));

	// get stat info on file
	struct stat stat_buf;
	if (fstat(mFileHandle->handle, &stat_buf) == -1)
	{
		throw tc::io::IOException(kClassName+"::open()", "Failed to check stream properties using fstat() (" + PlatformErrorHandlingUtil::GetGnuErrorNumString(errno) + ")");
	}

	// if this is a directory throw an exception
	if (S_ISDIR(stat_buf.st_mode))
	{
		throw tc::io::FileNotFoundException(kClassName+"::open()", "Path refers to a directory not a file");
	}

	// set state flags
	// would check O_RDONLY but that resolves to 0 so it can't be bitmask checked
	mCanRead = (open_flag & O_RDWR) || !(open_flag & O_WRONLY) ? true : false;
	mCanWrite = (open_flag & (O_WRONLY|O_RDWR)) ? true : false;
	mCanSeek = S_ISREG(stat_buf.st_mode) ? true : false;

	// seek to end of file if in append mode
	if (mode == FileMode::Append)
	{
		seek_impl(0, SeekOrigin::End);
		mIsAppendRestrictSeekCall = true;
	}
}

int64_t tc::io::FileStream::length_impl()
{
	int64_t length;

	// get stat info on file
	struct stat stat_buf;
	if (fstat(mFileHandle->handle, &stat_buf) == -1)
	{
		throw tc::io::IOException(kClassName+"::length()", "Failed to check stream properties using fstat() (" + PlatformErrorHandlingUtil::GetGnuErrorNumString(errno) + ")");
	}

	if (S_ISREG(stat_buf.st_mode))
	{
		length = stat_buf.st_size;
	}
	else
	{
		throw tc::NotSupportedException(kClassName+"::length()", "length() cannot be used with device-files or pipes");
	}
	
	return length;
}

size_t tc::io::FileStream::read_impl(byte_t* ptr, size_t count)
{
	int64_t read_len = ::read(mFileHandle->handle, ptr, count);

	// handle error
	if (read_len == -1)
	{
		switch (errno) 
		{	
			case (EINVAL):
				throw tc::ArgumentOutOfRangeException(kClassName+"::read()", PlatformErrorHandlingUtil::GetGnuErrorNumString(errno));
			case (EFAULT):
				throw tc::AccessViolationException(kClassName+"::read()", PlatformErrorHandlingUtil::GetGnuErrorNumString(errno));
			case (EISDIR):
			case (EBADF):
			case (EAGAIN):
			case (EINTR):
			case (EIO):
			default:
				throw tc::io::IOException(kClassName+"::read()", "Failed to read from stream (" + PlatformErrorHandlingUtil::GetGnuErrorNumString(errno) + ")");
		}
	}

	return size_t(read_len);
}

size_t tc::io::FileStream::write_impl(const byte_t* ptr, size_t count)
{
	int64_t write_len = ::write(mFileHandle->handle, ptr, count);

	// handle error
	if (write_len == -1)
	{
		switch (errno) 
		{	
			case (EINVAL):
				throw tc::ArgumentOutOfRangeException(kClassName+"::write()", PlatformErrorHandlingUtil::GetGnuErrorNumString(errno));
			case (EFAULT):
				throw tc::AccessViolationException(kClassName+"::write()", PlatformErrorHandlingUtil::GetGnuErrorNumString(errno));
			case (EFBIG):
			case (EAGAIN):
			case (EDESTADDRREQ):
			case (EDQUOT):
			case (EINTR):
			case (EIO):
			case (ENOSPC):
			case (EPERM):
			case (EPIPE):
			default:
				throw tc::io::IOException(kClassName+"::write()", "Failed to write to stream (" + PlatformErrorHandlingUtil::GetGnuErrorNumString(errno) + ")");
		}
	}

	return size_t(write_len);
}

int64_t tc::io::FileStream::seek_impl(int64_t offset, SeekOrigin origin)
{
	int seek_flag = 0;
	switch(origin)
	{
		case (SeekOrigin::Begin):
			seek_flag = SEEK_SET;
			break;
		case (SeekOrigin::Current):
			seek_flag = SEEK_CUR;
			break;
		case (SeekOrigin::End):
			seek_flag = SEEK_END;
			break;
		default:
			throw tc::ArgumentOutOfRangeException(kClassName+"::seek()", "Unknown SeekOrigin value");
	}

#ifdef _LARGEFILE64_SOURCE
	int64_t fpos = lseek64(mFileHandle->handle, offset, seek_flag);
#else 
	int64_t fpos = lseek(mFileHandle->handle, offset, seek_flag);
#endif

	// handle error
	if (fpos == -1)
	{
		switch (errno) 
		{
			case (EINVAL):
				throw tc::ArgumentOutOfRangeException(kClassName+"::seek()",  PlatformErrorHandlingUtil::GetGnuErrorNumString(errno));
			case (EOVERFLOW):
				throw tc::OverflowException(kClassName+"::seek()",  PlatformErrorHandlingUtil::GetGnuErrorNumString(errno));
			case (EBADF):
			case (ESPIPE):
			case (ENXIO):
			default:
				throw tc::io::IOException(kClassName+"::seek()", "Failed to set stream position (" + PlatformErrorHandlingUtil::GetGnuErrorNumString(errno) + ")");
		}
	}

	return fpos;
}

void tc::io::FileStream::setLength_impl(int64_t length)
{
#ifdef _LARGEFILE64_SOURCE
	int trun_res = ftruncate64(mFileHandle->handle, length);
#else 
	int trun_res = ftruncate(mFileHandle->handle, length);
#endif

	if (trun_res == -1)
	{
		switch (errno)
		{
			case (EINTR):
			case (EINVAL):
			case (EFBIG):
			case (EIO):
			case (EBADF):
			default:
				throw tc::io::IOException(kClassName+"::seek()", "Failed to set stream position (" + PlatformErrorHandlingUtil::GetGnuErrorNumString(errno) + ")");
		}
	}
	
}

void tc::io::FileStream::flush_impl()
{
	// open/read/write are non-buffered
}
#endif