#include <iostream>
#include <sstream>
#include <fstream>

#include <mbedtls/aes.h>

#include "crypto_Aes128EcbEncryptor_TestClass.h"

#include <tc/Exception.h>
#include <tc/crypto/Aes128EcbEncryptor.h>
#include <tc/cli/FormatUtil.h>

#include <tc/io/PaddingSource.h>

void crypto_Aes128EcbEncryptor_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::Aes128EcbEncryptor] START" << std::endl;
	test_Constants();
	test_UseClassEnc();
	test_UseClassDec();
	test_UseUtilFuncEnc();
	test_UseUtilFuncDec();

	test_DoesNothingWhenNotInit();
	test_InitializeThrowsExceptionOnBadInput();
	test_EncryptThrowsExceptionOnBadInput();
	test_DecryptThrowsExceptionOnBadInput();
	std::cout << "[tc::crypto::Aes128EcbEncryptor] END" << std::endl;
}

void crypto_Aes128EcbEncryptor_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::Aes128EcbEncryptor] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check block size
			static const size_t kExpectedBlockSize = 16;
			if (tc::crypto::Aes128EcbEncryptor::kBlockSize != kExpectedBlockSize)
			{
				ss << "kBlockSize had value " << std::dec << tc::crypto::Aes128EcbEncryptor::kBlockSize << " (expected " << kExpectedBlockSize << ")";
				throw tc::Exception(ss.str());
			}

			// check key size
			static const size_t kExpectedKeySize = 16;
			if (tc::crypto::Aes128EcbEncryptor::kKeySize != kExpectedKeySize)
			{
				ss << "kKeySize had value " << std::dec << tc::crypto::Aes128EcbEncryptor::kKeySize << " (expected " << kExpectedKeySize << ")";
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

void crypto_Aes128EcbEncryptor_TestClass::test_UseClassEnc()
{
	std::cout << "[tc::crypto::Aes128EcbEncryptor] test_UseClassEnc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes128EcbEncryptor cryptor;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->plaintext.size());	

				// initialize key
				cryptor.initialize(test->key.data(), test->key.size());
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// encrypt data
				cryptor.encrypt(data.data(), test->plaintext.data(), data.size());
				
				// validate cipher text
				if (memcmp(data.data(), test->ciphertext.data(), data.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed: " << tc::cli::FormatUtil::formatBytesAsString(data, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->ciphertext, true, "");
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

void crypto_Aes128EcbEncryptor_TestClass::test_UseClassDec()
{
	std::cout << "[tc::crypto::Aes128EcbEncryptor] test_UseClassDec : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes128EcbEncryptor cryptor;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->plaintext.size());
				
				// initialize key
				cryptor.initialize(test->key.data(), test->key.size());
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// decrypt data
				cryptor.decrypt(data.data(), test->ciphertext.data(), data.size());

				// test plain text			
				if (memcmp(data.data(), test->plaintext.data(), data.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed: " << tc::cli::FormatUtil::formatBytesAsString(data, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->plaintext, true, "");
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

void crypto_Aes128EcbEncryptor_TestClass::test_UseUtilFuncEnc()
{
	std::cout << "[tc::crypto::Aes128EcbEncryptor] test_UseUtilFuncEnc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->plaintext.size());	

				// clear data
				memset(data.data(), 0xff, data.size());

				// encrypt data
				tc::crypto::EncryptAes128Ecb(data.data(), test->plaintext.data(), data.size(), test->key.data(), test->key.size());
				
				// validate cipher text
				if (memcmp(data.data(), test->ciphertext.data(), data.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed: " << tc::cli::FormatUtil::formatBytesAsString(data, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->ciphertext, true, "");
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

void crypto_Aes128EcbEncryptor_TestClass::test_UseUtilFuncDec()
{
	std::cout << "[tc::crypto::Aes128EcbEncryptor] test_UseUtilFuncDec : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->plaintext.size());
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// decrypt data
				tc::crypto::DecryptAes128Ecb(data.data(), test->ciphertext.data(), data.size(), test->key.data(), test->key.size());

				// test plain text			
				if (memcmp(data.data(), test->plaintext.data(), data.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed: " << tc::cli::FormatUtil::formatBytesAsString(data, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->plaintext, true, "");
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

void crypto_Aes128EcbEncryptor_TestClass::test_DoesNothingWhenNotInit()
{
	std::cout << "[tc::crypto::Aes128EcbEncryptor] test_DoesNothingWhenNotInit : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			tc::crypto::Aes128EcbEncryptor cryptor;

			// create data
			tc::ByteData control_data = tc::io::PaddingSource(0xee, 0x20).pullData(0, 0x20);
			tc::ByteData data = tc::ByteData(control_data.data(), control_data.size());

			// try to decrypt without calling initialize()
			cryptor.decrypt(data.data(), data.data(), data.size());

			// test plain text			
			if (memcmp(data.data(), control_data.data(), data.size()) != 0)
			{
				ss << "Failed: decrypt() operated on data when not initialized";
				throw tc::Exception(ss.str());
			}

			// try to encrypt without calling initialize()
			cryptor.encrypt(data.data(), data.data(), data.size());

			// test plain text			
			if (memcmp(data.data(), control_data.data(), data.size()) != 0)
			{
				ss << "Failed: encrypt() operated on data when not initialized";
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

void crypto_Aes128EcbEncryptor_TestClass::test_InitializeThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes128EcbEncryptor] test_InitializeThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes128EcbEncryptor cryptor;

			try {
				cryptor.initialize(nullptr, tests[0].key.size());
				throw tc::Exception("Failed to throw ArgumentNullException where key==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key.data(), 0);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key_size==0");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key.data(), tc::crypto::Aes128EcbEncryptor::kKeySize-1);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key_size==tc::crypto::Aes128EcbEncryptor::kKeySize-1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key.data(), tc::crypto::Aes128EcbEncryptor::kKeySize+1);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key_size==tc::crypto::Aes128EcbEncryptor::kKeySize+1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
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

void crypto_Aes128EcbEncryptor_TestClass::test_EncryptThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes128EcbEncryptor] test_EncryptThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes128EcbEncryptor cryptor;

			cryptor.initialize(tests[0].key.data(), tests[0].key.size());

			tc::ByteData data = tc::ByteData(tests[0].plaintext.size());

			// reference encrypt call
			//cryptor.encrypt(data.data(), tests[0].plaintext.data(), data.size());

			try {
				cryptor.encrypt(nullptr, tests[0].plaintext.data(), data.size());
				throw tc::Exception("Failed to throw ArgumentNullException where dst==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.encrypt(data.data(), nullptr, data.size());
				throw tc::Exception("Failed to throw ArgumentNullException where src==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.encrypt(data.data(), tests[0].plaintext.data(), 0);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where size==0");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.encrypt(data.data(), tests[0].plaintext.data(), tc::crypto::Aes128EcbEncryptor::kBlockSize-1);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where size==tc::crypto::Aes128EcbEncryptor::kBlockSize-1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
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

void crypto_Aes128EcbEncryptor_TestClass::test_DecryptThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes128EcbEncryptor] test_DecryptThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes128EcbEncryptor cryptor;

			cryptor.initialize(tests[0].key.data(), tests[0].key.size());

			tc::ByteData data = tc::ByteData(tests[0].plaintext.size());

			// reference decrypt call
			//cryptor.decrypt(data.data(), tests[0].ciphertext.data(), data.size());

			try {
				cryptor.decrypt(nullptr, tests[0].ciphertext.data(), data.size());
				throw tc::Exception("Failed to throw ArgumentNullException where dst==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.decrypt(data.data(), nullptr, data.size());
				throw tc::Exception("Failed to throw ArgumentNullException where src==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.decrypt(data.data(), tests[0].ciphertext.data(), 0);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where size==0");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.decrypt(data.data(), tests[0].ciphertext.data(), tc::crypto::Aes128EcbEncryptor::kBlockSize-1);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where size==tc::crypto::Aes128EcbEncryptor::kBlockSize-1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
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

void crypto_Aes128EcbEncryptor_TestClass::util_Setup_TestCases(std::vector<crypto_Aes128EcbEncryptor_TestClass::TestCase>& test_cases)
{
	TestCase tmp;

	test_cases.clear();

	// Test vectors taken from NIST SP 800-38A
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("2b7e151628aed2a6abf7158809cf4f3c");

	tmp.test_name  = "Test 1";
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("6bc1bee22e409f96e93d7e117393172a");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("3ad77bb40d7a3660a89ecaf32466ef97");
	test_cases.push_back(tmp);

	tmp.test_name  = "Test 2";
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("ae2d8a571e03ac9c9eb76fac45af8e51");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("f5d3d58503b9699de785895a96fdbaaf");
	test_cases.push_back(tmp);

	tmp.test_name  = "Test 3";
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("30c81c46a35ce411e5fbc1191a0a52ef");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("43b1cd7f598ece23881b00e3ed030688");
	test_cases.push_back(tmp);

	tmp.test_name  = "Test 4";
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("f69f2445df4f9b17ad2b417be66c3710");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7b0c785e27e8ad3f8223207104725dd4");
	test_cases.push_back(tmp);

	tmp.test_name  = "Tests 1-4";
	tmp.plaintext = tc::ByteData(test_cases[0].plaintext.size() * 4, false);
	tmp.ciphertext = tc::ByteData(tmp.plaintext.size(), false);
	for (size_t i = 0; i < 4; i++)
	{
		memcpy(tmp.plaintext.data() + (i * 0x10), test_cases[i].plaintext.data(), test_cases[i].plaintext.size());
		memcpy(tmp.ciphertext.data() + (i * 0x10), test_cases[i].ciphertext.data(), test_cases[i].ciphertext.size());
	}
	test_cases.push_back(tmp);
}