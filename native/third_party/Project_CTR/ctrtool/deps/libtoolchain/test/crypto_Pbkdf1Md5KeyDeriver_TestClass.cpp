#include <iostream>
#include <sstream>
#include <fstream>

#include "crypto_Pbkdf1Md5KeyDeriver_TestClass.h"
#include "PbkdfUtil.h"

#include <tc/crypto/Pbkdf1Md5KeyDeriver.h>
#include <tc/cli/FormatUtil.h>

void crypto_Pbkdf1Md5KeyDeriver_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::Pbkdf1Md5KeyDeriver] START" << std::endl;
	test_Constants();
	test_ConfirmTestVector_Class();
	test_ConfirmTestVector_UtilFunc();
	test_WillThrowExceptionOnZeroRounds();
	if (std::numeric_limits<size_t>::max() < std::numeric_limits<uint64_t>::max())
	{
		std::cout << "[tc::crypto::Pbkdf1Md5KeyDeriver] test_WillThrowExceptionOnTooLargeDkSize : SKIP (Cannot perform this test non 64bit systems)" << std::endl;
	}
	else
	{
		test_WillThrowExceptionOnTooLargeDkSize();
	}
	test_GetBytesWithoutInitDoesNothing();
	std::cout << "[tc::crypto::Pbkdf1Md5KeyDeriver] END" << std::endl;
}

