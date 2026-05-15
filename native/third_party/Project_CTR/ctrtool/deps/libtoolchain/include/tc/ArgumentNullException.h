	/**
	 * @file ArgumentNullException.h
	 * @brief Declaration of tc::ArgumentNullException
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/01/22
	 **/
#pragma once
#include <tc/ArgumentException.h>

namespace tc {

	/**
	 * @class ArgumentNullException
	 * @brief The exception that is thrown when a null reference is passed to a method that does not accept it as a valid argument.
	 **/
class ArgumentNullException : public tc::ArgumentException
{
public:
		/// Default Constructor
	ArgumentNullException() noexcept :
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
	ArgumentNullException(const std::string& what) noexcept :
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
	ArgumentNullException(const std::string& module, const std::string& what) noexcept :
		tc::ArgumentException(module, what)
	{
	}
};

} // namespace tc