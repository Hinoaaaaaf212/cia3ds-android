#include "cli_FormatUtil_TestClass.h"
#include <iostream>
#include <iomanip>
#include <sstream>

void cli_FormatUtil_TestClass::runAllTests()
{
	std::cout << "[tc::cli::FormatUtil] START" << std::endl;
	testHexStringToBytes();
	testFormatBytesAsString();
	testFormatBytesAsStringWithLineLimit();
	testFormatListWithLineLimit();
	testFormatBytesAsHxdHexString();
	std::cout << "[tc::cli::FormatUtil] END" << std::endl;
}

void cli_FormatUtil_TestClass::testHexStringToBytes()
{
	std::cout << "[tc::cli::FormatUtil] testHexStringToBytes : " << std::flush;
	try
	{
		std::stringstream ss;

		struct TestCase
		{
			std::string test_name;
			std::string in_string;
			tc::ByteData out_data;
		};

		// create manual tests
		std::vector<TestCase> tests = 
		{
			{ "empty string", "", tc::ByteData()},
			{ "unaligned string" ,"1", tc::ByteData()},
			{ "unaligned larger string" ,"123456789", tc::ByteData()},
			{ "multi-byte" ,"00112233445566778899aabbccddeeff010203", tc::ByteData({0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01, 0x02, 0x03})},
		};

		// add programatically determined tests
		for (size_t i = 0; i < 0xff; i++)
		{
			std::string all_lower, all_upper, upper_lower, lower_upper;

			size_t upper_num = (i >> 4) & 0xf;
			size_t lower_num = (i) & 0xf;

			if (upper_num <= 9)
			{
				all_lower += char('0' + upper_num);
				all_upper += char('0' + upper_num);
				upper_lower += char('0' + upper_num);
				lower_upper += char('0' + upper_num);
			}
			else
			{
				all_lower += char('a' + upper_num - 10);
				all_upper += char('A' + upper_num - 10);
				upper_lower += char('A' + upper_num - 10);
				lower_upper += char('a' + upper_num - 10);
			}

			if (lower_num <= 9)
			{
				all_lower += char('0' + lower_num);
				all_upper += char('0' + lower_num);
				upper_lower += char('0' + lower_num);
				lower_upper += char('0' + lower_num);
			}
			else
			{
				all_lower += char('a' + lower_num - 10);
				all_upper += char('A' + lower_num - 10);
				upper_lower += char('a' + lower_num - 10);
				lower_upper += char('A' + lower_num - 10);
			}

			tests.push_back({"all_lower_" + all_lower, all_lower, tc::ByteData({(byte_t)i})});
			tests.push_back({"all_upper_" + all_lower, all_upper, tc::ByteData({(byte_t)i})});
			tests.push_back({"upper_lower_" + all_lower, upper_lower, tc::ByteData({(byte_t)i})});
			tests.push_back({"lower_upper_" + all_lower, lower_upper, tc::ByteData({(byte_t)i})});
		}

		// run tests
		for (auto test = tests.begin(); test != tests.end(); test++)
		{
			tc::ByteData out = tc::cli::FormatUtil::hexStringToBytes(test->in_string);

			if (out.size() != test->out_data.size())
			{
				ss << "Test(" << test->test_name << ") to convert str(" << test->in_string << ") returned ByteData with wrong size: " << out.size() << " (should be: " << test->out_data.size() << ")";
				throw tc::Exception(ss.str());
			}

			if (out.size() != 0 && memcmp(out.data(), test->out_data.data(), out.size()) != 0)
			{
				ss << "Test(" << test->test_name << ") to convert str(" << test->in_string << ") returned ByteData with wrong data";
				throw tc::Exception(ss.str());
			}
		}

		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e)
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl; 
	}
}

