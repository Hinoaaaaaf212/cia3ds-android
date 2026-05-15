	/**
	 * @file ObjectDisposedException.h
	 * @brief Declaration of tc::ObjectDisposedException
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/01/22
	 **/
#pragma once
#include <tc/InvalidOperationException.h>

namespace tc {

	/**
	 * @class ObjectDisposedException
	 * @brief The exception that is thrown when an operation is performed on a disposed object.
	 **/
class ObjectDisposedException : public tc::InvalidOperationException
{
public:
		/// Default Constructor
	ObjectDisposedException() noexcept :
		tc::InvalidOperationException()
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
	ObjectDisposedException(const std::string& what) noexcept :
		tc::InvalidOperationException(what)
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
	ObjectDisposedException(const std::string& module, const std::string& what) noexcept :
		tc::InvalidOperationException(module, what)
	{
	}
};

} // namespace tc