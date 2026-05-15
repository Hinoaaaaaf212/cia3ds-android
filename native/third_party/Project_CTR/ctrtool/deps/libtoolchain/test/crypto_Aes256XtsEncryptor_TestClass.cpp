#include <iostream>
#include <sstream>
#include <fstream>

#include "crypto_Aes256XtsEncryptor_TestClass.h"

#include <tc/Exception.h>
#include <tc/crypto/Aes256XtsEncryptor.h>
#include <tc/cli/FormatUtil.h>

#include <tc/io/PaddingSource.h>

void crypto_Aes256XtsEncryptor_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::Aes256XtsEncryptor] START" << std::endl;
	test_Constants();
	test_UseClassEnc();
	test_UseClassDec();
	test_UseUtilFuncEnc();
	test_UseUtilFuncDec();

	test_DoesNothingWhenNotInit();
	test_InitializeThrowsExceptionOnBadInput();
	test_EncryptThrowsExceptionOnBadInput();
	test_DecryptThrowsExceptionOnBadInput();
	std::cout << "[tc::crypto::Aes256XtsEncryptor] END" << std::endl;
}

void crypto_Aes256XtsEncryptor_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::Aes256XtsEncryptor] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check block size
			static const size_t kExpectedBlockSize = 16;
			if (tc::crypto::Aes256XtsEncryptor::kBlockSize != kExpectedBlockSize)
			{
				ss << "kBlockSize had value " << std::dec << tc::crypto::Aes256XtsEncryptor::kBlockSize << " (expected " << kExpectedBlockSize << ")";
				throw tc::Exception(ss.str());
			}

			// check key size
			static const size_t kExpectedKeySize = 32;
			if (tc::crypto::Aes256XtsEncryptor::kKeySize != kExpectedKeySize)
			{
				ss << "kKeySize had value " << std::dec << tc::crypto::Aes256XtsEncryptor::kKeySize << " (expected " << kExpectedKeySize << ")";
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

void crypto_Aes256XtsEncryptor_TestClass::test_UseClassEnc()
{
	std::cout << "[tc::crypto::Aes256XtsEncryptor] test_UseClassEnc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_IEEE1619_2007_TestCases(tests);

			tc::crypto::Aes256XtsEncryptor cryptor;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->plaintext.size());
				
				// initialize key
				cryptor.initialize(test->key1.data(), test->key1.size(), test->key2.data(), test->key2.size(), test->data_unit, true);
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// test encryption
				cryptor.encrypt(data.data(), test->plaintext.data(), data.size(), test->data_unit_sequence_number);				
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

void crypto_Aes256XtsEncryptor_TestClass::test_UseClassDec()
{
	std::cout << "[tc::crypto::Aes256XtsEncryptor] test_UseClassDec : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_IEEE1619_2007_TestCases(tests);

			tc::crypto::Aes256XtsEncryptor cryptor;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->plaintext.size());
				
				// initialize key
				cryptor.initialize(test->key1.data(), test->key1.size(), test->key2.data(), test->key2.size(), test->data_unit, true);
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// test decryption
				cryptor.decrypt(data.data(), test->ciphertext.data(), data.size(), test->data_unit_sequence_number);				
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

void crypto_Aes256XtsEncryptor_TestClass::test_UseUtilFuncEnc()
{
	std::cout << "[tc::crypto::Aes256XtsEncryptor] test_UseUtilFuncEnc : " << std::flush;
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

				// test encryption
				tc::crypto::EncryptAes256Xts(data.data(), test->plaintext.data(), data.size(), test->data_unit_sequence_number, test->key1.data(), test->key1.size(), test->key2.data(), test->key2.size(), test->data_unit, true);
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

void crypto_Aes256XtsEncryptor_TestClass::test_UseUtilFuncDec()
{
	std::cout << "[tc::crypto::Aes256XtsEncryptor] test_UseUtilFuncDec : " << std::flush;
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

				// test decryption
				tc::crypto::DecryptAes256Xts(data.data(), test->ciphertext.data(), data.size(), test->data_unit_sequence_number, test->key1.data(), test->key1.size(), test->key2.data(), test->key2.size(), test->data_unit, true);
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

void crypto_Aes256XtsEncryptor_TestClass::test_DoesNothingWhenNotInit()
{
	std::cout << "[tc::crypto::Aes256XtsEncryptor] test_DoesNothingWhenNotInit : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			tc::crypto::Aes256XtsEncryptor cryptor;

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

void crypto_Aes256XtsEncryptor_TestClass::test_InitializeThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes256XtsEncryptor] test_InitializeThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_IEEE1619_2007_TestCases(tests);

			tc::crypto::Aes256XtsEncryptor cryptor;

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
				cryptor.initialize(tests[0].key1.data(), tc::crypto::Aes256XtsEncryptor::kKeySize-1, tests[0].key2.data(), tests[0].key2.size(), tests[0].data_unit, true);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key1_size==tc::crypto::Aes256XtsEncryptor::kKeySize-1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key1.data(), tc::crypto::Aes256XtsEncryptor::kKeySize+1, tests[0].key2.data(), tests[0].key2.size(), tests[0].data_unit, true);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key1_size==tc::crypto::Aes256XtsEncryptor::kKeySize+1");
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
				cryptor.initialize(tests[0].key1.data(), tests[0].key1.size(), tests[0].key2.data(), tc::crypto::Aes256XtsEncryptor::kKeySize-1, tests[0].data_unit, true);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key2_size==tc::crypto::Aes256XtsEncryptor::kKeySize-1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key1.data(), tests[0].key1.size(), tests[0].key2.data(), tc::crypto::Aes256XtsEncryptor::kKeySize+1, tests[0].data_unit, true);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key2_size==tc::crypto::Aes256XtsEncryptor::kKeySize+1");
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
				cryptor.initialize(tests[0].key1.data(), tests[0].key1.size(), tests[0].key2.data(), tests[0].key2.size(), tc::crypto::Aes256XtsEncryptor::kBlockSize-1, true);
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

void crypto_Aes256XtsEncryptor_TestClass::test_EncryptThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes256XtsEncryptor] test_EncryptThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_IEEE1619_2007_TestCases(tests);

			tc::crypto::Aes256XtsEncryptor cryptor;

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