void cli_FormatUtil_TestClass::testFormatBytesAsString()
{
	std::cout << "[tc::cli::FormatUtil] testFormatBytesAsString : " << std::flush;
	try
	{
		std::stringstream ss;

		// test recipe
		struct TestCase
		{
			std::string test_name;
			tc::ByteData in_data;
			bool in_is_uppercase;
			std::string in_delimiter;
			std::string out_string;
		};

		// create tests
		std::vector<TestCase> tests = 
		{
			{"empty data", tc::ByteData(), false, "", ""},
			{"8byte lowercase no delim", tc::ByteData({0x00, 0xaa, 0x11, 0xbb, 0x22, 0xcc, 0x33, 0xdd}), false, "", "00aa11bb22cc33dd"},
			{"8byte lowercase ' ' delim", tc::ByteData({0x00, 0xaa, 0x11, 0xbb, 0x22, 0xcc, 0x33, 0xdd}), false, " ", "00 aa 11 bb 22 cc 33 dd"},
			{"8byte lowercase ':' delim", tc::ByteData({0x00, 0xaa, 0x11, 0xbb, 0x22, 0xcc, 0x33, 0xdd}), false, ":", "00:aa:11:bb:22:cc:33:dd"},
			{"8byte uppercase no delim", tc::ByteData({0x00, 0xaa, 0x11, 0xbb, 0x22, 0xcc, 0x33, 0xdd}), true, "", "00AA11BB22CC33DD"},
			{"8byte uppercase ' ' delim", tc::ByteData({0x00, 0xaa, 0x11, 0xbb, 0x22, 0xcc, 0x33, 0xdd}), true, " ", "00 AA 11 BB 22 CC 33 DD"},
			{"8byte uppercase ':' delim", tc::ByteData({0x00, 0xaa, 0x11, 0xbb, 0x22, 0xcc, 0x33, 0xdd}), true, ":", "00:AA:11:BB:22:CC:33:DD"},
		};


		// run tests
		for (auto test = tests.begin(); test != tests.end(); test++)
		{
			std::string res = tc::cli::FormatUtil::formatBytesAsString(test->in_data.data(), test->in_data.size(), test->in_is_uppercase, test->in_delimiter);

			if (res != test->out_string)
			{
				ss << "Test(" << test->test_name << ") Failed to format data correctly. Output: \"" << res << "\", Expected: \"" << test->out_string << "\"";
				throw tc::Exception(ss.str());
			}
		}

		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e)
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl; 
	}
}

void cli_FormatUtil_TestClass::testFormatBytesAsStringWithLineLimit()
{
	std::cout << "[tc::cli::FormatUtil] testFormatBytesAsStringWithLineLimit : " << std::flush;
	try
	{
		std::stringstream ss;

		// test recipe
		struct TestCase
		{
			std::string test_name;
			tc::ByteData in_data;
			size_t in_row_len;
			size_t in_indent_len;
			std::string out_string;
		};

		// create tests
		std::vector<TestCase> tests = 
		{
			{"empty data", tc::ByteData(), 0, 0, ""},
			{"size:8 row:8 indent:0", tc::ByteData({0x00, 0xaa, 0x11, 0xbb, 0x22, 0xcc, 0x33, 0xdd}), 8, 0, "00aa11bb22cc33dd\n"},
			{"size:8 row:4 indent:0", tc::ByteData({0x00, 0xaa, 0x11, 0xbb, 0x22, 0xcc, 0x33, 0xdd}), 4, 0, "00aa11bb\n22cc33dd\n"},
			{"size:8 row:3 indent:0", tc::ByteData({0x00, 0xaa, 0x11, 0xbb, 0x22, 0xcc, 0x33, 0xdd}), 3, 0, "00aa11\nbb22cc\n33dd\n"},
			{"size:8 row:3 indent:2", tc::ByteData({0x00, 0xaa, 0x11, 0xbb, 0x22, 0xcc, 0x33, 0xdd}), 3, 2, "  00aa11\n  bb22cc\n  33dd\n"},
			{"size:8 row:1 indent:3", tc::ByteData({0x00, 0xaa, 0x11, 0xbb, 0x22, 0xcc, 0x33, 0xdd}), 1, 3, "   00\n   aa\n   11\n   bb\n   22\n   cc\n   33\n   dd\n"},
		};


		// run tests
		for (auto test = tests.begin(); test != tests.end(); test++)
		{
			std::string res = tc::cli::FormatUtil::formatBytesAsStringWithLineLimit(test->in_data.data(), test->in_data.size(), false, "", test->in_row_len, test->in_indent_len);

			if (res != test->out_string)
			{
				std::string& expected = test->out_string;

				// replace literal new lines so they can be printed on one line for debugging
				for (size_t pos = res.find('\n', 0); pos != std::string::npos; pos = res.find('\n', pos+1))
				{
					res.replace(pos, 1, "\\n");
				}

				// replace literal new lines so they can be printed on one line for debugging
				for (size_t pos = expected.find('\n', 0); pos != std::string::npos; pos = expected.find('\n', pos+1))
				{
					expected.replace(pos, 1, "\\n");
				}
				
				ss << "Test(" << test->test_name << ") Failed to format data correctly. Output: \"" << res << "\", Expected: \"" << expected << "\"";
				throw tc::Exception(ss.str());
			}
		}

		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e)
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl; 
	}
}

