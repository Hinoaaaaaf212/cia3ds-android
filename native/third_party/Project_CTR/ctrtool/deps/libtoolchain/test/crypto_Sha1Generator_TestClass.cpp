#include <iostream>
#include <sstream>
#include <fstream>

#include "crypto_Sha1Generator_TestClass.h"

#include <tc/Exception.h>
#include <tc/crypto/Sha1Generator.h>
#include <tc/cli/FormatUtil.h>
#include <tc/ByteData.h>

void crypto_Sha1Generator_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::Sha1Generator] START" << std::endl;
	test_Constants();
	test_SingleUpdateCall();
	test_MultiUpdateCall();
	test_UtilFunc();

	test_NoInitNoUpdateDoHash();
	test_NoInitDoUpdateDoHash();
	test_DoInitNoUpdateDoHash();

	test_CallGetHashRepeatedly();
	std::cout << "[tc::crypto::Sha1Generator] END" << std::endl;
}

void crypto_Sha1Generator_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::Sha1Generator] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check hash size
			static const size_t kExpectedHashSize = 20;
			if (tc::crypto::Sha1Generator::kHashSize != kExpectedHashSize)
			{
				ss << "kHashSize had value " << std::dec << tc::crypto::Sha1Generator::kHashSize << " (expected " << kExpectedHashSize << ")";
				throw tc::Exception(ss.str());
			}

			// check block size
			static const size_t kExpectedBlockSize = 64;
			if (tc::crypto::Sha1Generator::kBlockSize != kExpectedBlockSize)
			{
				ss << "kBlockSize had value " << std::dec << tc::crypto::Sha1Generator::kBlockSize << " (expected " << kExpectedBlockSize << ")";
				throw tc::Exception(ss.str());
			}

			// check ASN.1 OID data
			tc::ByteData kExpectedAsn1OidData = tc::cli::FormatUtil::hexStringToBytes("3021300906052B0E03021A05000414");
			if (tc::crypto::Sha1Generator::kAsn1OidDataSize != kExpectedAsn1OidData.size())
			{
				ss << "kAsn1OidDataSize had value " << std::dec << tc::crypto::Sha1Generator::kAsn1OidDataSize << " (expected " << kExpectedAsn1OidData.size() << ")";
				throw tc::Exception(ss.str());
			}
			if (tc::crypto::Sha1Generator::kAsn1OidData.size() != kExpectedAsn1OidData.size())
			{
				ss << "kAsn1OidData.size() had value " << std::dec << tc::crypto::Sha1Generator::kAsn1OidData.size() << " (expected " << kExpectedAsn1OidData.size() << ")";
				throw tc::Exception(ss.str());
			}
			if (memcmp(tc::crypto::Sha1Generator::kAsn1OidData.data(), kExpectedAsn1OidData.data(), kExpectedAsn1OidData.size()) != 0)
			{
				ss << "kAsn1OidData.data() had data " << tc::cli::FormatUtil::formatBytesAsString(tc::crypto::Sha1Generator::kAsn1OidData.data(), tc::crypto::Sha1Generator::kAsn1OidData.size(), true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(kExpectedAsn1OidData, true, "");
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

void crypto_Sha1Generator_TestClass::test_SingleUpdateCall()
{
	std::cout << "[tc::crypto::Sha1Generator] test_SingleUpdateCall : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("DA39A3EE5E6B4B0D3255BFEF95601890AFD80709")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("A9993E364706816ABA3E25717850C26C9CD0D89D")},
				{ "long string" ,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", tc::cli::FormatUtil::hexStringToBytes("84983E441C3BD26EBAAE4AA1F95129E5E54670F1")},
			};

			tc::crypto::Sha1Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha1Generator::kHashSize);

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

void crypto_Sha1Generator_TestClass::test_MultiUpdateCall()
{
	std::cout << "[tc::crypto::Sha1Generator] test_MultiUpdateCall : " << std::flush;
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
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("A9993E364706816ABA3E25717850C26C9CD0D89D")},
				{ "long string" ,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", tc::cli::FormatUtil::hexStringToBytes("84983E441C3BD26EBAAE4AA1F95129E5E54670F1")},
			};

			tc::crypto::Sha1Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha1Generator::kHashSize);

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

void crypto_Sha1Generator_TestClass::test_UtilFunc()
{
	std::cout << "[tc::crypto::Sha1Generator] test_UtilFunc : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("DA39A3EE5E6B4B0D3255BFEF95601890AFD80709")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("A9993E364706816ABA3E25717850C26C9CD0D89D")},
				{ "long string" ,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", tc::cli::FormatUtil::hexStringToBytes("84983E441C3BD26EBAAE4AA1F95129E5E54670F1")},
			};

			tc::ByteData hash = tc::ByteData(tc::crypto::Sha1Generator::kHashSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				memset(hash.data(), 0xff, hash.size());
				tc::crypto::GenerateSha1Hash(hash.data(), (const byte_t*)test->in_string.c_str(), test->in_string.size());
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

void crypto_Sha1Generator_TestClass::test_NoInitNoUpdateDoHash()
{
	std::cout << "[tc::crypto::Sha1Generator] test_NoInitNoUpdateDoHash : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "long string" ,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
			};

			tc::crypto::Sha1Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha1Generator::kHashSize);

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

void crypto_Sha1Generator_TestClass::test_NoInitDoUpdateDoHash()
{
	std::cout << "[tc::crypto::Sha1Generator] test_NoInitDoUpdateDoHash : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "long string" ,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
			};

			tc::crypto::Sha1Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha1Generator::kHashSize);

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

void crypto_Sha1Generator_TestClass::test_DoInitNoUpdateDoHash()
{
	std::cout << "[tc::crypto::Sha1Generator] test_DoInitNoUpdateDoHash : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("DA39A3EE5E6B4B0D3255BFEF95601890AFD80709")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("DA39A3EE5E6B4B0D3255BFEF95601890AFD80709")},
				{ "long string" ,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", tc::cli::FormatUtil::hexStringToBytes("DA39A3EE5E6B4B0D3255BFEF95601890AFD80709")},
			};

			tc::crypto::Sha1Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha1Generator::kHashSize);

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

void crypto_Sha1Generator_TestClass::test_CallGetHashRepeatedly()
{
	std::cout << "[tc::crypto::Sha1Generator] test_CallGetHashRepeatedly : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("DA39A3EE5E6B4B0D3255BFEF95601890AFD80709")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("A9993E364706816ABA3E25717850C26C9CD0D89D")},
				{ "long string" ,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", tc::cli::FormatUtil::hexStringToBytes("84983E441C3BD26EBAAE4AA1F95129E5E54670F1")},
			};

			tc::crypto::Sha1Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha1Generator::kHashSize);

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