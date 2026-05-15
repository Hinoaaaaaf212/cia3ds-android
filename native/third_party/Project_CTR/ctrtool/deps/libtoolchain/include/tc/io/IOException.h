	/**
	 * @file IOException.h
	 * @brief Declaration of tc::io::IOException
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/01/22
	 **/
#pragma once
#include <tc/Exception.h>

namespace tc { namespace io {

	/**
	 * @class IOException
	 * @brief The exception that is thrown when an I/O error occurs.
	 **/
class IOException : public tc::Exception
{
public:
		/// Default Constructor
	IOException() noexcept :
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
	IOException(const std::string& what) noexcept :
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
	IOException(const std::string& module, const std::string& what) noexcept :
		tc::Exception(module, what)
	{
	}
};

}} // namespace tc::io