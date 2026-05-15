	/**
	 * @file OutOfMemoryException.h
	 * @brief Declaration of tc::OutOfMemoryException
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/01/22
	 **/
#pragma once
#include <tc/Exception.h>

namespace tc {

	/**
	 * @class OutOfMemoryException
	 * @brief The exception that is thrown when there is not enough memory to continue the execution of a program.
	 **/
class OutOfMemoryException : public tc::Exception
{
public:
		/// Default Constructor
	OutOfMemoryException() noexcept :
		tc::Exception()
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
	OutOfMemoryException(const std::string& what) noexcept :
		tc::Exception(what)
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
	OutOfMemoryException(const std::string& module, const std::string& what) noexcept :
		tc::Exception(module, what)
	{
	}
};

} // namespace tc