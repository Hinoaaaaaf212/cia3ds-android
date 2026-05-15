	/**
	 * @file DirectoryNotFoundException.h
	 * @brief Declaration of tc::io::DirectoryNotFoundException
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/01/22
	 **/
#pragma once
#include <tc/io/IOException.h>

namespace tc { namespace io {

	/**
	 * @class DirectoryNotFoundException
	 * @brief The exception that is thrown when part of a file or directory cannot be found.
	 **/
class DirectoryNotFoundException : public tc::io::IOException
{
public:
		/// Default Constructor
	DirectoryNotFoundException() noexcept :
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
	DirectoryNotFoundException(const std::string& what) noexcept :
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
	DirectoryNotFoundException(const std::string& module, const std::string& what) noexcept :
		tc::io::IOException(module, what)
	{
	}
};

}} // namespace tc::io