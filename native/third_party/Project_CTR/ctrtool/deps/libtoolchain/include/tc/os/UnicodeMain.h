	/**
	 * @file UnicodeMain.h
	 * @brief Declaration of unicode entry point (umain())
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/04/13
	 * @details
	 * Including UnicodeMain.h bootstraps @ref umain() by defining the platform specific unicode entry point.
	 * 
	 * You must define @ref umain() as your entry point if you include this header. Compiler errors will occur if you define any other entrypoints or do not define @ref umain().
	 **/
#include <string>
#include <vector>
#ifdef _WIN32
#include <wchar.h>
#endif
#include <tc/string/TranscodeUtil.h>

#include <iostream>

	/**
	 * @brief Multi-platform UTF-8 entry point.
	 * 
	 * @param[in] args vector of UTF-8 encoded command-line arguments.
	 * @param[in] env vector of UTF-8 encoded environment variables.
	 * 
	 * @details
	 * You must define this function, it replaces using regular entry points like main, wmain, tmain, etc...
	 */
int umain(const std::vector<std::string>& args, const std::vector<std::string>& env);

#ifdef _WIN32
	/**
	 * @brief Native unicode entry point is defined here and bootstraps @ref umain().
	 * @warning Do not call or define this function
	 */
int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
#else
	/**
	 * @brief Native unicode entry point is defined here and bootstraps @ref umain().
	 * @warning Do not call or define this function
	 */
int main(int argc, char* argv[], char* envp[])
#endif
{
	std::vector<std::string> args;
	for (size_t i = 0; i < (size_t)argc; i++)
	{
#ifdef _WIN32
		std::string u8_arg;
		tc::string::TranscodeUtil::UTF16ToUTF8(std::u16string((char16_t*)argv[i]), u8_arg);
		args.push_back(u8_arg);
#else
		args.push_back(argv[i]);
#endif
	}

	std::vector<std::string> env;
	for (; *envp != nullptr; envp++)
	{
#ifdef _WIN32
		std::string u8_env;
		tc::string::TranscodeUtil::UTF16ToUTF8(std::u16string((char16_t*)*envp), u8_env);
		env.push_back(u8_env);
#else
		env.push_back(std::string(*envp));
#endif
	}

	return umain(args, env);
}