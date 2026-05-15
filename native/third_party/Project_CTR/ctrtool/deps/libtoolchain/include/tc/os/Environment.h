	/**
	 * @file Environment.h
	 * @brief Declarations for API resources for accessing run-time environment
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/06/12
	 **/
#pragma once
#include <tc/types.h>
#include <tc/ByteData.h>

namespace tc { namespace os {

	/**
	 * @brief Get environment variable.
	 * 
	 * @details This function supports UTF-8 encoding
	 * 
	 * @param[in] name Name of environment variable.
	 * @param[out] value Reference to string to populate with variable.
	 *
	 * @post @p value will contain the environment variable if it exists.
	 * 
	 * @return true if operation was successful.
	 */
bool getEnvVar(const std::string& name, std::string& value);

}} // namespace tc::cli