#include <tc/os/Environment.h>
#include <tc/string.h>

bool tc::os::getEnvVar(const std::string& name, std::string& value)
{
	bool did_find_variable = false;
#ifdef _WIN32
	// convert utf-8 to utf-16 for wide char functions
	std::u16string utf16_name;
	tc::string::TranscodeUtil::UTF8ToUTF16(name, utf16_name);

	// get size of environment variable
	size_t required_size = 0;
	_wgetenv_s(&required_size, nullptr, 0, (wchar_t*)utf16_name.c_str());

	// set output if variable was found
	if (required_size != 0)
	{
		// get environment variable
		std::shared_ptr<wchar_t> utf16_value(new wchar_t[required_size]);
		_wgetenv_s(&required_size, utf16_value.get(), required_size, (wchar_t*)utf16_name.c_str());

		// transcode back to utf-8
		tc::string::TranscodeUtil::UTF16ToUTF8((char16_t*)utf16_value.get(), value);

		did_find_variable = true;
	}
#else
	// get ptr to env variable
	char* env_ptr = getenv(name.c_str());

	// set output if variable was found
	if (env_ptr != nullptr)
	{
		value = std::string(env_ptr);
		did_find_variable = true;
	}
#endif
	return did_find_variable;
}