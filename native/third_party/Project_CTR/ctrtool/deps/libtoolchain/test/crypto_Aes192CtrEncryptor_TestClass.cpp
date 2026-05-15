#include <iostream>
#include <sstream>
#include <fstream>

#include <mbedtls/aes.h>

#include "crypto_Aes192CtrEncryptor_TestClass.h"

#include <tc/Exception.h>
#include <tc/crypto/Aes192CtrEncryptor.h>
#include <tc/cli/FormatUtil.h>

#include <tc/io/PaddingSource.h>

void crypto_Aes192CtrEncryptor_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::Aes192CtrEncryptor] START" << std::endl;
	test_Constants();
	test_UseClassEnc();
	test_UseClassDec();
	test_UseUtilFuncEnc();
	test_UseUtilFuncDec();

	test_DoesNothingWhenNotInit();
	test_InitializeThrowsExceptionOnBadInput();
	test_EncryptThrowsExceptionOnBadInput();
	test_DecryptThrowsExceptionOnBadInput();
	std::cout << "[tc::crypto::Aes192CtrEncryptor] END" << std::endl;
}

void crypto_Aes192CtrEncryptor_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::Aes192CtrEncryptor] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check block size
			static const size_t kExpectedBlockSize = 16;
			if (tc::crypto::Aes192CtrEncryptor::kBlockSize != kExpectedBlockSize)
			{
				ss << "kBlockSize had value " << std::dec << tc::crypto::Aes192CtrEncryptor::kBlockSize << " (expected " << kExpectedBlockSize << ")";
				throw tc::Exception(ss.str());
			}

			// check key size
			static const size_t kExpectedKeySize = 24;
			if (tc::crypto::Aes192CtrEncryptor::kKeySize != kExpectedKeySize)
			{
				ss << "kKeySize had value " << std::dec << tc::crypto::Aes192CtrEncryptor::kKeySize << " (expected " << kExpectedKeySize << ")";
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

void crypto_Aes192CtrEncryptor_TestClass::test_UseClassEnc()
{
	std::cout << "[tc::crypto::Aes192CtrEncryptor] test_UseClassEnc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes192CtrEncryptor cryptor;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->plaintext.size());	

				// initialize key
				cryptor.initialize(test->key.data(), test->key.size(), test->iv.data(), test->iv.size());
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// encrypt data
				cryptor.encrypt(data.data(), test->plaintext.data(), data.size(), test->block_number);
				
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

void crypto_Aes192CtrEncryptor_TestClass::test_UseClassDec()
{
	std::cout << "[tc::crypto::Aes192CtrEncryptor] test_UseClassDec : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes192CtrEncryptor cryptor;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->plaintext.size());
				
				// initialize key
				cryptor.initialize(test->key.data(), test->key.size(), test->iv.data(), test->iv.size());
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// decrypt data
				cryptor.decrypt(data.data(), test->ciphertext.data(), data.size(), test->block_number);

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

void crypto_Aes192CtrEncryptor_TestClass::test_UseUtilFuncEnc()
{
	std::cout << "[tc::crypto::Aes192CtrEncryptor] test_UseUtilFuncEnc : " << std::flush;
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
				tc::crypto::EncryptAes192Ctr(data.data(), test->plaintext.data(), data.size(), test->block_number, test->key.data(), test->key.size(), test->iv.data(), test->iv.size());
				
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

void crypto_Aes192CtrEncryptor_TestClass::test_UseUtilFuncDec()
{
	std::cout << "[tc::crypto::Aes192CtrEncryptor] test_UseUtilFuncDec : " << std::flush;
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
				tc::crypto::DecryptAes192Ctr(data.data(), test->ciphertext.data(), data.size(), test->block_number, test->key.data(), test->key.size(), test->iv.data(), test->iv.size());

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

void crypto_Aes192CtrEncryptor_TestClass::test_DoesNothingWhenNotInit()
{
	std::cout << "[tc::crypto::Aes192CtrEncryptor] test_DoesNothingWhenNotInit : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			tc::crypto::Aes192CtrEncryptor cryptor;

			// create data
			tc::ByteData control_data = tc::io::PaddingSource(0xee, 0x20).pullData(0, 0x20);
			tc::ByteData data = tc::ByteData(control_data.data(), control_data.size());

			// try to decrypt without calling initialize()
			cryptor.decrypt(data.data(), data.data(), data.size(), 0);

			// test plain text			
			if (memcmp(data.data(), control_data.data(), data.size()) != 0)
			{
				ss << "Failed: decrypt() operated on data when not initialized";
				throw tc::Exception(ss.str());
			}

			// try to encrypt without calling initialize()
			cryptor.encrypt(data.data(), data.data(), data.size(), 0);

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

void crypto_Aes192CtrEncryptor_TestClass::test_InitializeThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes192CtrEncryptor] test_InitializeThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes192CtrEncryptor cryptor;

			try {
				cryptor.initialize(nullptr, tests[0].key.size(), tests[0].iv.data(), tests[0].iv.size());
				throw tc::Exception("Failed to throw ArgumentNullException where key==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key.data(), 0, tests[0].iv.data(), tests[0].iv.size());
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key_size==0");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key.data(), tc::crypto::Aes192CtrEncryptor::kKeySize-1, tests[0].iv.data(), tests[0].iv.size());
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key_size==tc::crypto::Aes192CtrEncryptor::kKeySize-1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key.data(), tc::crypto::Aes192CtrEncryptor::kKeySize+1, tests[0].iv.data(), tests[0].iv.size());
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key_size==tc::crypto::Aes192CtrEncryptor::kKeySize+1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key.data(), tests[0].key.size(), nullptr, tests[0].iv.size());
				throw tc::Exception("Failed to throw ArgumentNullException where iv==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key.data(), tests[0].key.size(), tests[0].iv.data(), 0);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where iv_size==0");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key.data(), tests[0].key.size(), tests[0].iv.data(), tc::crypto::Aes192CtrEncryptor::kBlockSize-1);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where iv_size==kBlockSize-1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key.data(), tests[0].key.size(), tests[0].iv.data(), tc::crypto::Aes192CtrEncryptor::kBlockSize+1);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where iv_size==kBlockSize+1");
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

void crypto_Aes192CtrEncryptor_TestClass::test_EncryptThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes192CtrEncryptor] test_EncryptThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes192CtrEncryptor cryptor;

			cryptor.initialize(tests[0].key.data(), tests[0].key.size(), tests[0].iv.data(), tests[0].iv.size());

			tc::ByteData data = tc::ByteData(tests[0].plaintext.size());

			// reference encrypt call
			//cryptor.encrypt(data.data(), tests[0].plaintext.data(), data.size(), 0);

			try {
				cryptor.encrypt(nullptr, tests[0].plaintext.data(), data.size(), 0);
				throw tc::Exception("Failed to throw ArgumentNullException where dst==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.encrypt(data.data(), nullptr, data.size(), 0);
				throw tc::Exception("Failed to throw ArgumentNullException where src==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.encrypt(data.data(), tests[0].plaintext.data(), 0, 0);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where size==0");
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

void crypto_Aes192CtrEncryptor_TestClass::test_DecryptThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes192CtrEncryptor] test_DecryptThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes192CtrEncryptor cryptor;

			cryptor.initialize(tests[0].key.data(), tests[0].key.size(), tests[0].iv.data(), tests[0].iv.size());

			tc::ByteData data = tc::ByteData(tests[0].plaintext.size());

			// reference decrypt call
			//cryptor.decrypt(data.data(), tests[0].ciphertext.data(), data.size(), 0);

			try {
				cryptor.decrypt(nullptr, tests[0].ciphertext.data(), data.size(), 0);
				throw tc::Exception("Failed to throw ArgumentNullException where dst==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.decrypt(data.data(), nullptr, data.size(), 0);
				throw tc::Exception("Failed to throw ArgumentNullException where src==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.decrypt(data.data(), tests[0].ciphertext.data(), 0, 0);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where size==0");
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

void crypto_Aes192CtrEncryptor_TestClass::util_Setup_TestCases(std::vector<crypto_Aes192CtrEncryptor_TestClass::TestCase>& test_cases)
{
	TestCase tmp;

	test_cases.clear();

	// Test vectors taken from NIST SP 800-38A
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b");
	tmp.iv  = tc::cli::FormatUtil::hexStringToBytes("f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");

	tmp.test_name  = "Test 1";
	tmp.block_number = 0;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("6bc1bee22e409f96e93d7e117393172a");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("1abc932417521ca24f2b0459fe7e6e0b");
	test_cases.push_back(tmp);

	tmp.test_name  = "Test 2";
	tmp.block_number = 1;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("ae2d8a571e03ac9c9eb76fac45af8e51");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("090339ec0aa6faefd5ccc2c6f4ce8e94");
	test_cases.push_back(tmp);

	tmp.test_name  = "Test 3";
	tmp.block_number = 2;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("30c81c46a35ce411e5fbc1191a0a52ef");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("1e36b26bd1ebc670d1bd1d665620abf7");
	test_cases.push_back(tmp);

	tmp.test_name  = "Test 4";
	tmp.block_number = 3;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("f69f2445df4f9b17ad2b417be66c3710");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("4f78a7f6d29809585a97daec58c6b050");
	test_cases.push_back(tmp);

	tmp.test_name  = "Tests 1-4";
	tmp.block_number = 0;
	tmp.plaintext  = tc::ByteData(test_cases[0].plaintext.size() * 4, false);
	tmp.ciphertext = tc::ByteData(tmp.plaintext.size(), false);
	for (size_t i = 0; i < 4; i++)
	{
		memcpy(tmp.plaintext.data() + (i * 0x10), test_cases[i].plaintext.data(), test_cases[i].plaintext.size());
		memcpy(tmp.ciphertext.data() + (i * 0x10), test_cases[i].ciphertext.data(), test_cases[i].ciphertext.size());
	}
	test_cases.push_back(tmp);
}