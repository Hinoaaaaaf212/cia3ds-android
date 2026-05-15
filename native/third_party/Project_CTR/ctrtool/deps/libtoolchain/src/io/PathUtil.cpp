#include <tc/io/PathUtil.h>
#include <tc/string.h>

void tc::io::PathUtil::pathToWindowsUTF16(const tc::io::Path& path, std::u16string& out)
{
	out = path.to_u16string(tc::io::Path::Format::Win32);
}

void tc::io::PathUtil::pathToUnixUTF8(const tc::io::Path& path, std::string& out)
{
	out = path.to_string(tc::io::Path::Format::POSIX);
}
