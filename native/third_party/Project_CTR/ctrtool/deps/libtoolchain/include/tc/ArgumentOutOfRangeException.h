	/**
	 * @file ArgumentOutOfRangeException.h
	 * @brief Declaration of tc::ArgumentOutOfRangeException
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/01/22
	 **/
#pragma once
#include <tc/ArgumentException.h>

namespace tc {

	/**
	 * @class ArgumentOutOfRangeException
	 * @brief The exception that is thrown when the value of an argument is outside the allowable range of values as defined by the invoked method.
	 **/
class ArgumentOutOfRangeException : public tc::ArgumentException
{
public:
		/// Default Constructor
	ArgumentOutOfRangeException() noexcept :
		tc::ArgumentException()
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
	ArgumentOutOfRangeException(const std::string& what) noexcept :
		tc::ArgumentException(what)
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
	ArgumentOutOfRangeException(const std::string& module, const std::string& what) noexcept :
		tc::ArgumentException(module, what)
	{
	}
};

} // namespace tc