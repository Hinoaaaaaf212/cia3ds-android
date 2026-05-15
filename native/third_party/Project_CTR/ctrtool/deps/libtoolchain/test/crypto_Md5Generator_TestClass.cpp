#include <iostream>
#include <sstream>
#include <fstream>

#include "crypto_Md5Generator_TestClass.h"

#include <tc/Exception.h>
#include <tc/crypto/Md5Generator.h>
#include <tc/cli/FormatUtil.h>
#include <tc/ByteData.h>

void crypto_Md5Generator_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::Md5Generator] START" << std::endl;
	test_Constants();
	test_SingleUpdateCall();
	test_MultiUpdateCall();
	test_UtilFunc();

	test_NoInitNoUpdateDoHash();
	test_NoInitDoUpdateDoHash();
	test_DoInitNoUpdateDoHash();

	test_CallGetHashRepeatedly();
	std::cout << "[tc::crypto::Md5Generator] END" << std::endl;
}

void crypto_Md5Generator_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::Md5Generator] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check hash size
			static const size_t kExpectedHashSize = 16;
			if (tc::crypto::Md5Generator::kHashSize != kExpectedHashSize)
			{
				ss << "kHashSize had value " << std::dec << tc::crypto::Md5Generator::kHashSize << " (expected " << kExpectedHashSize << ")";
				throw tc::Exception(ss.str());
			}

			// check block size
			static const size_t kExpectedBlockSize = 64;
			if (tc::crypto::Md5Generator::kBlockSize != kExpectedBlockSize)
			{
				ss << "kBlockSize had value " << std::dec << tc::crypto::Md5Generator::kBlockSize << " (expected " << kExpectedBlockSize << ")";
				throw tc::Exception(ss.str());
			}

			// check ASN.1 OID data
			tc::ByteData kExpectedAsn1OidData = tc::cli::FormatUtil::hexStringToBytes("3020300C06082A864886F70D020505000410");
			if (tc::crypto::Md5Generator::kAsn1OidDataSize != kExpectedAsn1OidData.size())
			{
				ss << "kAsn1OidDataSize had value " << std::dec << tc::crypto::Md5Generator::kAsn1OidDataSize << " (expected " << kExpectedAsn1OidData.size() << ")";
				throw tc::Exception(ss.str());
			}
			if (tc::crypto::Md5Generator::kAsn1OidData.size() != kExpectedAsn1OidData.size())
			{
				ss << "kAsn1OidData.size() had value " << std::dec << tc::crypto::Md5Generator::kAsn1OidData.size() << " (expected " << kExpectedAsn1OidData.size() << ")";
				throw tc::Exception(ss.str());
			}
			if (memcmp(tc::crypto::Md5Generator::kAsn1OidData.data(), kExpectedAsn1OidData.data(), kExpectedAsn1OidData.size()) != 0)
			{
				ss << "kAsn1OidData.data() had data " << tc::cli::FormatUtil::formatBytesAsString(tc::crypto::Md5Generator::kAsn1OidData.data(), tc::crypto::Md5Generator::kAsn1OidData.size(), true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(kExpectedAsn1OidData, true, "");
				throw tc::Exception(ss.str());
			}

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void crypto_Md5Generator_TestClass::test_SingleUpdateCall()
{
	std::cout << "[tc::crypto::Md5Generator] test_SingleUpdateCall : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			struct TestCase
			{
				std::string test_name;
				std::string in_string;
				tc::ByteData out_hash;
			};

			// create tests
			std::vector<TestCase> tests = 
			{
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("D41D8CD98F00B204E9800998ECF8427E")},
				{ "short string (\"a\")", "a", tc::cli::FormatUtil::hexStringToBytes("0CC175B9C0F1B6A831C399E269772661")},
				{ "short string (\"abc\")", "abc", tc::cli::FormatUtil::hexStringToBytes("900150983CD24FB0D6963F7D28E17F72")},
				{ "long string (\"message digest\")", "message digest", tc::cli::FormatUtil::hexStringToBytes("F96B697D7CB7938D525A2F31AAF161D0")},
				{ "long string (alphabet)", "abcdefghijklmnopqrstuvwxyz", tc::cli::FormatUtil::hexStringToBytes("C3FCD3D76192E4007DFB496CCA67E13B")},
				{ "long string (alphanum)", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", tc::cli::FormatUtil::hexStringToBytes("D174AB98D277D9F5A5611C2C9F419D9F")},
				{ "long string (numerals)", "12345678901234567890123456789012345678901234567890123456789012345678901234567890", tc::cli::FormatUtil::hexStringToBytes("57EDF4A22BE3C955AC49DA2E2107B67A")},
			};

			tc::crypto::Md5Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Md5Generator::kHashSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				calc.initialize();
				calc.update((const byte_t*)test->in_string.c_str(), test->in_string.size());
				memset(hash.data(), 0xff, hash.size());
				calc.getHash(hash.data());
				if (memcmp(hash.data(), test->out_hash.data(), hash.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong hash value: " << tc::cli::FormatUtil::formatBytesAsString(hash, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_hash, true, "");
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
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void crypto_Md5Generator_TestClass::test_MultiUpdateCall()
{
	std::cout << "[tc::crypto::Md5Generator] test_MultiUpdateCall : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			struct TestCase
			{
				std::string test_name;
				std::string in_string;
				tc::ByteData out_hash;
			};

			// create tests
			std::vector<TestCase> tests = 
			{
				{ "short string (\"a\")", "a", tc::cli::FormatUtil::hexStringToBytes("0CC175B9C0F1B6A831C399E269772661")},
				{ "short string (\"abc\")", "abc", tc::cli::FormatUtil::hexStringToBytes("900150983CD24FB0D6963F7D28E17F72")},
				{ "long string (\"message digest\")", "message digest", tc::cli::FormatUtil::hexStringToBytes("F96B697D7CB7938D525A2F31AAF161D0")},
				{ "long string (alphabet)", "abcdefghijklmnopqrstuvwxyz", tc::cli::FormatUtil::hexStringToBytes("C3FCD3D76192E4007DFB496CCA67E13B")},
				{ "long string (alphanum)", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", tc::cli::FormatUtil::hexStringToBytes("D174AB98D277D9F5A5611C2C9F419D9F")},
				{ "long string (numerals)", "12345678901234567890123456789012345678901234567890123456789012345678901234567890", tc::cli::FormatUtil::hexStringToBytes("57EDF4A22BE3C955AC49DA2E2107B67A")},
			};

			tc::crypto::Md5Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Md5Generator::kHashSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				calc.initialize();

				// pick an offset to split the in_string at
				size_t offset = test->in_string.size() / 2;

				// update with first half
				calc.update((const byte_t*)test->in_string.c_str(), offset);

				// update with second half
				calc.update((const byte_t*)test->in_string.c_str() + offset, test->in_string.size() - offset);
				
				memset(hash.data(), 0xff, hash.size());
				calc.getHash(hash.data());
				if (memcmp(hash.data(), test->out_hash.data(), hash.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong hash value: " << tc::cli::FormatUtil::formatBytesAsString(hash, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_hash, true, "");
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
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void crypto_Md5Generator_TestClass::test_UtilFunc()
{
	std::cout << "[tc::crypto::Md5Generator] test_UtilFunc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			struct TestCase
			{
				std::string test_name;
				std::string in_string;
				tc::ByteData out_hash;
			};

			// create tests
			std::vector<TestCase> tests = 
			{
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("D41D8CD98F00B204E9800998ECF8427E")},
				{ "short string (\"a\")", "a", tc::cli::FormatUtil::hexStringToBytes("0CC175B9C0F1B6A831C399E269772661")},
				{ "short string (\"abc\")", "abc", tc::cli::FormatUtil::hexStringToBytes("900150983CD24FB0D6963F7D28E17F72")},
				{ "long string (\"message digest\")", "message digest", tc::cli::FormatUtil::hexStringToBytes("F96B697D7CB7938D525A2F31AAF161D0")},
				{ "long string (alphabet)", "abcdefghijklmnopqrstuvwxyz", tc::cli::FormatUtil::hexStringToBytes("C3FCD3D76192E4007DFB496CCA67E13B")},
				{ "long string (alphanum)", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", tc::cli::FormatUtil::hexStringToBytes("D174AB98D277D9F5A5611C2C9F419D9F")},
				{ "long string (numerals)", "12345678901234567890123456789012345678901234567890123456789012345678901234567890", tc::cli::FormatUtil::hexStringToBytes("57EDF4A22BE3C955AC49DA2E2107B67A")},
			};

			tc::ByteData hash = tc::ByteData(tc::crypto::Md5Generator::kHashSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				memset(hash.data(), 0xff, hash.size());
				tc::crypto::GenerateMd5Hash(hash.data(), (const byte_t*)test->in_string.c_str(), test->in_string.size());
				if (memcmp(hash.data(), test->out_hash.data(), hash.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong hash value: " << tc::cli::FormatUtil::formatBytesAsString(hash, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_hash, true, "");
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
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void crypto_Md5Generator_TestClass::test_NoInitNoUpdateDoHash()
{
	std::cout << "[tc::crypto::Md5Generator] test_NoInitNoUpdateDoHash : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			struct TestCase
			{
				std::string test_name;
				std::string in_string;
				tc::ByteData out_hash;
			};

			// create tests (when not initalized getHash() should not populate the hash buffer, and so the hash buffer should remain as what we set it)
			std::vector<TestCase> tests = 
			{
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "short string (\"a\")", "a", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "short string (\"abc\")", "abc", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "long string (\"message digest\")", "message digest", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "long string (alphabet)", "abcdefghijklmnopqrstuvwxyz", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "long string (alphanum)", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "long string (numerals)", "12345678901234567890123456789012345678901234567890123456789012345678901234567890", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
			};

			tc::crypto::Md5Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Md5Generator::kHashSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				//calc.initialize();
				//calc.update((const byte_t*)test->in_string.c_str(), test->in_string.size());
				memset(hash.data(), 0xff, hash.size());
				calc.getHash(hash.data());
				if (memcmp(hash.data(), test->out_hash.data(), hash.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong hash value: " << tc::cli::FormatUtil::formatBytesAsString(hash, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_hash, true, "");
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
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void crypto_Md5Generator_TestClass::test_NoInitDoUpdateDoHash()
{
	std::cout << "[tc::crypto::Md5Generator] test_NoInitDoUpdateDoHash : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			struct TestCase
			{
				std::string test_name;
				std::string in_string;
				tc::ByteData out_hash;
			};

			// create tests (when not initalized getHash() should not populate the hash buffer, and so the hash buffer should remain as what we set it)
			std::vector<TestCase> tests = 
			{
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "short string (\"a\")", "a", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "short string (\"abc\")", "abc", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "long string (\"message digest\")", "message digest", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "long string (alphabet)", "abcdefghijklmnopqrstuvwxyz", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "long string (alphanum)", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "long string (numerals)", "12345678901234567890123456789012345678901234567890123456789012345678901234567890", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
			};

			tc::crypto::Md5Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Md5Generator::kHashSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				//calc.initialize();
				calc.update((const byte_t*)test->in_string.c_str(), test->in_string.size());
				memset(hash.data(), 0xff, hash.size());
				calc.getHash(hash.data());
				if (memcmp(hash.data(), test->out_hash.data(), hash.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong hash value: " << tc::cli::FormatUtil::formatBytesAsString(hash, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_hash, true, "");
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
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void crypto_Md5Generator_TestClass::test_DoInitNoUpdateDoHash()
{
	std::cout << "[tc::crypto::Md5Generator] test_DoInitNoUpdateDoHash : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			struct TestCase
			{
				std::string test_name;
				std::string in_string;
				tc::ByteData out_hash;
			};

			// create tests (.getHash() should return the hash for an empty string if update is not called since they are logically the same thing)
			std::vector<TestCase> tests = 
			{
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("D41D8CD98F00B204E9800998ECF8427E")},
				{ "short string (\"a\")", "a", tc::cli::FormatUtil::hexStringToBytes("D41D8CD98F00B204E9800998ECF8427E")},
				{ "short string (\"abc\")", "abc", tc::cli::FormatUtil::hexStringToBytes("D41D8CD98F00B204E9800998ECF8427E")},
				{ "long string (\"message digest\")", "message digest", tc::cli::FormatUtil::hexStringToBytes("D41D8CD98F00B204E9800998ECF8427E")},
				{ "long string (alphabet)", "abcdefghijklmnopqrstuvwxyz", tc::cli::FormatUtil::hexStringToBytes("D41D8CD98F00B204E9800998ECF8427E")},
				{ "long string (alphanum)", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", tc::cli::FormatUtil::hexStringToBytes("D41D8CD98F00B204E9800998ECF8427E")},
				{ "long string (numerals)", "12345678901234567890123456789012345678901234567890123456789012345678901234567890", tc::cli::FormatUtil::hexStringToBytes("D41D8CD98F00B204E9800998ECF8427E")},
			};

			tc::crypto::Md5Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Md5Generator::kHashSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				calc.initialize();
				//calc.update((const byte_t*)test->in_string.c_str(), test->in_string.size());
				memset(hash.data(), 0xff, hash.size());
				calc.getHash(hash.data());
				if (memcmp(hash.data(), test->out_hash.data(), hash.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong hash value: " << tc::cli::FormatUtil::formatBytesAsString(hash, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_hash, true, "");
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
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void crypto_Md5Generator_TestClass::test_CallGetHashRepeatedly()
{
	std::cout << "[tc::crypto::Md5Generator] test_CallGetHashRepeatedly : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			struct TestCase
			{
				std::string test_name;
				std::string in_string;
				tc::ByteData out_hash;
			};

			// create tests
			std::vector<TestCase> tests = 
			{
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("D41D8CD98F00B204E9800998ECF8427E")},
				{ "short string (\"a\")", "a", tc::cli::FormatUtil::hexStringToBytes("0CC175B9C0F1B6A831C399E269772661")},
				{ "short string (\"abc\")", "abc", tc::cli::FormatUtil::hexStringToBytes("900150983CD24FB0D6963F7D28E17F72")},
				{ "long string (\"message digest\")", "message digest", tc::cli::FormatUtil::hexStringToBytes("F96B697D7CB7938D525A2F31AAF161D0")},
				{ "long string (alphabet)", "abcdefghijklmnopqrstuvwxyz", tc::cli::FormatUtil::hexStringToBytes("C3FCD3D76192E4007DFB496CCA67E13B")},
				{ "long string (alphanum)", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", tc::cli::FormatUtil::hexStringToBytes("D174AB98D277D9F5A5611C2C9F419D9F")},
				{ "long string (numerals)", "12345678901234567890123456789012345678901234567890123456789012345678901234567890", tc::cli::FormatUtil::hexStringToBytes("57EDF4A22BE3C955AC49DA2E2107B67A")},
			};

			tc::crypto::Md5Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Md5Generator::kHashSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				calc.initialize();
				calc.update((const byte_t*)test->in_string.c_str(), test->in_string.size());
				for (size_t i = 0; i < 100; i++)
				{
					// by resetting the hash here we can tell if it is updated each time
					memset(hash.data(), 0xff, hash.size());
					calc.getHash(hash.data());
					if (memcmp(hash.data(), test->out_hash.data(), hash.size()) != 0)
					{
						ss << "Test \"" << test->test_name << "\" Failed. Had wrong hash value: " << tc::cli::FormatUtil::formatBytesAsString(hash, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_hash, true, "");
						throw tc::Exception(ss.str());
					}
				}
			}

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}