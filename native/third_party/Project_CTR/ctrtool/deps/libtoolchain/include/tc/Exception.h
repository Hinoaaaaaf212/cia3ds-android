	/**
	 * @file Exception.h
	 * @brief Declaration of tc::Exception
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2019/01/15
	 **/
#pragma once
#include <exception>
#include <string>

namespace tc {

	/**
	 * @class Exception
	 * @brief An extension of std::exception that allows optionally specifying a module name
	 **/
class Exception : public std::exception
{
public:
		/// Default Constructor
	Exception() noexcept;

		/**
		 * @brief Basic Parameterized Constructor
		 * 
		 * Inherited from std::exception
		 * 
		 * @param[in] what Explanation for exception
		 * 
		 * @post
		 * - what() == what
		 * - module() == ""
		 * - error() == what
		 **/
	Exception(const std::string& what) noexcept;

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
	Exception(const std::string& module, const std::string& what) noexcept;

		/// Get explanation for exception (inherited from std::exception)
	const char* what() const noexcept;

		/// Get module tag 
	const char* module() const noexcept;

		/**
		 * @brief Get explanation for exception
		 * 
		 * Omits the module tag from the description
		 * 
		 * @returns exception description
		 **/
	const char* error() const noexcept;
private:
	std::string what_;
	std::string module_;
	std::string error_;
};

} // namespace tc
