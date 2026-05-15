	/**
	 * @file PlatformErrorHandlingUtil.h
	 * @brief Declaration of tc::PlatformErrorHandlingUtil
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/04/09
	 **/
#pragma once
#include <tc/types.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace tc
{
	/**
	 * @class PlatformErrorHandlingUtil
	 * @brief Platform specific error handling utilities.
	 **/
class PlatformErrorHandlingUtil
{
public:
#ifdef _WIN32
		/**
		 * @brief Create a string from Win32 error code.
		 * 
		 * @param[in] error Error code, returned from GetLastError().
		 * 
		 * @return Error as a localised string.
		 **/
	static std::string GetLastErrorString(DWORD error);
#else
		/**
		 * @brief Create a string from GNU error number.
		 * 
		 * @param[in] errnum Error code, returned from @a errno macro.
		 * 
		 * @return Error as a localised string.
		 **/
	static std::string GetGnuErrorNumString(int errnum);
#endif

};

} // namespace tc