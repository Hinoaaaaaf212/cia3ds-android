	/**
	 * @file CryptoException.h
	 * @brief Declaration of tc::crypto::CryptoException
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/01/22
	 **/
#pragma once
#include <tc/Exception.h>

namespace tc { namespace crypto {

	/**
	 * @class CryptoException
	 * @brief The exception that is thrown when an cryptography error occurs.
	 **/
class CryptoException : public tc::Exception
{
public:
		/// Default Constructor
	CryptoException() noexcept :
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
	CryptoException(const std::string& what) noexcept :
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
	CryptoException(const std::string& module, const std::string& what) noexcept :
		tc::Exception(module, what)
	{
	}
};

}} // namespace tc::crypto