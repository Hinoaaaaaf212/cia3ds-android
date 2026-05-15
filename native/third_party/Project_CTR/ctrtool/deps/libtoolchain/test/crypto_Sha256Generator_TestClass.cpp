#include <iostream>
#include <sstream>
#include <fstream>

#include "crypto_Sha256Generator_TestClass.h"

#include <tc/Exception.h>
#include <tc/crypto/Sha256Generator.h>
#include <tc/cli/FormatUtil.h>
#include <tc/ByteData.h>

void crypto_Sha256Generator_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::Sha256Generator] START" << std::endl;
	test_Constants();
	test_SingleUpdateCall();
	test_MultiUpdateCall();
	test_UtilFunc();

	test_NoInitNoUpdateDoHash();
	test_NoInitDoUpdateDoHash();
	test_DoInitNoUpdateDoHash();

	test_CallGetHashRepeatedly();
	std::cout << "[tc::crypto::Sha256Generator] END" << std::endl;
}

void crypto_Sha256Generator_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::Sha256Generator] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check hash size
			static const size_t kExpectedHashSize = 32;
			if (tc::crypto::Sha256Generator::kHashSize != kExpectedHashSize)
			{
				ss << "kHashSize had value " << std::dec << tc::crypto::Sha256Generator::kHashSize << " (expected " << kExpectedHashSize << ")";
				throw tc::Exception(ss.str());
			}

			// check block size
			static const size_t kExpectedBlockSize = 64;
			if (tc::crypto::Sha256Generator::kBlockSize != kExpectedBlockSize)
			{
				ss << "kBlockSize had value " << std::dec << tc::crypto::Sha256Generator::kBlockSize << " (expected " << kExpectedBlockSize << ")";
				throw tc::Exception(ss.str());
			}

			// check ASN.1 OID data
			tc::ByteData kExpectedAsn1OidData = tc::cli::FormatUtil::hexStringToBytes("3031300D060960864801650304020105000420");
			if (tc::crypto::Sha256Generator::kAsn1OidDataSize != kExpectedAsn1OidData.size())
			{
				ss << "kAsn1OidDataSize had value " << std::dec << tc::crypto::Sha256Generator::kAsn1OidDataSize << " (expected " << kExpectedAsn1OidData.size() << ")";
				throw tc::Exception(ss.str());
			}
			if (tc::crypto::Sha256Generator::kAsn1OidData.size() != kExpectedAsn1OidData.size())
			{
				ss << "kAsn1OidData.size() had value " << std::dec << tc::crypto::Sha256Generator::kAsn1OidData.size() << " (expected " << kExpectedAsn1OidData.size() << ")";
				throw tc::Exception(ss.str());
			}
			if (memcmp(tc::crypto::Sha256Generator::kAsn1OidData.data(), kExpectedAsn1OidData.data(), kExpectedAsn1OidData.size()) != 0)
			{
				ss << "kAsn1OidData.data() had data " << tc::cli::FormatUtil::formatBytesAsString(tc::crypto::Sha256Generator::kAsn1OidData.data(), tc::crypto::Sha256Generator::kAsn1OidData.size(), true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(kExpectedAsn1OidData, true, "");
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

void crypto_Sha256Generator_TestClass::test_SingleUpdateCall()
{
	std::cout << "[tc::crypto::Sha256Generator] test_SingleUpdateCall : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD")},
				{ "long string" ,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", tc::cli::FormatUtil::hexStringToBytes("248D6A61D20638B8E5C026930C3E6039A33CE45964FF2167F6ECEDD419DB06C1")},
			};

			tc::crypto::Sha256Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha256Generator::kHashSize);

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

void crypto_Sha256Generator_TestClass::test_MultiUpdateCall()
{
	std::cout << "[tc::crypto::Sha256Generator] test_MultiUpdateCall : " << std::flush;
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
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD")},
				{ "long string" ,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", tc::cli::FormatUtil::hexStringToBytes("248D6A61D20638B8E5C026930C3E6039A33CE45964FF2167F6ECEDD419DB06C1")},
			};

			tc::crypto::Sha256Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha256Generator::kHashSize);

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

void crypto_Sha256Generator_TestClass::test_UtilFunc()
{
	std::cout << "[tc::crypto::Sha256Generator] test_UtilFunc : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD")},
				{ "long string" ,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", tc::cli::FormatUtil::hexStringToBytes("248D6A61D20638B8E5C026930C3E6039A33CE45964FF2167F6ECEDD419DB06C1")},
			};

			tc::ByteData hash = tc::ByteData(tc::crypto::Sha256Generator::kHashSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				memset(hash.data(), 0xff, hash.size());
				tc::crypto::GenerateSha256Hash(hash.data(), (const byte_t*)test->in_string.c_str(), test->in_string.size());
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

void crypto_Sha256Generator_TestClass::test_NoInitNoUpdateDoHash()
{
	std::cout << "[tc::crypto::Sha256Generator] test_NoInitNoUpdateDoHash : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "long string" ,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
			};

			tc::crypto::Sha256Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha256Generator::kHashSize);

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

void crypto_Sha256Generator_TestClass::test_NoInitDoUpdateDoHash()
{
	std::cout << "[tc::crypto::Sha256Generator] test_NoInitDoUpdateDoHash : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "long string" ,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
			};

			tc::crypto::Sha256Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha256Generator::kHashSize);

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

void crypto_Sha256Generator_TestClass::test_DoInitNoUpdateDoHash()
{
	std::cout << "[tc::crypto::Sha256Generator] test_DoInitNoUpdateDoHash : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855")},
				{ "long string" ,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", tc::cli::FormatUtil::hexStringToBytes("E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855")},
			};

			tc::crypto::Sha256Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha256Generator::kHashSize);

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

void crypto_Sha256Generator_TestClass::test_CallGetHashRepeatedly()
{
	std::cout << "[tc::crypto::Sha256Generator] test_CallGetHashRepeatedly : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD")},
				{ "long string" ,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", tc::cli::FormatUtil::hexStringToBytes("248D6A61D20638B8E5C026930C3E6039A33CE45964FF2167F6ECEDD419DB06C1")},
			};

			tc::crypto::Sha256Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha256Generator::kHashSize);

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