void crypto_Aes256XtsEncryptor_TestClass::test_DecryptThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes256XtsEncryptor] test_DecryptThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_IEEE1619_2007_TestCases(tests);

			tc::crypto::Aes256XtsEncryptor cryptor;

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

void crypto_Aes256XtsEncryptor_TestClass::util_Setup_IEEE1619_2007_TestCases(std::vector<crypto_Aes256XtsEncryptor_TestClass::TestCase>& test_cases)
{
	TestCase tmp;

	test_cases.clear();

	// XTS-AES-256 applied for a data unit of 512 bytes
	tmp.data_unit = 512;

	tmp.test_name = "IEEE 1619-2007 Vector 10";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("2718281828459045235360287471352662497757247093699959574966967627");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("3141592653589793238462643383279502884197169399375105820974944592");
	tmp.data_unit_sequence_number = 0xff;
	tmp.plaintext =  tc::cli::FormatUtil::hexStringToBytes("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("1c3b3a102f770386e4836c99e370cf9bea00803f5e482357a4ae12d414a3e63b5d31e276f8fe4a8d66b317f9ac683f44680a86ac35adfc3345befecb4bb188fd5776926c49a3095eb108fd1098baec70aaa66999a72a82f27d848b21d4a741b0c5cd4d5fff9dac89aeba122961d03a757123e9870f8acf1000020887891429ca2a3e7a7d7df7b10355165c8b9a6d0a7de8b062c4500dc4cd120c0f7418dae3d0b5781c34803fa75421c790dfe1de1834f280d7667b327f6c8cd7557e12ac3a0f93ec05c52e0493ef31a12d3d9260f79a289d6a379bc70c50841473d1a8cc81ec583e9645e07b8d9670655ba5bbcfecc6dc3966380ad8fecb17b6ba02469a020a84e18e8f84252070c13e9f1f289be54fbc481457778f616015e1327a02b140f1505eb309326d68378f8374595c849d84f4c333ec4423885143cb47bd71c5edae9be69a2ffeceb1bec9de244fbe15992b11b77c040f12bd8f6a975a44a0f90c29a9abc3d4d893927284c58754cce294529f8614dcd2aba991925fedc4ae74ffac6e333b93eb4aff0479da9a410e4450e0dd7ae4c6e2910900575da401fc07059f645e8b7e9bfdef33943054ff84011493c27b3429eaedb4ed5376441a77ed43851ad77f16f541dfd269d50d6a5f14fb0aab1cbb4c1550be97f7ab4066193c4caa773dad38014bd2092fa755c824bb5e54c4f36ffda9fcea70b9c6e693e148c151");
	test_cases.push_back(tmp);

	tmp.test_name = "IEEE 1619-2007 Vector 11";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("2718281828459045235360287471352662497757247093699959574966967627");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("3141592653589793238462643383279502884197169399375105820974944592");
	tmp.data_unit_sequence_number = 0xffff;
	tmp.plaintext =  tc::cli::FormatUtil::hexStringToBytes("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("77a31251618a15e6b92d1d66dffe7b50b50bad552305ba0217a610688eff7e11e1d0225438e093242d6db274fde801d4cae06f2092c728b2478559df58e837c2469ee4a4fa794e4bbc7f39bc026e3cb72c33b0888f25b4acf56a2a9804f1ce6d3d6e1dc6ca181d4b546179d55544aa7760c40d06741539c7e3cd9d2f6650b2013fd0eeb8c2b8e3d8d240ccae2d4c98320a7442e1c8d75a42d6e6cfa4c2eca1798d158c7aecdf82490f24bb9b38e108bcda12c3faf9a21141c3613b58367f922aaa26cd22f23d708dae699ad7cb40a8ad0b6e2784973dcb605684c08b8d6998c69aac049921871ebb65301a4619ca80ecb485a31d744223ce8ddc2394828d6a80470c092f5ba413c3378fa6054255c6f9df4495862bbb3287681f931b687c888abf844dfc8fc28331e579928cd12bd2390ae123cf03818d14dedde5c0c24c8ab018bfca75ca096f2d531f3d1619e785f1ada437cab92e980558b3dce1474afb75bfedbf8ff54cb2618e0244c9ac0d3c66fb51598cd2db11f9be39791abe447c63094f7c453b7ff87cb5bb36b7c79efb0872d17058b83b15ab0866ad8a58656c5a7e20dbdf308b2461d97c0ec0024a2715055249cf3b478ddd4740de654f75ca686e0d7345c69ed50cdc2a8b332b1f8824108ac937eb050585608ee734097fc09054fbff89eeaeea791f4a7ab1f9868294a4f9e27b42af8100cb9d59cef9645803");
	test_cases.push_back(tmp);

	tmp.test_name = "IEEE 1619-2007 Vector 12";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("2718281828459045235360287471352662497757247093699959574966967627");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("3141592653589793238462643383279502884197169399375105820974944592");
	tmp.data_unit_sequence_number = 0xffffff;
	tmp.plaintext =  tc::cli::FormatUtil::hexStringToBytes("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("e387aaa58ba483afa7e8eb469778317ecf4cf573aa9d4eac23f2cdf914e4e200a8b490e42ee646802dc6ee2b471b278195d60918ececb44bf79966f83faba0499298ebc699c0c8634715a320bb4f075d622e74c8c932004f25b41e361025b5a87815391f6108fc4afa6a05d9303c6ba68a128a55705d415985832fdeaae6c8e19110e84d1b1f199a2692119edc96132658f09da7c623efcec712537a3d94c0bf5d7e352ec94ae5797fdb377dc1551150721adf15bd26a8efc2fcaad56881fa9e62462c28f30ae1ceaca93c345cf243b73f542e2074a705bd2643bb9f7cc79bb6e7091ea6e232df0f9ad0d6cf502327876d82207abf2115cdacf6d5a48f6c1879a65b115f0f8b3cb3c59d15dd8c769bc014795a1837f3901b5845eb491adfefe097b1fa30a12fc1f65ba22905031539971a10f2f36c321bb51331cdefb39e3964c7ef079994f5b69b2edd83a71ef549971ee93f44eac3938fcdd61d01fa71799da3a8091c4c48aa9ed263ff0749df95d44fef6a0bb578ec69456aa5408ae32c7af08ad7ba8921287e3bbee31b767be06a0e705c864a769137df28292283ea81a2480241b44d9921cdbec1bc28dc1fda114bd8e5217ac9d8ebafa720e9da4f9ace231cc949e5b96fe76ffc21063fddc83a6b8679c00d35e09576a875305bed5f36ed242c8900dd1fa965bc950dfce09b132263a1eef52dd6888c309f5a7d712826");
	test_cases.push_back(tmp);

	tmp.test_name = "IEEE 1619-2007 Vector 13";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("2718281828459045235360287471352662497757247093699959574966967627");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("3141592653589793238462643383279502884197169399375105820974944592");
	tmp.data_unit_sequence_number = 0xffffffff;
	tmp.plaintext =  tc::cli::FormatUtil::hexStringToBytes("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("bf53d2dade78e822a4d949a9bc6766b01b06a8ef70d26748c6a7fc36d80ae4c5520f7c4ab0ac8544424fa405162fef5a6b7f229498063618d39f0003cb5fb8d1c86b643497da1ff945c8d3bedeca4f479702a7a735f043ddb1d6aaade3c4a0ac7ca7f3fa5279bef56f82cd7a2f38672e824814e10700300a055e1630b8f1cb0e919f5e942010a416e2bf48cb46993d3cb6a51c19bacf864785a00bc2ecff15d350875b246ed53e68be6f55bd7e05cfc2b2ed6432198a6444b6d8c247fab941f569768b5c429366f1d3f00f0345b96123d56204c01c63b22ce78baf116e525ed90fdea39fa469494d3866c31e05f295ff21fea8d4e6e13d67e47ce722e9698a1c1048d68ebcde76b86fcf976eab8aa9790268b7068e017a8b9b749409514f1053027fd16c3786ea1bac5f15cb79711ee2abe82f5cf8b13ae73030ef5b9e4457e75d1304f988d62dd6fc4b94ed38ba831da4b7634971b6cd8ec325d9c61c00f1df73627ed3745a5e8489f3a95c69639c32cd6e1d537a85f75cc844726e8a72fc0077ad22000f1d5078f6b866318c668f1ad03d5a5fced5219f2eabbd0aa5c0f460d183f04404a0d6f469558e81fab24a167905ab4c7878502ad3e38fdbe62a41556cec37325759533ce8f25f367c87bb5578d667ae93f9e2fd99bcbc5f2fbba88cf6516139420fcff3b7361d86322c4bd84c82f335abb152c4a93411373aaa8220");
	test_cases.push_back(tmp);
	
	tmp.test_name = "IEEE 1619-2007 Vector 14";
	tmp.key1 = tc::cli::FormatUtil::hexStringToBytes("2718281828459045235360287471352662497757247093699959574966967627");
	tmp.key2 = tc::cli::FormatUtil::hexStringToBytes("3141592653589793238462643383279502884197169399375105820974944592");
	tmp.data_unit_sequence_number = 0xffffffffff;
	tmp.plaintext =  tc::cli::FormatUtil::hexStringToBytes("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("64497e5a831e4a932c09be3e5393376daa599548b816031d224bbf50a818ed2350eae7e96087c8a0db51ad290bd00c1ac1620857635bf246c176ab463be30b808da548081ac847b158e1264be25bb0910bbc92647108089415d45fab1b3d2604e8a8eff1ae4020cfa39936b66827b23f371b92200be90251e6d73c5f86de5fd4a950781933d79a28272b782a2ec313efdfcc0628f43d744c2dc2ff3dcb66999b50c7ca895b0c64791eeaa5f29499fb1c026f84ce5b5c72ba1083cddb5ce45434631665c333b60b11593fb253c5179a2c8db813782a004856a1653011e93fb6d876c18366dd8683f53412c0c180f9c848592d593f8609ca736317d356e13e2bff3a9f59cd9aeb19cd482593d8c46128bb32423b37a9adfb482b99453fbe25a41bf6feb4aa0bef5ed24bf73c762978025482c13115e4015aac992e5613a3b5c2f685b84795cb6e9b2656d8c88157e52c42f978d8634c43d06fea928f2822e465aa6576e9bf419384506cc3ce3c54ac1a6f67dc66f3b30191e698380bc999b05abce19dc0c6dcc2dd001ec535ba18deb2df1a101023108318c75dc98611a09dc48a0acdec676fabdf222f07e026f059b672b56e5cbc8e1d21bbd867dd927212054681d70ea737134cdfce93b6f82ae22423274e58a0821cc5502e2d0ab4585e94de6975be5e0b4efce51cd3e70c25a1fbbbd609d273ad5b0d59631c531f6a0a57b9");
	test_cases.push_back(tmp);
}