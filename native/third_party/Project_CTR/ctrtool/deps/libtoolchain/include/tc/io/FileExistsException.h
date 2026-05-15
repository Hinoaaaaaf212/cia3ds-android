	/**
	 * @file FileExistsException.h
	 * @brief Declaration of tc::io::FileExistsException
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/01/26
	 **/
#pragma once
#include <tc/io/IOException.h>

namespace tc { namespace io {

	/**
	 * @class FileExistsException
	 * @brief The exception that is thrown when an attempt to overwrite/remove a file that exists on disk fails.
	 **/
class FileExistsException : public tc::io::IOException
{
public:
		/// Default Constructor
	FileExistsException() noexcept :
		tc::io::IOException()
	{
	}

		/**
		 * @brief Basic Parameterized Constructor
		 * 
		 * @param[in] what Explanation for exception
		 * 
		 * @post
		 * - what() == what
		 * - module() == ""
		 * - error() == what
		 **/
	FileExistsException(const std::string& what) noexcept :
		tc::io::IOException(what)
	{}

		/**
		 * @brief Parameterized Constructor
		 * 
		 * @param[in] module Name of module that threw the exception
		 * @param[in] what Explanation for exception
		 * 
		 * @post
		 * - what() == "[" + module + " ERROR] " + what
		 * - module() == module
		 * - error() == what
		 **/
	FileExistsException(const std::string& module, const std::string& what) noexcept :
		tc::io::IOException(module, what)
	{
	}
};

}} // namespace tc::io