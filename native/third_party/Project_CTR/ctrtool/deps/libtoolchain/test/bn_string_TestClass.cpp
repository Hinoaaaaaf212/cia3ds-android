#include <tc/Exception.h>
#include <fmt/core.h>

#include "bn_string_TestClass.h"

void bn_string_TestClass::runAllTests(void)
{
	fmt::print("[tc::bn::string] START\n");
	test_EncodedSizeVersusLogicalSize();
	test_StringSizeNeverExceedsLogicalSize();
	test_EncodeStringRespectsLogicalSize();
	fmt::print("[tc::bn::string] END\n");
}

void bn_string_TestClass::test_EncodedSizeVersusLogicalSize()
{
	fmt::print("[tc::bn::string] test_EncodedSizeVersusLogicalSize : ");
	try
	{
		try 
		{
			// create bn::string with encoded size of 16 bytes (and implicit logical size of 16)
			{
				static const size_t kEncodedSize = 16;
				tc::bn::string<kEncodedSize> bn_string;

				if (sizeof(bn_string) != kEncodedSize)
				{
					throw tc::Exception(fmt::format("sizeof(tc::bn::string<{:d}>) did not return ENCODED_SIZE ({:d})", kEncodedSize, kEncodedSize));
				}

				if (bn_string.max_size() != kEncodedSize)
				{
					throw tc::Exception(fmt::format("tc::bn::string<{:d}>::max_size() did not return LOGICAL_SIZE ({:d})", kEncodedSize, kEncodedSize));
				}
			}

			// create bn::string with encoded size of 16 bytes and explicit logical size of 16
			{
				static const size_t kEncodedSize = 16;
				static const size_t kLogicalSize = 16;
				tc::bn::string<kEncodedSize, kLogicalSize> bn_string;

				if (sizeof(bn_string) != kEncodedSize)
				{
					throw tc::Exception(fmt::format("sizeof(tc::bn::string<{:d}, {:d}>) did not return ENCODED_SIZE ({:d})", kEncodedSize, kLogicalSize, kEncodedSize));
				}

				if (bn_string.max_size() != kLogicalSize)
				{
					throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::max_size() did not return LOGICAL_SIZE ({:d})", kEncodedSize, kLogicalSize, kLogicalSize));
				}
			}

			// create bn::string with encoded size of 16 bytes and explicit logical size of 8
			{
				static const size_t kEncodedSize = 16;
				static const size_t kLogicalSize = 8;
				tc::bn::string<kEncodedSize, kLogicalSize> bn_string;

				if (sizeof(bn_string) != kEncodedSize)
				{
					throw tc::Exception(fmt::format("sizeof(tc::bn::string<{:d}, {:d}>) did not return ENCODED_SIZE ({:d})", kEncodedSize, kLogicalSize, kEncodedSize));
				}

				if (bn_string.max_size() != kLogicalSize)
				{
					throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::max_size() did not return LOGICAL_SIZE ({:d})", kEncodedSize, kLogicalSize, kLogicalSize));
				}
			}

			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void bn_string_TestClass::test_StringSizeNeverExceedsLogicalSize()
{
	fmt::print("[tc::bn::string] test_StringSizeNeverExceedsLogicalSize : ");
	try
	{
		try 
		{
			union {
				std::array<char, 30> char_array;
				tc::bn::string<20,20> str_2020;
				tc::bn::string<20,10> str_2010;
				tc::bn::string<20,05> str_2005;
			} test;

			// fill underlying data with different characters
			for (char chr_idx = 0; chr_idx < test.char_array.size(); chr_idx++)
			{
				test.char_array[chr_idx] = ((chr_idx / 26) & 1) ? ('a' + (chr_idx % 26)) : ('A' + (chr_idx % 26));
			}

			// generate reference point string
			std::string base_string = std::string(test.char_array.data(), 30);

			if (test.str_2020.size() != 20)
			{
				throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::size() did not respect LOGICAL_SIZE ({:d})", 20, 20, 20));
			}

			if (test.str_2020.data() != test.char_array.data())
			{
				throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::data() did not return expected pointer)", 20, 20));
			}

			if (test.str_2020.decode() != base_string.substr(0, 20))
			{
				throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::decode() returned \"{:s}\" (expected \"{:s}\")", 20, 20, test.str_2020.decode(), base_string.substr(0, 20)));
			}

			if (test.str_2010.size() != 10)
			{
				throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::size() did not respect LOGICAL_SIZE ({:d})", 20, 10, 10));
			}

			if (test.str_2010.data() != test.char_array.data())
			{
				throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::data() did not return expected pointer)", 20, 10));
			}

			if (test.str_2010.decode() != base_string.substr(0, 10))
			{
				throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::decode() returned \"{:s}\" (expected \"{:s}\")", 20, 10, test.str_2010.decode(), base_string.substr(0, 10)));
			}

			if (test.str_2005.size() != 5)
			{
				throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::size() did not respect LOGICAL_SIZE ({:d})", 20, 5, 5));
			}

			if (test.str_2005.data() != test.char_array.data())
			{
				throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::data() did not return expected pointer)", 20, 05));
			}

			if (test.str_2005.decode() != base_string.substr(0, 5))
			{
				throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::decode() returned \"{:s}\" (expected \"{:s}\")", 20, 5, test.str_2005.decode(), base_string.substr(0, 5)));
			}

			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void bn_string_TestClass::test_EncodeStringRespectsLogicalSize()
{
	fmt::print("[tc::bn::string] test_EncodeStringRespectsLogicalSize : ");
	try
	{
		try 
		{
			union {
				std::array<char, 30> char_array;
				tc::bn::string<20,20> str_2020;
			} test;

			std::array<char, 30> empty_array;

			// clear memory
			memset(test.char_array.data(), 0, test.char_array.size());
			memset(empty_array.data(), 0, empty_array.size());

			// generate reference point string
			std::string base_long_string = std::string("I am a very long string indeed");
			std::string base_short_string = std::string("Smol str");

			test.str_2020.encode(base_long_string);
			if (test.str_2020.decode() != base_long_string.substr(0, 20))
			{
				throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::encode() failed encode a string greater ({:d}) than the LOGICAL_SIZE", 20, 20, base_long_string.size()));
			}
			if (memcmp(test.char_array.data() + 20, empty_array.data(), 10) != 0)
			{
				throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::encode() touched data after the LOGICAL_SIZE boundary", 20, 20));
			}

			test.str_2020.encode(base_short_string);
			if (test.str_2020.decode() != base_short_string)
			{
				throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::encode() failed encode a string smaller ({:d}) than the LOGICAL_SIZE)", 20, 20, base_short_string.size()));
			}
			if (memcmp(test.char_array.data() + base_short_string.size(), empty_array.data(), 20-base_short_string.size()) != 0)
			{
				throw tc::Exception(fmt::format("tc::bn::string<{:d}, {:d}>::encode() did not clear data after string ended", 20, 20));
			}

			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}