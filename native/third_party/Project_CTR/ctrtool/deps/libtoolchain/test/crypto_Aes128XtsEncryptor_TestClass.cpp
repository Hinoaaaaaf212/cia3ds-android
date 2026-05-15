#include <iostream>
#include <sstream>
#include <fstream>

#include "crypto_Aes128XtsEncryptor_TestClass.h"

#include <tc/Exception.h>
#include <tc/crypto/Aes128XtsEncryptor.h>
#include <tc/cli/FormatUtil.h>

#include <tc/io/PaddingSource.h>

void crypto_Aes128XtsEncryptor_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::Aes128XtsEncryptor] START" << std::endl;
	test_Constants();
	test_UseClassEnc();
	test_UseClassDec();
	test_UseUtilFuncEnc();
	test_UseUtilFuncDec();

	test_DoesNothingWhenNotInit();
	test_InitializeThrowsExceptionOnBadInput();
	test_EncryptThrowsExceptionOnBadInput();
	test_DecryptThrowsExceptionOnBadInput();
	std::cout << "[tc::crypto::Aes128XtsEncryptor] END" << std::endl;
}

void crypto_Aes128XtsEncryptor_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::Aes128XtsEncryptor] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check block size
			static const size_t kExpectedBlockSize = 16;
			if (tc::crypto::Aes128XtsEncryptor::kBlockSize != kExpectedBlockSize)
			{
				ss << "kBlockSize had value " << std::dec << tc::crypto::Aes128XtsEncryptor::kBlockSize << " (expected " << kExpectedBlockSize << ")";
				throw tc::Exception(ss.str());
			}

			// check key size
			static const size_t kExpectedKeySize = 16;
			if (tc::crypto::Aes128XtsEncryptor::kKeySize != kExpectedKeySize)
			{
				ss << "kKeySize had value " << std::dec << tc::crypto::Aes128XtsEncryptor::kKeySize << " (expected " << kExpectedKeySize << ")";
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

void crypto_Aes128XtsEncryptor_TestClass::test_UseClassEnc()
{
	std::cout << "[tc::crypto::Aes128XtsEncryptor] test_UseClassEnc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_IEEE1619_2007_TestCases(tests);

			tc::crypto::Aes128XtsEncryptor cryptor;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->plaintext.size());	

				// initialize key
				cryptor.initialize(test->key1.data(), test->key1.size(), test->key2.data(), test->key2.size(), test->data_unit, true);
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// encrypt data
				cryptor.encrypt(data.data(), test->plaintext.data(), data.size(), test->data_unit_sequence_number);
				
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

void crypto_Aes128XtsEncryptor_TestClass::test_UseClassDec()
{
	std::cout << "[tc::crypto::Aes128XtsEncryptor] test_UseClassDec : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_IEEE1619_2007_TestCases(tests);

			tc::crypto::Aes128XtsEncryptor cryptor;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->plaintext.size());
				
				// initialize key
				cryptor.initialize(test->key1.data(), test->key1.size(), test->key2.data(), test->key2.size(), test->data_unit, true);
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// decrypt data
				cryptor.decrypt(data.data(), test->ciphertext.data(), data.size(), test->data_unit_sequence_number);

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

void crypto_Aes128XtsEncryptor_TestClass::test_UseUtilFuncEnc()
{
	std::cout << "[tc::crypto::Aes128XtsEncryptor] test_UseUtilFuncEnc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_IEEE1619_2007_TestCases(tests);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->plaintext.size());
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// encrypt data
				tc::crypto::EncryptAes128Xts(data.data(), test->plaintext.data(), data.size(), test->data_unit_sequence_number, test->key1.data(), test->key1.size(), test->key2.data(), test->key2.size(), test->data_unit, true);
				
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

void crypto_Aes128XtsEncryptor_TestClass::test_UseUtilFuncDec()
{
	std::cout << "[tc::crypto::Aes128XtsEncryptor] test_UseUtilFuncDec : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_IEEE1619_2007_TestCases(tests);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->plaintext.size());

				// clear data
				memset(data.data(), 0xff, data.size());

				// decrypt data
				tc::crypto::DecryptAes128Xts(data.data(), test->ciphertext.data(), data.size(), test->data_unit_sequence_number, test->key1.data(), test->key1.size(), test->key2.data(), test->key2.size(), test->data_unit, true);
				
				// validate plain text
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

void crypto_Aes128XtsEncryptor_TestClass::test_DoesNothingWhenNotInit()
{
	std::cout << "[tc::crypto::Aes128XtsEncryptor] test_DoesNothingWhenNotInit : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			tc::crypto::Aes128XtsEncryptor cryptor;

			// create data
			tc::ByteData control_data = tc::io::PaddingSource(0xee, 0x20).pullData(0, 0x20);
			tc::ByteData data = tc::ByteData(control_data.data(), control_data.size());

			if (cryptor.sector_size() != 0)
			{
				ss << "Failed: sector_size() reported a non-zero answer when not initialized";
				throw tc::Exception(ss.str());
			}

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

void crypto_Aes128XtsEncryptor_TestClass::test_InitializeThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes128XtsEncryptor] test_InitializeThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_IEEE1619_2007_TestCases(tests);

			tc::crypto::Aes128XtsEncryptor cryptor;

			// reference initialize call
			//cryptor.initialize(tests[0].key1.data(), tests[0].key1.size(), tests[0].key2.data(), tests[0].key2.size(), tests[0].data_unit, true);

			try {
				cryptor.initialize(nullptr, tests[0].key1.size(), tests[0].key2.data(), tests[0].key2.size(), tests[0].data_unit, true);
				throw tc::Exception("Failed to throw ArgumentNullException where key1==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key1.data(), 0, tests[0].key2.data(), tests[0].key2.size(), tests[0].data_unit, true);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key1_size==0");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key1.data(), tc::crypto::Aes128XtsEncryptor::kKeySize-1, tests[0].key2.data(), tests[0].key2.size(), tests[0].data_unit, true);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key1_size==tc::crypto::Aes128XtsEncryptor::kKeySize-1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key1.data(), tc::crypto::Aes128XtsEncryptor::kKeySize+1, tests[0].key2.data(), tests[0].key2.size(), tests[0].data_unit, true);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key1_size==tc::crypto::Aes128XtsEncryptor::kKeySize+1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key1.data(), tests[0].key1.size(), nullptr, tests[0].key2.size(), tests[0].data_unit, true);
				throw tc::Exception("Failed to throw ArgumentNullException where key2==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key1.data(), tests[0].key1.size(), tests[0].key2.data(), 0, tests[0].data_unit, true);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key2_size==0");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key1.data(), tests[0].key1.size(), tests[0].key2.data(), tc::crypto::Aes128XtsEncryptor::kKeySize-1, tests[0].data_unit, true);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key2_size==tc::crypto::Aes128XtsEncryptor::kKeySize-1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key1.data(), tests[0].key1.size(), tests[0].key2.data(), tc::crypto::Aes128XtsEncryptor::kKeySize+1, tests[0].data_unit, true);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key2_size==tc::crypto::Aes128XtsEncryptor::kKeySize+1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}


			try {
				cryptor.initialize(tests[0].key1.data(), tests[0].key1.size(), tests[0].key2.data(), tests[0].key2.size(), 0, true);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where sector_size==0");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key1.data(), tests[0].key1.size(), tests[0].key2.data(), tests[0].key2.size(), tc::crypto::Aes128XtsEncryptor::kBlockSize-1, true);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where sector_size==kBlockSize-1");
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

