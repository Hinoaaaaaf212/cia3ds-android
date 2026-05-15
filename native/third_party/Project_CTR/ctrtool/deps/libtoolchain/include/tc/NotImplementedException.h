	/**
	 * @file NotImplementedException.h
	 * @brief Declaration of tc::NotImplementedException
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/01/22
	 **/
#pragma once
#include <tc/Exception.h>

namespace tc {

	/**
	 * @class NotImplementedException
	 * @brief The exception that is thrown when a requested method or operation is not implemented.
	 **/
class NotImplementedException : public tc::Exception
{
public:
		/// Default Constructor
	NotImplementedException() noexcept :
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
	NotImplementedException(const std::string& what) noexcept :
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
	NotImplementedException(const std::string& module, const std::string& what) noexcept :
		tc::Exception(module, what)
	{
	}
};

} // namespace tc