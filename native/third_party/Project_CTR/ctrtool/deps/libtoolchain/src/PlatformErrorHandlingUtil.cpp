#include <tc/PlatformErrorHandlingUtil.h>
#include <algorithm>

#ifdef _WIN32
std::string tc::PlatformErrorHandlingUtil::GetLastErrorString(DWORD error)
{
	if (error)
	{
		LPVOID lpMsgBuf;
		DWORD bufLen = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );
			
		if (bufLen)
		{
			LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
			std::string result(lpMsgStr, lpMsgStr+bufLen);
			
			LocalFree(lpMsgBuf);

			// remove CR+LF from string
			result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
			result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());

			return result;
		}
	}
	return std::string();
}
#else
std::string tc::PlatformErrorHandlingUtil::GetGnuErrorNumString(int errnum)
{
	return std::string(strerror(errnum));
}
#endif