void crypto_Aes128XtsEncryptor_TestClass::test_EncryptThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes128XtsEncryptor] test_EncryptThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_IEEE1619_2007_TestCases(tests);

			tc::crypto::Aes128XtsEncryptor cryptor;

			cryptor.initialize(tests[0].key1.data(), tests[0].key1.size(), tests[0].key2.data(), tests[0].key2.size(), tests[0].data_unit, true);

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

			try {
				cryptor.encrypt(data.data(), tests[0].plaintext.data(), cryptor.sector_size()-1, 0);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where size==cryptor.sector_size()-1");
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

void crypto_Aes128XtsEncryptor_TestClass::test_DecryptThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes128XtsEncryptor] test_DecryptThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_IEEE1619_2007_TestCases(tests);

			tc::crypto::Aes128XtsEncryptor cryptor;

			cryptor.initialize(tests[0].key1.data(), tests[0].key1.size(), tests[0].key2.data(), tests[0].key2.size(), tests[0].data_unit, true);

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

			try {
				cryptor.decrypt(data.data(), tests[0].plaintext.data(), cryptor.sector_size()-1, 0);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where size==cryptor.sector_size()-1");
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

void crypto_Aes128XtsEncryptor_TestClass::util_Setup_IEEE1619_2007_TestCases(std::vector<crypto_Aes128XtsEncryptor_TestClass::TestCase>& test_cases)
{
	TestCase tmp;

	test_cases.clear();

	// XTS-AES applied for a data unit of 32 bytes, 32 bytes key material.
	tmp.data_unit = 32;

	tmp.test_name = "IEEE 1619-2007 Vector 1";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000000");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000000");
	tmp.data_unit_sequence_number = 0x00;
	tmp.plaintext = tc::cli::FormatUtil::hexStringToBytes("0000000000000000000000000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("917cf69ebd68b2ec9b9fe9a3eadda692cd43d2f59598ed858c02c2652fbf922e");
	test_cases.push_back(tmp);
	
	tmp.test_name = "IEEE 1619-2007 Vector 2";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("11111111111111111111111111111111");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("22222222222222222222222222222222");
	tmp.data_unit_sequence_number = 0x3333333333;
	tmp.plaintext = tc::cli::FormatUtil::hexStringToBytes("4444444444444444444444444444444444444444444444444444444444444444");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("c454185e6a16936e39334038acef838bfb186fff7480adc4289382ecd6d394f0");
	test_cases.push_back(tmp);
	
	tmp.test_name = "IEEE 1619-2007 Vector 3";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("fffefdfcfbfaf9f8f7f6f5f4f3f2f1f0");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("22222222222222222222222222222222");
	tmp.data_unit_sequence_number = 0x3333333333;
	tmp.plaintext = tc::cli::FormatUtil::hexStringToBytes("4444444444444444444444444444444444444444444444444444444444444444");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("af85336b597afc1a900b2eb21ec949d292df4c047e0b21532186a5971a227a89");
	test_cases.push_back(tmp);
	
	// XTS-AES-128 applied for a data unit of 512 bytes
	tmp.data_unit = 512;

	tmp.test_name = "IEEE 1619-2007 Vector 4";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("27182818284590452353602874713526");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("31415926535897932384626433832795");
	tmp.data_unit_sequence_number = 0x00;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("27a7479befa1d476489f308cd4cfa6e2a96e4bbe3208ff25287dd3819616e89cc78cf7f5e543445f8333d8fa7f56000005279fa5d8b5e4ad40e736ddb4d35412328063fd2aab53e5ea1e0a9f332500a5df9487d07a5c92cc512c8866c7e860ce93fdf166a24912b422976146ae20ce846bb7dc9ba94a767aaef20c0d61ad02655ea92dc4c4e41a8952c651d33174be51a10c421110e6d81588ede82103a252d8a750e8768defffed9122810aaeb99f9172af82b604dc4b8e51bcb08235a6f4341332e4ca60482a4ba1a03b3e65008fc5da76b70bf1690db4eae29c5f1badd03c5ccf2a55d705ddcd86d449511ceb7ec30bf12b1fa35b913f9f747a8afd1b130e94bff94effd01a91735ca1726acd0b197c4e5b03393697e126826fb6bbde8ecc1e08298516e2c9ed03ff3c1b7860f6de76d4cecd94c8119855ef5297ca67e9f3e7ff72b1e99785ca0a7e7720c5b36dc6d72cac9574c8cbbc2f801e23e56fd344b07f22154beba0f08ce8891e643ed995c94d9a69c9f1b5f499027a78572aeebd74d20cc39881c213ee770b1010e4bea718846977ae119f7a023ab58cca0ad752afe656bb3c17256a9f6e9bf19fdd5a38fc82bbe872c5539edb609ef4f79c203ebb140f2e583cb2ad15b4aa5b655016a8449277dbd477ef2c8d6c017db738b18deb4a427d1923ce3ff262735779a418f20a282df920147beabe421ee5319d0568");
	test_cases.push_back(tmp);

	tmp.test_name = "IEEE 1619-2007 Vector 5";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("27182818284590452353602874713526");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("31415926535897932384626433832795");
	tmp.data_unit_sequence_number = 0x01;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("27a7479befa1d476489f308cd4cfa6e2a96e4bbe3208ff25287dd3819616e89cc78cf7f5e543445f8333d8fa7f56000005279fa5d8b5e4ad40e736ddb4d35412328063fd2aab53e5ea1e0a9f332500a5df9487d07a5c92cc512c8866c7e860ce93fdf166a24912b422976146ae20ce846bb7dc9ba94a767aaef20c0d61ad02655ea92dc4c4e41a8952c651d33174be51a10c421110e6d81588ede82103a252d8a750e8768defffed9122810aaeb99f9172af82b604dc4b8e51bcb08235a6f4341332e4ca60482a4ba1a03b3e65008fc5da76b70bf1690db4eae29c5f1badd03c5ccf2a55d705ddcd86d449511ceb7ec30bf12b1fa35b913f9f747a8afd1b130e94bff94effd01a91735ca1726acd0b197c4e5b03393697e126826fb6bbde8ecc1e08298516e2c9ed03ff3c1b7860f6de76d4cecd94c8119855ef5297ca67e9f3e7ff72b1e99785ca0a7e7720c5b36dc6d72cac9574c8cbbc2f801e23e56fd344b07f22154beba0f08ce8891e643ed995c94d9a69c9f1b5f499027a78572aeebd74d20cc39881c213ee770b1010e4bea718846977ae119f7a023ab58cca0ad752afe656bb3c17256a9f6e9bf19fdd5a38fc82bbe872c5539edb609ef4f79c203ebb140f2e583cb2ad15b4aa5b655016a8449277dbd477ef2c8d6c017db738b18deb4a427d1923ce3ff262735779a418f20a282df920147beabe421ee5319d0568");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("264d3ca8512194fec312c8c9891f279fefdd608d0c027b60483a3fa811d65ee59d52d9e40ec5672d81532b38b6b089ce951f0f9c35590b8b978d175213f329bb1c2fd30f2f7f30492a61a532a79f51d36f5e31a7c9a12c286082ff7d2394d18f783e1a8e72c722caaaa52d8f065657d2631fd25bfd8e5baad6e527d763517501c68c5edc3cdd55435c532d7125c8614deed9adaa3acade5888b87bef641c4c994c8091b5bcd387f3963fb5bc37aa922fbfe3df4e5b915e6eb514717bdd2a74079a5073f5c4bfd46adf7d282e7a393a52579d11a028da4d9cd9c77124f9648ee383b1ac763930e7162a8d37f350b2f74b8472cf09902063c6b32e8c2d9290cefbd7346d1c779a0df50edcde4531da07b099c638e83a755944df2aef1aa31752fd323dcb710fb4bfbb9d22b925bc3577e1b8949e729a90bbafeacf7f7879e7b1147e28ba0bae940db795a61b15ecf4df8db07b824bb062802cc98a9545bb2aaeed77cb3fc6db15dcd7d80d7d5bc406c4970a3478ada8899b329198eb61c193fb6275aa8ca340344a75a862aebe92eee1ce032fd950b47d7704a3876923b4ad62844bf4a09c4dbe8b4397184b7471360c9564880aedddb9baa4af2e75394b08cd32ff479c57a07d3eab5d54de5f9738b8d27f27a9f0ab11799d7b7ffefb2704c95c6ad12c39f1e867a4b7b1d7818a4b753dfd2a89ccb45e001a03a867b187f225dd");
	test_cases.push_back(tmp);

	tmp.test_name = "IEEE 1619-2007 Vector 6";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("27182818284590452353602874713526");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("31415926535897932384626433832795");
	tmp.data_unit_sequence_number = 0x02;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("264d3ca8512194fec312c8c9891f279fefdd608d0c027b60483a3fa811d65ee59d52d9e40ec5672d81532b38b6b089ce951f0f9c35590b8b978d175213f329bb1c2fd30f2f7f30492a61a532a79f51d36f5e31a7c9a12c286082ff7d2394d18f783e1a8e72c722caaaa52d8f065657d2631fd25bfd8e5baad6e527d763517501c68c5edc3cdd55435c532d7125c8614deed9adaa3acade5888b87bef641c4c994c8091b5bcd387f3963fb5bc37aa922fbfe3df4e5b915e6eb514717bdd2a74079a5073f5c4bfd46adf7d282e7a393a52579d11a028da4d9cd9c77124f9648ee383b1ac763930e7162a8d37f350b2f74b8472cf09902063c6b32e8c2d9290cefbd7346d1c779a0df50edcde4531da07b099c638e83a755944df2aef1aa31752fd323dcb710fb4bfbb9d22b925bc3577e1b8949e729a90bbafeacf7f7879e7b1147e28ba0bae940db795a61b15ecf4df8db07b824bb062802cc98a9545bb2aaeed77cb3fc6db15dcd7d80d7d5bc406c4970a3478ada8899b329198eb61c193fb6275aa8ca340344a75a862aebe92eee1ce032fd950b47d7704a3876923b4ad62844bf4a09c4dbe8b4397184b7471360c9564880aedddb9baa4af2e75394b08cd32ff479c57a07d3eab5d54de5f9738b8d27f27a9f0ab11799d7b7ffefb2704c95c6ad12c39f1e867a4b7b1d7818a4b753dfd2a89ccb45e001a03a867b187f225dd");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("fa762a3680b76007928ed4a4f49a9456031b704782e65e16cecb54ed7d017b5e18abd67b338e81078f21edb7868d901ebe9c731a7c18b5e6dec1d6a72e078ac9a4262f860beefa14f4e821018272e411a951502b6e79066e84252c3346f3aa62344351a291d4bedc7a07618bdea2af63145cc7a4b8d4070691ae890cd65733e7946e9021a1dffc4c59f159425ee6d50ca9b135fa6162cea18a939838dc000fb386fad086acce5ac07cb2ece7fd580b00cfa5e98589631dc25e8e2a3daf2ffdec26531659912c9d8f7a15e5865ea8fb5816d6207052bd7128cd743c12c8118791a4736811935eb982a532349e31dd401e0b660a568cb1a4711f552f55ded59f1f15bf7196b3ca12a91e488ef59d64f3a02bf45239499ac6176ae321c4a211ec545365971c5d3f4f09d4eb139bfdf2073d33180b21002b65cc9865e76cb24cd92c874c24c18350399a936ab3637079295d76c417776b94efce3a0ef7206b15110519655c956cbd8b2489405ee2b09a6b6eebe0c53790a12a8998378b33a5b71159625f4ba49d2a2fdba59fbf0897bc7aabd8d707dc140a80f0f309f835d3da54ab584e501dfa0ee977fec543f74186a802b9a37adb3e8291eca04d66520d229e60401e7282bef486ae059aa70696e0e305d777140a7a883ecdcb69b9ff938e8a4231864c69ca2c2043bed007ff3e605e014bcf518138dc3a25c5e236171a2d01d6");
	test_cases.push_back(tmp);

	tmp.test_name = "IEEE 1619-2007 Vector 7";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("27182818284590452353602874713526");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("31415926535897932384626433832795");
	tmp.data_unit_sequence_number = 0xfd;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("8e41b78c390b5af9d758bb214a67e9f6bf7727b09ac6124084c37611398fa45daad94868600ed391fb1acd4857a95b466e62ef9f4b377244d1c152e7b30d731aad30c716d214b707aed99eb5b5e580b3e887cf7497465651d4b60e6042051da3693c3b78c14489543be8b6ad0ba629565bba202313ba7b0d0c94a3252b676f46cc02ce0f8a7d34c0ed229129673c1f61aed579d08a9203a25aac3a77e9db60267996db38df637356d9dcd1632e369939f2a29d89345c66e05066f1a3677aef18dea4113faeb629e46721a66d0a7e785d3e29af2594eb67dfa982affe0aac058f6e15864269b135418261fc3afb089472cf68c45dd7f231c6249ba0255e1e033833fc4d00a3fe02132d7bc3873614b8aee34273581ea0325c81f0270affa13641d052d36f0757d484014354d02d6883ca15c24d8c3956b1bd027bcf41f151fd8023c5340e5606f37e90fdb87c86fb4fa634b3718a30bace06a66eaf8f63c4aa3b637826a87fe8cfa44282e92cb1615af3a28e53bc74c7cba1a0977be9065d0c1a5dec6c54ae38d37f37aa35283e048e5530a85c4e7a29d7b92ec0c3169cdf2a805c7604bce60049b9fb7b8eaac10f51ae23794ceba68bb58112e293b9b692ca721b37c662f8574ed4dba6f88e170881c82cddc1034a0ca7e284bf0962b6b26292d836fa9f73c1ac770eef0f2d3a1eaf61d3e03555fd424eedd67e18a18094f888");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("d55f684f81f4426e9fde92a5ff02df2ac896af63962888a97910c1379e20b0a3b1db613fb7fe2e07004329ea5c22bfd33e3dbe4cf58cc608c2c26c19a2e2fe22f98732c2b5cb844cc6c0702d91e1d50fc4382a7eba5635cd602432a2306ac4ce82f8d70c8d9bc15f918fe71e74c622d5cf71178bf6e0b9cc9f2b41dd8dbe441c41cd0c73a6dc47a348f6702f9d0e9b1b1431e948e299b9ec2272ab2c5f0c7be86affa5dec87a0bee81d3d50007edaa2bcfccb35605155ff36ed8edd4a40dcd4b243acd11b2b987bdbfaf91a7cac27e9c5aea525ee53de7b2d3332c8644402b823e94a7db26276d2d23aa07180f76b4fd29b9c0823099c9d62c519880aee7e9697617c1497d47bf3e571950311421b6b734d38b0db91eb85331b91ea9f61530f54512a5a52a4bad589eb69781d537f23297bb459bdad2948a29e1550bf4787e0be95bb173cf5fab17dab7a13a052a63453d97ccec1a321954886b7a1299faaeecae35c6eaaca753b041b5e5f093bf83397fd21dd6b3012066fcc058cc32c3b09d7562dee29509b5839392c9ff05f51f3166aaac4ac5f238038a3045e6f72e48ef0fe8bc675e82c318a268e43970271bf119b81bf6a982746554f84e72b9f00280a320a08142923c23c883423ff949827f29bbacdc1ccdb04938ce6098c95ba6b32528f4ef78eed778b2e122ddfd1cbdd11d1c0a6783e011fc536d63d053260637");
	test_cases.push_back(tmp);
	
	tmp.test_name = "IEEE 1619-2007 Vector 8";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("27182818284590452353602874713526");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("31415926535897932384626433832795");
	tmp.data_unit_sequence_number = 0xfe;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("d55f684f81f4426e9fde92a5ff02df2ac896af63962888a97910c1379e20b0a3b1db613fb7fe2e07004329ea5c22bfd33e3dbe4cf58cc608c2c26c19a2e2fe22f98732c2b5cb844cc6c0702d91e1d50fc4382a7eba5635cd602432a2306ac4ce82f8d70c8d9bc15f918fe71e74c622d5cf71178bf6e0b9cc9f2b41dd8dbe441c41cd0c73a6dc47a348f6702f9d0e9b1b1431e948e299b9ec2272ab2c5f0c7be86affa5dec87a0bee81d3d50007edaa2bcfccb35605155ff36ed8edd4a40dcd4b243acd11b2b987bdbfaf91a7cac27e9c5aea525ee53de7b2d3332c8644402b823e94a7db26276d2d23aa07180f76b4fd29b9c0823099c9d62c519880aee7e9697617c1497d47bf3e571950311421b6b734d38b0db91eb85331b91ea9f61530f54512a5a52a4bad589eb69781d537f23297bb459bdad2948a29e1550bf4787e0be95bb173cf5fab17dab7a13a052a63453d97ccec1a321954886b7a1299faaeecae35c6eaaca753b041b5e5f093bf83397fd21dd6b3012066fcc058cc32c3b09d7562dee29509b5839392c9ff05f51f3166aaac4ac5f238038a3045e6f72e48ef0fe8bc675e82c318a268e43970271bf119b81bf6a982746554f84e72b9f00280a320a08142923c23c883423ff949827f29bbacdc1ccdb04938ce6098c95ba6b32528f4ef78eed778b2e122ddfd1cbdd11d1c0a6783e011fc536d63d053260637");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("72efc1ebfe1ee25975a6eb3aa8589dda2b261f1c85bdab442a9e5b2dd1d7c3957a16fc08e526d4b1223f1b1232a11af274c3d70dac57f83e0983c498f1a6f1aecb021c3e70085a1e527f1ce41ee5911a82020161529cd82773762daf5459de94a0a82adae7e1703c808543c29ed6fb32d9e004327c1355180c995a07741493a09c21ba01a387882da4f62534b87bb15d60d197201c0fd3bf30c1500a3ecfecdd66d8721f90bcc4c17ee925c61b0a03727a9c0d5f5ca462fbfa0af1c2513a9d9d4b5345bd27a5f6e653f751693e6b6a2b8ead57d511e00e58c45b7b8d005af79288f5c7c22fd4f1bf7a898b03a5634c6a1ae3f9fae5de4f296a2896b23e7ed43ed14fa5a2803f4d28f0d3ffcf24757677aebdb47bb388378708948a8d4126ed1839e0da29a537a8c198b3c66ab00712dd261674bf45a73d67f76914f830ca014b65596f27e4cf62de66125a5566df9975155628b400fbfb3a29040ed50faffdbb18aece7c5c44693260aab386c0a37b11b114f1c415aebb653be468179428d43a4d8bc3ec38813eca30a13cf1bb18d524f1992d44d8b1a42ea30b22e6c95b199d8d182f8840b09d059585c31ad691fa0619ff038aca2c39a943421157361717c49d322028a74648113bd8c9d7ec77cf3c89c1ec8718ceff8516d96b34c3c614f10699c9abc4ed0411506223bea16af35c883accdbe1104eef0cfdb54e12fb230a");
	test_cases.push_back(tmp);

	tmp.test_name = "IEEE 1619-2007 Vector 9";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("27182818284590452353602874713526");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("31415926535897932384626433832795");
	tmp.data_unit_sequence_number = 0xff;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("72efc1ebfe1ee25975a6eb3aa8589dda2b261f1c85bdab442a9e5b2dd1d7c3957a16fc08e526d4b1223f1b1232a11af274c3d70dac57f83e0983c498f1a6f1aecb021c3e70085a1e527f1ce41ee5911a82020161529cd82773762daf5459de94a0a82adae7e1703c808543c29ed6fb32d9e004327c1355180c995a07741493a09c21ba01a387882da4f62534b87bb15d60d197201c0fd3bf30c1500a3ecfecdd66d8721f90bcc4c17ee925c61b0a03727a9c0d5f5ca462fbfa0af1c2513a9d9d4b5345bd27a5f6e653f751693e6b6a2b8ead57d511e00e58c45b7b8d005af79288f5c7c22fd4f1bf7a898b03a5634c6a1ae3f9fae5de4f296a2896b23e7ed43ed14fa5a2803f4d28f0d3ffcf24757677aebdb47bb388378708948a8d4126ed1839e0da29a537a8c198b3c66ab00712dd261674bf45a73d67f76914f830ca014b65596f27e4cf62de66125a5566df9975155628b400fbfb3a29040ed50faffdbb18aece7c5c44693260aab386c0a37b11b114f1c415aebb653be468179428d43a4d8bc3ec38813eca30a13cf1bb18d524f1992d44d8b1a42ea30b22e6c95b199d8d182f8840b09d059585c31ad691fa0619ff038aca2c39a943421157361717c49d322028a74648113bd8c9d7ec77cf3c89c1ec8718ceff8516d96b34c3c614f10699c9abc4ed0411506223bea16af35c883accdbe1104eef0cfdb54e12fb230a");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("3260ae8dad1f4a32c5cafe3ab0eb95549d461a67ceb9e5aa2d3afb62dece0553193ba50c75be251e08d1d08f1088576c7efdfaaf3f459559571e12511753b07af073f35da06af0ce0bbf6b8f5ccc5cea500ec1b211bd51f63b606bf6528796ca12173ba39b8935ee44ccce646f90a45bf9ccc567f0ace13dc2d53ebeedc81f58b2e41179dddf0d5a5c42f5d8506c1a5d2f8f59f3ea873cbcd0eec19acbf325423bd3dcb8c2b1bf1d1eaed0eba7f0698e4314fbeb2f1566d1b9253008cbccf45a2b0d9c5c9c21474f4076e02be26050b99dee4fd68a4cf890e496e4fcae7b70f94ea5a9062da0daeba1993d2ccd1dd3c244b8428801495a58b216547e7e847c46d1d756377b6242d2e5fb83bf752b54e0df71e889f3a2bb0f4c10805bf3c590376e3c24e22ff57f7fa965577375325cea5d920db94b9c336b455f6e894c01866fe9fbb8c8d3f70a2957285f6dfb5dcd8cbf54782f8fe7766d4723819913ac773421e3a31095866bad22c86a6036b2518b2059b4229d18c8c2ccbdf906c6cc6e82464ee57bddb0bebcb1dc645325bfb3e665ef7251082c88ebb1cf203bd779fdd38675713c8daadd17e1cabee432b09787b6ddf3304e38b731b45df5df51b78fcfb3d32466028d0ba36555e7e11ab0ee0666061d1645d962444bc47a38188930a84b4d561395c73c087021927ca638b7afc8a8679ccb84c26555440ec7f10445cd");
	test_cases.push_back(tmp);

	tmp.test_name = "Custom Test, Vectors 4-6";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("27182818284590452353602874713526");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("31415926535897932384626433832795");
	tmp.data_unit_sequence_number = 0x00;
	tmp.plaintext = tc::ByteData(tmp.data_unit * 3);
	tmp.ciphertext = tc::ByteData(tmp.data_unit * 3);
	for (size_t idx = 0; idx < 3; idx++)
	{
		memcpy(tmp.plaintext.data() + idx * tmp.data_unit, test_cases[3 + idx].plaintext.data(), tmp.data_unit);
		memcpy(tmp.ciphertext.data() + idx * tmp.data_unit, test_cases[3 + idx].ciphertext.data(), tmp.data_unit);
	}
	test_cases.push_back(tmp);

	tmp.test_name = "Custom Test, Vectors 7-9";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("27182818284590452353602874713526");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("31415926535897932384626433832795");
	tmp.data_unit_sequence_number = 0xfd;
	tmp.plaintext = tc::ByteData(tmp.data_unit * 3);
	tmp.ciphertext = tc::ByteData(tmp.data_unit * 3);
	for (size_t idx = 0; idx < 3; idx++)
	{
		memcpy(tmp.plaintext.data() + idx * tmp.data_unit, test_cases[6 + idx].plaintext.data(), tmp.data_unit);
		memcpy(tmp.ciphertext.data() + idx * tmp.data_unit, test_cases[6 + idx].ciphertext.data(), tmp.data_unit);
	}
	test_cases.push_back(tmp);

	// XTS-AES-128 applied for a data unit that is not a multiple of 16 bytes
	// please note the ciphertext for these have been modified, as even the reference implementation cannot be validated against these test vectors
	tmp.test_name = "IEEE 1619-2007 Vector 15";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("fffefdfcfbfaf9f8f7f6f5f4f3f2f1f0");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("bfbebdbcbbbab9b8b7b6b5b4b3b2b1b0");
	tmp.data_unit_sequence_number = 0x9a78563412;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("000102030405060708090a0b0c0d0e0f10");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("641610679DCBF92E505C41333FB06C2A95"); // original 6c1625db4671522d3d7599601de7ca09ed
	tmp.data_unit = tmp.plaintext.size();
	test_cases.push_back(tmp);

	tmp.test_name = "IEEE 1619-2007 Vector 16";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("fffefdfcfbfaf9f8f7f6f5f4f3f2f1f0");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("bfbebdbcbbbab9b8b7b6b5b4b3b2b1b0");
	tmp.data_unit_sequence_number = 0x9a78563412;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("000102030405060708090a0b0c0d0e0f1011");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("223A725CBCD4DC647B9A9826D54C99C895C8"); // original d069444b7a7e0cab09e24447d24deb1fedbf
	tmp.data_unit = tmp.plaintext.size();
	test_cases.push_back(tmp);

	tmp.test_name = "IEEE 1619-2007 Vector 17";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("fffefdfcfbfaf9f8f7f6f5f4f3f2f1f0");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("bfbebdbcbbbab9b8b7b6b5b4b3b2b1b0");
	tmp.data_unit_sequence_number = 0x9a78563412;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("000102030405060708090a0b0c0d0e0f101112");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("0D39809A65C1D55501960B671D4B8B6B95C871"); // original e5df1351c0544ba1350b3363cd8ef4beedbf9d
	tmp.data_unit = tmp.plaintext.size();
	test_cases.push_back(tmp);

	tmp.test_name = "IEEE 1619-2007 Vector 18";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("fffefdfcfbfaf9f8f7f6f5f4f3f2f1f0");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("bfbebdbcbbbab9b8b7b6b5b4b3b2b1b0");
	tmp.data_unit_sequence_number = 0x9a78563412;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("000102030405060708090a0b0c0d0e0f10111213");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A8BA0048D75084603EB8423A09B7BF7595C871F6"); // original 9d84c813f719aa2c7be3f66171c7c5c2edbf9dac
	tmp.data_unit = tmp.plaintext.size();
	test_cases.push_back(tmp);

	tmp.test_name = "IEEE 1619-2007 Vector 19";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("e0e1e2e3e4e5e6e7e8e9eaebecedeeef");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("c0c1c2c3c4c5c6c7c8c9cacbcccdcecf");
	tmp.data_unit_sequence_number = 0x21436587a9;
	tmp.plaintext  = tc::cli::FormatUtil::hexStringToBytes("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C4E60104E27AED4DEFF63B72B054AC82CC0175A5B8AEEEAA017EC12249AA641B36438DAB0589E8903BF23327127C12362E7D6864522C44538E4E979D97393BA67EAB768517E2C70B98035FC1BBEBFED48A9FF017EB3E04DC8E19AAD6BE04C23A6675726A4388C8A297FEF753BF71E9EA07DD354D42DC2393888A401188554A53315EB9A0624AE44E5B674B3607B73EE0E5FEB44F5BE7178EF54CBD460B1D6A2E93923F6B63210B06D74367BBB02884639AF3B958CACC041618A7940A983F8438123348F3D87F254377152302D821ECE64588F8BB1AF85CF934BD49C703186E48624772C9802228484249E887EBFD7440514130D8D38C2B1219241A42630BDFBD4135FAC6BEC92462D3EA9BEB95C797D23A8C04799E3EA2BA733C5E00718649E7AF0FDD6EAA5BFD5DF7F3FA9953A3B5266806709D17DDD0A0F5B7535BC9F986C109A848EF8C3A45F9033056817CE08DE8019EA103E28836B82D08C5D1FDEEC254E508BF253F6B90963FE43D7D8B8D66D30419C4733A32DA1505DE5DFD7A976E7852455DD454327EE8A1CB71D40F392A89EEE266A8F42772BB519F4044902D939E9716734F622F46E3C48F31D09E7859A14B54693F9EEB14FC021DC5B66589CA3FE16B7BC7166A3686CC869730656AE76285A518B290745E852C6AC626EB0DA25DFF404B83F001D6D23A65D91F3F38A097DA03A59B275B4F5A5480C9608E12F93B"); // original 38b45812ef43a05bd957e545907e223b954ab4aaf088303ad910eadf14b42be68b2461149d8c8ba85f992be970bc621f1b06573f63e867bf5875acafa04e42ccbd7bd3c2a0fb1fff791ec5ec36c66ae4ac1e806d81fbf709dbe29e471fad38549c8e66f5345d7c1eb94f405d1ec785cc6f6a68f6254dd8339f9d84057e01a17741990482999516b5611a38f41bb6478e6f173f320805dd71b1932fc333cb9ee39936beea9ad96fa10fb4112b901734ddad40bc1878995f8e11aee7d141a2f5d48b7a4e1e7f0b2c04830e69a4fd1378411c2f287edf48c6c4e5c247a19680f7fe41cefbd49b582106e3616cbbe4dfb2344b2ae9519391f3e0fb4922254b1d6d2d19c6d4d537b3a26f3bcc51588b32f3eca0829b6a5ac72578fb814fb43cf80d64a233e3f997a3f02683342f2b33d25b492536b93becb2f5e1a8b82f5b883342729e8ae09d16938841a21a97fb543eea3bbff59f13c1a18449e398701c1ad51648346cbc04c27bb2da3b93a1372ccae548fb53bee476f9e9c91773b1bb19828394d55d3e1a20ed69113a860b6829ffa847224604435070221b257e8dff783615d2cae4803a93aa4334ab482a0afac9c0aeda70b45a481df5dec5df8cc0f423c77a5fd46cd312021d4b438862419a791be03bb4d97c0e59578542531ba466a83baf92cefc151b5cc1611a167893819b63fb8a6b18e86de60290fa72b797b0ce59f3
	tmp.data_unit = tmp.plaintext.size();
	test_cases.push_back(tmp);
}