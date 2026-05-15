#include <iostream>
#include <sstream>
#include <fstream>

#include "crypto_Sha512Generator_TestClass.h"

#include <tc/Exception.h>
#include <tc/crypto/Sha512Generator.h>
#include <tc/cli/FormatUtil.h>
#include <tc/ByteData.h>

void crypto_Sha512Generator_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::Sha512Generator] START" << std::endl;
	test_Constants();
	test_SingleUpdateCall();
	test_MultiUpdateCall();
	test_UtilFunc();

	test_NoInitNoUpdateDoHash();
	test_NoInitDoUpdateDoHash();
	test_DoInitNoUpdateDoHash();

	test_CallGetHashRepeatedly();
	std::cout << "[tc::crypto::Sha512Generator] END" << std::endl;
}

void crypto_Sha512Generator_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::Sha512Generator] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check hash size
			static const size_t kExpectedHashSize = 64;
			if (tc::crypto::Sha512Generator::kHashSize != kExpectedHashSize)
			{
				ss << "kHashSize had value " << std::dec << tc::crypto::Sha512Generator::kHashSize << " (expected " << kExpectedHashSize << ")";
				throw tc::Exception(ss.str());
			}

			// check block size
			static const size_t kExpectedBlockSize = 128;
			if (tc::crypto::Sha512Generator::kBlockSize != kExpectedBlockSize)
			{
				ss << "kBlockSize had value " << std::dec << tc::crypto::Sha512Generator::kBlockSize << " (expected " << kExpectedBlockSize << ")";
				throw tc::Exception(ss.str());
			}

			// check ASN.1 OID data
			tc::ByteData kExpectedAsn1OidData = tc::cli::FormatUtil::hexStringToBytes("3051300D060960864801650304020305000440");
			if (tc::crypto::Sha512Generator::kAsn1OidDataSize != kExpectedAsn1OidData.size())
			{
				ss << "kAsn1OidDataSize had value " << std::dec << tc::crypto::Sha512Generator::kAsn1OidDataSize << " (expected " << kExpectedAsn1OidData.size() << ")";
				throw tc::Exception(ss.str());
			}
			if (tc::crypto::Sha512Generator::kAsn1OidData.size() != kExpectedAsn1OidData.size())
			{
				ss << "kAsn1OidData.size() had value " << std::dec << tc::crypto::Sha512Generator::kAsn1OidData.size() << " (expected " << kExpectedAsn1OidData.size() << ")";
				throw tc::Exception(ss.str());
			}
			if (memcmp(tc::crypto::Sha512Generator::kAsn1OidData.data(), kExpectedAsn1OidData.data(), kExpectedAsn1OidData.size()) != 0)
			{
				ss << "kAsn1OidData.data() had data " << tc::cli::FormatUtil::formatBytesAsString(tc::crypto::Sha512Generator::kAsn1OidData.data(), tc::crypto::Sha512Generator::kAsn1OidData.size(), true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(kExpectedAsn1OidData, true, "");
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

void crypto_Sha512Generator_TestClass::test_SingleUpdateCall()
{
	std::cout << "[tc::crypto::Sha512Generator] test_SingleUpdateCall : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("DDAF35A193617ABACC417349AE20413112E6FA4E89A97EA20A9EEEE64B55D39A2192992A274FC1A836BA3C23A3FEEBBD454D4423643CE80E2A9AC94FA54CA49F")},
				{ "long string" ,"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu", tc::cli::FormatUtil::hexStringToBytes("8E959B75DAE313DA8CF4F72814FC143F8F7779C6EB9F7FA17299AEADB6889018501D289E4900F7E4331B99DEC4B5433AC7D329EEB6DD26545E96E55B874BE909")},
			};

			tc::crypto::Sha512Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha512Generator::kHashSize);

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

void crypto_Sha512Generator_TestClass::test_MultiUpdateCall()
{
	std::cout << "[tc::crypto::Sha512Generator] test_MultiUpdateCall : " << std::flush;
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
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("DDAF35A193617ABACC417349AE20413112E6FA4E89A97EA20A9EEEE64B55D39A2192992A274FC1A836BA3C23A3FEEBBD454D4423643CE80E2A9AC94FA54CA49F")},
				{ "long string" ,"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu", tc::cli::FormatUtil::hexStringToBytes("8E959B75DAE313DA8CF4F72814FC143F8F7779C6EB9F7FA17299AEADB6889018501D289E4900F7E4331B99DEC4B5433AC7D329EEB6DD26545E96E55B874BE909")},
			};

			tc::crypto::Sha512Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha512Generator::kHashSize);

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

