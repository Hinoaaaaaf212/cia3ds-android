	/**
	 * @file OverflowException.h
	 * @brief Declaration of tc::OverflowException
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/01/22
	 **/
#pragma once
#include <tc/ArithmeticException.h>

namespace tc {

	/**
	 * @class OverflowException
	 * @brief The exception that is thrown when an arithmetic, casting, or conversion operation in a checked context results in an overflow.
	 **/
class OverflowException : public tc::ArithmeticException
{
public:
		/// Default Constructor
	OverflowException() noexcept :
		tc::ArithmeticException()
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
	OverflowException(const std::string& what) noexcept :
		tc::ArithmeticException(what)
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
	OverflowException(const std::string& module, const std::string& what) noexcept :
		tc::ArithmeticException(module, what)
	{
	}
};

} // namespace tc