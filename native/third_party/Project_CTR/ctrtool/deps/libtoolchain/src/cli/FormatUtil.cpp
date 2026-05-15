#include <tc/cli/FormatUtil.h>

#include <fmt/core.h>

inline int charToByte(char chr)
{
	if (chr >= 'a' && chr <= 'f')
		return (chr - 'a') + 0xa;
	else if (chr >= 'A' && chr <= 'F')
		return (chr - 'A') + 0xa; 
	else if (chr >= '0' && chr <= '9')
		return chr - '0';
	return -1;
}

tc::ByteData tc::cli::FormatUtil::hexStringToBytes(const std::string& str)
{
	size_t size = str.size();
	if ((size % 2))
	{
		return tc::ByteData();
	}

	auto bytes = tc::ByteData(size/2);

	for (size_t i = 0; i < bytes.size(); i++)
	{
		int byte = 0;

		byte = charToByte(str[i * 2]);
		if (byte == -1)
			return tc::ByteData();

		bytes.data()[i] = byte_t((byte & 0xf) << 4);

		byte = charToByte(str[(i * 2) + 1]);
		if (byte == -1)
			return tc::ByteData();
		
		bytes.data()[i] |= byte_t((byte & 0xf) << 0);
	}

	return bytes;
}

std::string tc::cli::FormatUtil::formatBytesAsStringWithLineLimit(const byte_t* data, size_t len, bool upper_case, const std::string& delimiter, size_t row_len, size_t indent_len, bool print_first_indent)
{
	// create indentation string
	std::string indent_str = "";
	for (size_t i = 0; i < indent_len; i++)
	{
		indent_str += " ";
	}

	const byte_t* original_data = data;

	// create output string
	std::string output_str = "";

	for (size_t print_len = 0; len > 0; len -= print_len, data += print_len)
	{
		if (data != original_data || print_first_indent)
		{
			output_str += indent_str;
		}

		print_len = std::min<size_t>(len, row_len);  

		output_str += formatBytesAsString(data, print_len, upper_case, delimiter);

		output_str += fmt::format("\n");
	}

	return output_str;
}

std::string tc::cli::FormatUtil::formatBytesAsString(const byte_t* data, size_t size, bool upper_case, const std::string& delimiter)
{
	// create output string
	std::string output_str;

	for (size_t i = 0; i < size; i++)
	{
		output_str += fmt::format((upper_case ? "{:02X}" : "{:02x}"), data[i]);
		if (i+1 < size)
		{
			output_str += delimiter;
		}	
	}

	return output_str;
}

std::string tc::cli::FormatUtil::formatBytesAsString(const tc::ByteData& data, bool upper_case, const std::string& delimiter)
{
	return formatBytesAsString(data.data(), data.size(), upper_case, delimiter);
}


std::string tc::cli::FormatUtil::formatListWithLineLimit(const std::vector<std::string>& str_list, size_t row_len, size_t indent_len, bool print_first_indent)
{
	if (str_list.size() == 0)
	{
		return "";
	}

	// create output string
	std::string output_str = "";

	// create indentation string
	std::string indent_str = "";
	for (size_t i = 0; i < indent_len; i++)
	{
		indent_str += " ";
	}

	// create delimiter string
	std::string delimiter_str = ", ";

	size_t printed_len = 0;
	for (auto itr = str_list.begin(); itr != str_list.end(); itr++)
	{
		// format the strings
		// wrap the line after row_len multples
		if (printed_len > row_len || printed_len == 0)
		{
			// don't print the new line if this is the first string
			if (itr != str_list.begin())
			{
				output_str += fmt::format("{:s}\n", delimiter_str);
			}	

			// print indent if this isn't the first string or the user has opted into printing the indent regardless
			if (itr != str_list.begin() || print_first_indent)
			{
				output_str += indent_str;
			}

			// reset printed_len
			printed_len = 0;
		}
		// within a line we want to separate the next string from the last one with a comma and a space
		else
		{
			//ss << delimiter_str;
			output_str += delimiter_str;
		}
		
		// print string
		output_str += *itr;

		// note the length of the string printed
		printed_len += itr->size() + delimiter_str.size();
	}
	output_str += fmt::format("\n");

	return output_str;
}

std::string tc::cli::FormatUtil::formatBytesAsHxdHexString(const byte_t* data, size_t size, size_t bytes_per_row, size_t byte_group_size)
{
	if (size == 0 || bytes_per_row == 0 || byte_group_size == 0)
	{
		return "";
	}

	// create output string
	std::string output_str = "";

	// iterate over blocks
	for (size_t i = 0; size > 0; i++)
	{
		size_t row_print_len = std::min<size_t>(size, bytes_per_row);

		output_str += fmt::format("{:08x} | ", uint64_t(i) * uint64_t(bytes_per_row));

		// for block i print each byte
		for (size_t j = 0; j < bytes_per_row; j++)
		{
			if (j < row_print_len)
			{
				output_str += fmt::format("{:02X}", data[(i * bytes_per_row) + j]);
			}
			else
			{
				output_str += "  ";
			}
				

			if (((j+1) % byte_group_size) == 0) 
			{
				output_str += " ";
			}
		}

		output_str += " "; 

		for (size_t j = 0; j < bytes_per_row; j++)
		{
			if (j < row_print_len)
			{
				byte_t byte = data[(i * bytes_per_row) + j];
				output_str += fmt::format("{:c}", (iscntrl(byte) == 0 && byte < 0x7f) ? (char)byte : '.');
			}
			else
			{
				output_str += " ";
			}
		}

		output_str += fmt::format("\n");

		size -= row_print_len;
	}

	return output_str;
}

std::string tc::cli::FormatUtil::formatBytesAsHxdHexString(const byte_t* data, size_t size)
{
	return formatBytesAsHxdHexString(data, size, 0x10, 1);
}