void crypto_Pbkdf1Md5KeyDeriver_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::Pbkdf1Md5KeyDeriver] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check max derivable size
			static const uint64_t kExpectedMaxDerivableSize = tc::crypto::Md5Generator::kHashSize;
			if (tc::crypto::Pbkdf1Md5KeyDeriver::kMaxDerivableSize != kExpectedMaxDerivableSize)
			{
				ss << "kMaxDerivableSize had value " << std::dec << tc::crypto::Pbkdf1Md5KeyDeriver::kMaxDerivableSize << " (expected " << kExpectedMaxDerivableSize << ")";
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

void crypto_Pbkdf1Md5KeyDeriver_TestClass::test_ConfirmTestVector_Class()
{
	std::cout << "[tc::crypto::Pbkdf1Md5KeyDeriver] test_ConfirmTestVector_Class : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<PbkdfUtil::TestVector> tests;
			PbkdfUtil::generatePbkdf1TestVectors_Custom(tests, PbkdfUtil::MD5);

			tc::crypto::Pbkdf1Md5KeyDeriver keydev;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{	
				keydev.initialize((const byte_t*)test->in_password.c_str(), test->in_password.size(), (const byte_t*)test->in_salt.c_str(), test->in_salt.size(), test->in_rounds);
				
				auto dk = tc::ByteData(test->in_dk_len);
				keydev.getBytes(dk.data(), dk.size());

				if (memcmp(dk.data(), test->out_dk.data(), dk.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong DK: " << tc::cli::FormatUtil::formatBytesAsString(dk, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_dk, true, "");
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

void crypto_Pbkdf1Md5KeyDeriver_TestClass::test_ConfirmTestVector_UtilFunc()
{
	std::cout << "[tc::crypto::Pbkdf1Md5KeyDeriver] test_ConfirmTestVector_UtilFunc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<PbkdfUtil::TestVector> tests;
			PbkdfUtil::generatePbkdf1TestVectors_Custom(tests, PbkdfUtil::MD5);


			for (auto test = tests.begin(); test != tests.end(); test++)
			{				
				auto dk = tc::ByteData(test->in_dk_len);

				tc::crypto::DeriveKeyPbkdf1Md5(dk.data(), dk.size(), (const byte_t*)test->in_password.c_str(), test->in_password.size(), (const byte_t*)test->in_salt.c_str(), test->in_salt.size(), test->in_rounds);

				if (memcmp(dk.data(), test->out_dk.data(), dk.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong DK: " << tc::cli::FormatUtil::formatBytesAsString(dk, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_dk, true, "");
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

void crypto_Pbkdf1Md5KeyDeriver_TestClass::test_WillThrowExceptionOnZeroRounds()
{
	std::cout << "[tc::crypto::Pbkdf1Md5KeyDeriver] test_WillThrowExceptionOnZeroRounds : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<PbkdfUtil::TestVector> tests;
			PbkdfUtil::generatePbkdf1TestVectors_Custom(tests, PbkdfUtil::MD5);

			auto dk = tc::ByteData(tests[0].in_dk_len);

			try 
			{
				tc::crypto::DeriveKeyPbkdf1Md5(dk.data(), dk.size(), (const byte_t*)tests[0].in_password.c_str(), tests[0].in_password.size(), (const byte_t*)tests[0].in_salt.c_str(), tests[0].in_salt.size(), 0);

				throw tc::Exception("DeriveKeyPbkdf1Md5() Did not throw exception");
			} catch (const tc::crypto::CryptoException&)
			{
				// do nothing
			}

			try 
			{
				tc::crypto::Pbkdf1Md5KeyDeriver keydev;

				keydev.initialize((const byte_t*)tests[0].in_password.c_str(), tests[0].in_password.size(), (const byte_t*)tests[0].in_salt.c_str(), tests[0].in_salt.size(), 0);

				throw tc::Exception("Pbkdf1Md5KeyDeriver::initialize() Did not throw exception");
			} catch (const tc::crypto::CryptoException&)
			{
				// do nothing
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

void crypto_Pbkdf1Md5KeyDeriver_TestClass::test_WillThrowExceptionOnTooLargeDkSize()
{
	std::cout << "[tc::crypto::Pbkdf1Md5KeyDeriver] test_WillThrowExceptionOnTooLargeDkSize : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<PbkdfUtil::TestVector> tests;
			PbkdfUtil::generatePbkdf1TestVectors_Custom(tests, PbkdfUtil::MD5);

			// derive a key larger than the maximum derivable size
			auto dk = tc::ByteData(tc::crypto::Pbkdf1Md5KeyDeriver::kMaxDerivableSize + 1);

			try 
			{
				tc::crypto::DeriveKeyPbkdf1Md5(dk.data(), dk.size(), (const byte_t*)tests[0].in_password.c_str(), tests[0].in_password.size(), (const byte_t*)tests[0].in_salt.c_str(), tests[0].in_salt.size(), tests[0].in_rounds);

				throw tc::Exception("DeriveKeyPbkdf1Md5() Did not throw exception");
			} catch (const tc::crypto::CryptoException&)
			{
				// do nothing
			}

			try 
			{
				tc::crypto::Pbkdf1Md5KeyDeriver keydev;

				keydev.initialize((const byte_t*)tests[0].in_password.c_str(), tests[0].in_password.size(), (const byte_t*)tests[0].in_salt.c_str(), tests[0].in_salt.size(), tests[0].in_rounds);
				keydev.getBytes(dk.data(), dk.size());

				throw tc::Exception("Pbkdf1Md5KeyDeriver::getBytes() Did not throw exception");
			} catch (const tc::crypto::CryptoException&)
			{
				// do nothing
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

void crypto_Pbkdf1Md5KeyDeriver_TestClass::test_GetBytesWithoutInitDoesNothing()
{
	std::cout << "[tc::crypto::Pbkdf1Md5KeyDeriver] test_GetBytesWithoutInitDoesNothing : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<PbkdfUtil::TestVector> tests;
			PbkdfUtil::generatePbkdf1TestVectors_Custom(tests, PbkdfUtil::MD5);

			auto dk = tc::ByteData(tests[0].in_dk_len);
			memset(dk.data(), 0xab, dk.size());

			tc::crypto::Pbkdf1Md5KeyDeriver keydev;
			keydev.getBytes(dk.data(), dk.size());

			byte_t cmp = 1;
			for (size_t i = 0; i < dk.size(); i++)
			{
				cmp &= dk[i] == 0xab;
			}

			if (cmp != 1)
			{
				throw tc::Exception("getBytes() operated inspite of not being initialized.");
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