void crypto_Sha512Generator_TestClass::test_UtilFunc()
{
	std::cout << "[tc::crypto::Sha512Generator] test_UtilFunc : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("DDAF35A193617ABACC417349AE20413112E6FA4E89A97EA20A9EEEE64B55D39A2192992A274FC1A836BA3C23A3FEEBBD454D4423643CE80E2A9AC94FA54CA49F")},
				{ "long string" ,"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu", tc::cli::FormatUtil::hexStringToBytes("8E959B75DAE313DA8CF4F72814FC143F8F7779C6EB9F7FA17299AEADB6889018501D289E4900F7E4331B99DEC4B5433AC7D329EEB6DD26545E96E55B874BE909")},
			};

			tc::ByteData hash = tc::ByteData(tc::crypto::Sha512Generator::kHashSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				memset(hash.data(), 0xff, hash.size());
				tc::crypto::GenerateSha512Hash(hash.data(), (const byte_t*)test->in_string.c_str(), test->in_string.size());
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

void crypto_Sha512Generator_TestClass::test_NoInitNoUpdateDoHash()
{
	std::cout << "[tc::crypto::Sha512Generator] test_NoInitNoUpdateDoHash : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "long string" ,"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
			};

			tc::crypto::Sha512Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha512Generator::kHashSize);

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

void crypto_Sha512Generator_TestClass::test_NoInitDoUpdateDoHash()
{
	std::cout << "[tc::crypto::Sha512Generator] test_NoInitDoUpdateDoHash : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
				{ "long string" ,"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu", tc::cli::FormatUtil::hexStringToBytes("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
			};

			tc::crypto::Sha512Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha512Generator::kHashSize);

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

void crypto_Sha512Generator_TestClass::test_DoInitNoUpdateDoHash()
{
	std::cout << "[tc::crypto::Sha512Generator] test_DoInitNoUpdateDoHash : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E")},
				{ "long string" ,"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu", tc::cli::FormatUtil::hexStringToBytes("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E")},
			};

			tc::crypto::Sha512Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha512Generator::kHashSize);

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

void crypto_Sha512Generator_TestClass::test_CallGetHashRepeatedly()
{
	std::cout << "[tc::crypto::Sha512Generator] test_CallGetHashRepeatedly : " << std::flush;
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
				{ "empty string", "", tc::cli::FormatUtil::hexStringToBytes("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E")},
				{ "short string", "abc", tc::cli::FormatUtil::hexStringToBytes("DDAF35A193617ABACC417349AE20413112E6FA4E89A97EA20A9EEEE64B55D39A2192992A274FC1A836BA3C23A3FEEBBD454D4423643CE80E2A9AC94FA54CA49F")},
				{ "long string" ,"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu", tc::cli::FormatUtil::hexStringToBytes("8E959B75DAE313DA8CF4F72814FC143F8F7779C6EB9F7FA17299AEADB6889018501D289E4900F7E4331B99DEC4B5433AC7D329EEB6DD26545E96E55B874BE909")},
			};

			tc::crypto::Sha512Generator calc;
			tc::ByteData hash = tc::ByteData(tc::crypto::Sha512Generator::kHashSize);

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