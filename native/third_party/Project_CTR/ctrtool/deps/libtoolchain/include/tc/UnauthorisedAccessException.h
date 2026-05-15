	/**
	 * @file UnauthorisedAccessException.h
	 * @brief Declaration of tc::UnauthorisedAccessException
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/01/22
	 **/
#pragma once
#include <tc/Exception.h>

namespace tc {

	/**
	 * @class UnauthorisedAccessException
	 * @brief The exception that is thrown when the operating system denies access because of an I/O error or a specific type of security error.
	 **/
class UnauthorisedAccessException : public tc::Exception
{
public:
		/// Default Constructor
	UnauthorisedAccessException() noexcept :
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
	UnauthorisedAccessException(const std::string& what) noexcept :
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
	UnauthorisedAccessException(const std::string& module, const std::string& what) noexcept :
		tc::Exception(module, what)
	{
	}
};

} // namespace tc