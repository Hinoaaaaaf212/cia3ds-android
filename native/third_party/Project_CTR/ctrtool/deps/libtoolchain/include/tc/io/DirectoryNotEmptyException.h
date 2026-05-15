	/**
	 * @file DirectoryNotEmptyException.h
	 * @brief Declaration of tc::io::DirectoryNotEmptyException
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/01/22
	 **/
#pragma once
#include <tc/io/IOException.h>

namespace tc { namespace io {

	/**
	 * @class DirectoryNotEmptyException
	 * @brief The exception that is thrown when a directory is not empty.
	 **/
class DirectoryNotEmptyException : public tc::io::IOException
{
public:
		/// Default Constructor
	DirectoryNotEmptyException() noexcept :
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
	DirectoryNotEmptyException(const std::string& what) noexcept :
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
	DirectoryNotEmptyException(const std::string& module, const std::string& what) noexcept :
		tc::io::IOException(module, what)
	{
	}
};

}} // namespace tc::io