void cli_FormatUtil_TestClass::testFormatListWithLineLimit()
{
	std::cout << "[tc::cli::FormatUtil] testFormatListWithLineLimit : " << std::flush;
	try
	{
		std::stringstream ss;

		// test recipe
		struct TestCase
		{
			std::string test_name;
			std::vector<std::string> in_list;
			size_t in_row_len;
			size_t in_indent_len;
			std::string out_string;
		};

		// create tests
		std::vector<TestCase> tests = 
		{
			{"empty", {}, 0, 0, ""},
			{"empty list", {}, 40, 0, ""},
			{"list of 4, row_len 20", {"Astr", "Bstr", "Cstr", "Dstr"}, 20, 0, "Astr, Bstr, Cstr, Dstr\n"},
			{"list of 4, row_len 8", {"Astr", "Bstr", "Cstr", "Dstr"}, 8, 0, "Astr, Bstr, \nCstr, Dstr\n"},
			{"list of 4, row_len 8, indent=2", {"Astr", "Bstr", "Cstr", "Dstr"}, 8, 2, "  Astr, Bstr, \n  Cstr, Dstr\n"},
			{"list of 4, row_len 4", {"Astr", "Bstr", "Cstr", "Dstr"}, 4, 0, "Astr, \nBstr, \nCstr, \nDstr\n"},
			{"list of 4, row_len 2", {"Astr", "Bstr", "Cstr", "Dstr"}, 2, 0, "Astr, \nBstr, \nCstr, \nDstr\n"},
			{"list of 4, row_len 1", {"Astr", "Bstr", "Cstr", "Dstr"}, 1, 0, "Astr, \nBstr, \nCstr, \nDstr\n"},
			{"list of 4, row_len 0", {"Astr", "Bstr", "Cstr", "Dstr"}, 0, 0, "Astr, \nBstr, \nCstr, \nDstr\n"},
		};


		// run tests
		for (auto test = tests.begin(); test != tests.end(); test++)
		{
			std::string res = tc::cli::FormatUtil::formatListWithLineLimit(test->in_list, test->in_row_len, test->in_indent_len);

			if (res != test->out_string)
			{
				std::string& expected = test->out_string;

				// replace literal new lines so they can be printed on one line for debugging
				for (size_t pos = res.find('\n', 0); pos != std::string::npos; pos = res.find('\n', pos+1))
				{
					res.replace(pos, 1, "\\n");
				}

				// replace literal new lines so they can be printed on one line for debugging
				for (size_t pos = expected.find('\n', 0); pos != std::string::npos; pos = expected.find('\n', pos+1))
				{
					expected.replace(pos, 1, "\\n");
				}
				
				ss << "Test(" << test->test_name << ") Failed to format data correctly. Output: \"" << res << "\", Expected: \"" << expected << "\"";
				throw tc::Exception(ss.str());
			}
		}

		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e)
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl; 
	}
}

