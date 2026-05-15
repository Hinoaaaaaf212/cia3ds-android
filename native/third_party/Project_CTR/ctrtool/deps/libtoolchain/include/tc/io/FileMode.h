	/**
	 * @file FileMode.h
	 * @brief Declaration of tc::io::FileMode
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/01/22
	 **/
#pragma once

namespace tc { namespace io {

	/**
	 * @enum FileMode
	 * @brief Specifies how the operating system should open a file.
	 **/
enum class FileMode
{
	CreateNew = 1, /**< Specifies that the operating system should create a new file. This requires Write permission. If the file already exists, an @ref tc::io::IOException exception is thrown. */
	Create = 2, /**< Specifies that the operating system should create a new file. If the file already exists, it will be overwritten. This requires Write permission. FileMode.Create is equivalent to requesting that if the file does not exist, use CreateNew; otherwise, use Truncate. If the file already exists but is a hidden file, an @ref tc::UnauthorisedAccessException exception is thrown. */
	Open = 3, /**< Specifies that the operating system should open an existing file. The ability to open the file is dependent on the value specified by the FileAccess enumeration. A @ref tc::io::FileNotFoundException exception is thrown if the file does not exist. */
	OpenOrCreate = 4, /**< Specifies that the operating system should open a file if it exists; otherwise, a new file should be created. If the file is opened with FileAccess.Read, Read permission is required. If the file access is FileAccess.Write, Write permission is required. If the file is opened with FileAccess.ReadWrite, both Read and Write permissions are required. */
	Truncate = 5, /**< Specifies that the operating system should open an existing file. When the file is opened, it should be truncated so that its size is zero bytes. This requires Write permission. Attempts to read from a file opened with FileMode.Truncate cause an @ref tc::ArgumentException exception. */
	Append = 6 /**< Opens the file if it exists and seeks to the end of the file, or creates a new file. This requires Append permission. FileMode.Append can be used only in conjunction with FileAccess.Write. Trying to seek to a position before the end of the file throws an @ref tc::io::IOException exception, and any attempt to read fails and throws a @ref tc::NotSupportedException exception. */
};

}} // namespace tc::io