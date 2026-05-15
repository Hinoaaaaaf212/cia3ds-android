	/**
	 * @file SecurityException.h
	 * @brief Declaration of tc::SecurityException
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2022/02/06
	 **/
#pragma once
#include <tc/Exception.h>

namespace tc {

	/**
	 * @class SecurityException
	 * @brief The exception that is thrown when a security error is detected.
	 **/
class SecurityException : public tc::Exception
{
public:
		/// Default Constructor
	SecurityException() noexcept :
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
	SecurityException(const std::string& what) noexcept :
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
	SecurityException(const std::string& module, const std::string& what) noexcept :
		tc::Exception(module, what)
	{
	}
};

} // namespace tc