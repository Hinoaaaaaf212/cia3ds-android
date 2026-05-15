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
	 * @enum FileAccess
	 * @brief Defines constants for read, write, or read/write access to a file.
	 **/
enum class FileAccess
{
	Read = 1, /**< Read access to the file. Data can be read from the file. Combine with Write for read/write access. */
	Write = 2, /**< Write access to the file. Data can be written to the file. Combine with Read for read/write access. */
	ReadWrite = Read|Write /**< Read and write access to the file. Data can be written to and read from the file. */
};

}} // namespace tc::io