void cli_FormatUtil_TestClass::testFormatBytesAsHxdHexString()
{
	std::cout << "[tc::cli::FormatUtil] testFormatBytesAsHxdHexString : " << std::flush;
	try
	{
		std::stringstream ss;

		// test recipe
		struct TestCase
		{
			std::string test_name;
			tc::ByteData in_data;
			size_t in_bytes_per_row;
			size_t in_byte_group_size;
			std::string out_string;
		};

		// create tests
		std::vector<TestCase> tests = 
		{
			{"empty all", {}, 0, 0, ""},
			{"empty data", {}, 16, 1, ""},
			{"empty bytes_per_row", {0x00, 0x01}, 0, 1, ""},
			{"empty byte_group_size", {0x00, 0x01}, 16, 0, ""},
			{"little data", {0x00, 0x01}, 16, 1, "00000000 | 00 01                                            ..              \n"},
			{"full row data", {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 1, "00000000 | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ................\n"},
			{"2 row ascii", {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4e, 0x4f, 0x50, 0x51, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66}, 16, 1, "00000000 | 41 42 43 44 45 46 47 48 49 4A 4B 4C 4E 4F 50 51  ABCDEFGHIJKLNOPQ\n00000010 | 51 52 53 54 55 56 57 58 59 5A 61 62 63 64 65 66  QRSTUVWXYZabcdef\n"},
			{"full row data, byte_group_size=2",  {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 2,  "00000000 | 0001 0203 0405 0607 0809 0A0B 0C0D 0E0F  ................\n"},
			{"full row data, byte_group_size=3",  {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 3,  "00000000 | 000102 030405 060708 090A0B 0C0D0E 0F ................\n"},
			{"full row data, byte_group_size=4",  {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 4,  "00000000 | 00010203 04050607 08090A0B 0C0D0E0F  ................\n"},
			{"full row data, byte_group_size=5",  {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 5,  "00000000 | 0001020304 0506070809 0A0B0C0D0E 0F ................\n"},
			{"full row data, byte_group_size=6",  {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 6,  "00000000 | 000102030405 060708090A0B 0C0D0E0F ................\n"},
			{"full row data, byte_group_size=7",  {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 7,  "00000000 | 00010203040506 0708090A0B0C0D 0E0F ................\n"},
			{"full row data, byte_group_size=8",  {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 8,  "00000000 | 0001020304050607 08090A0B0C0D0E0F  ................\n"},
			{"full row data, byte_group_size=9",  {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 9,  "00000000 | 000102030405060708 090A0B0C0D0E0F ................\n"},
			{"full row data, byte_group_size=10", {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 10, "00000000 | 00010203040506070809 0A0B0C0D0E0F ................\n"},
			{"full row data, byte_group_size=11", {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 11, "00000000 | 000102030405060708090A 0B0C0D0E0F ................\n"},
			{"full row data, byte_group_size=12", {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 12, "00000000 | 000102030405060708090A0B 0C0D0E0F ................\n"},
			{"full row data, byte_group_size=13", {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 13, "00000000 | 000102030405060708090A0B0C 0D0E0F ................\n"},
			{"full row data, byte_group_size=14", {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 14, "00000000 | 000102030405060708090A0B0C0D 0E0F ................\n"},
			{"full row data, byte_group_size=15", {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 15, "00000000 | 000102030405060708090A0B0C0D0E 0F ................\n"},
			{"full row data, byte_group_size=16", {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, 16, 16, "00000000 | 000102030405060708090A0B0C0D0E0F  ................\n"},
		};


		// run tests
		for (auto test = tests.begin(); test != tests.end(); test++)
		{
			std::string res = tc::cli::FormatUtil::formatBytesAsHxdHexString(test->in_data.data(), test->in_data.size(), test->in_bytes_per_row, test->in_byte_group_size);

			if (res != test->out_string)
			{
				std::string& expected = test->out_string;

				// replace literal new lines so they can be printed on one line for debugging
				for (size_t pos = res.find('\n', 0); pos != std::string::npos; pos = res.find('\n', pos+1))
				{
					res.replace(pos, 1, "\\n");
				}

				// replace literal new lines so they can be printed on one line for debugging
				for (size_t pos = expected.find('\n', 0); pos != std::string::npos; pos = expected.find('\n', pos+1))
				{
					expected.replace(pos, 1, "\\n");
				}
				
				ss << "Test(" << test->test_name << ") Failed to format data correctly. Output: \"" << res << "\", Expected: \"" << expected << "\"";
				throw tc::Exception(ss.str());
			}
		}

		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e)
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl; 
	}
}