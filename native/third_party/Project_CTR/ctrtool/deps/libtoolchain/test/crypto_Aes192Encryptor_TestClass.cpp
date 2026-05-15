#include <iostream>
#include <sstream>
#include <fstream>

#include <mbedtls/aes.h>

#include "crypto_Aes192Encryptor_TestClass.h"

#include <tc/Exception.h>
#include <tc/crypto/AesEncryptor.h>
#include <tc/cli/FormatUtil.h>

#include <tc/io/PaddingSource.h>

void crypto_Aes192Encryptor_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::Aes192Encryptor] START" << std::endl;
	test_Constants();
	test_UseClassEnc();
	test_UseClassDec();

	test_DoesNothingWhenNotInit();
	test_InitializeThrowsExceptionOnBadInput();
	test_EncryptThrowsExceptionOnBadInput();
	test_DecryptThrowsExceptionOnBadInput();
	std::cout << "[tc::crypto::Aes192Encryptor] END" << std::endl;
}

void crypto_Aes192Encryptor_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::Aes192Encryptor] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check block size
			static const size_t kExpectedBlockSize = 16;
			if (tc::crypto::Aes192Encryptor::kBlockSize != kExpectedBlockSize)
			{
				ss << "kBlockSize had value " << std::dec << tc::crypto::Aes192Encryptor::kBlockSize << " (expected " << kExpectedBlockSize << ")";
				throw tc::Exception(ss.str());
			}

			// check key size
			static const size_t kExpectedKeySize = 24;
			if (tc::crypto::Aes192Encryptor::kKeySize != kExpectedKeySize)
			{
				ss << "kKeySize had value " << std::dec << tc::crypto::Aes192Encryptor::kKeySize << " (expected " << kExpectedKeySize << ")";
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

void crypto_Aes192Encryptor_TestClass::test_UseClassEnc()
{
	std::cout << "[tc::crypto::Aes192Encryptor] test_UseClassEnc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes192Encryptor cryptor;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->plaintext.size());	

				// initialize key
				cryptor.initialize(test->key.data(), test->key.size());
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// encrypt data
				cryptor.encrypt(data.data(), test->plaintext.data());
				
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

void crypto_Aes192Encryptor_TestClass::test_UseClassDec()
{
	std::cout << "[tc::crypto::Aes192Encryptor] test_UseClassDec : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes192Encryptor cryptor;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->plaintext.size());
				
				// initialize key
				cryptor.initialize(test->key.data(), test->key.size());
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// decrypt data
				cryptor.decrypt(data.data(), test->ciphertext.data());

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

void crypto_Aes192Encryptor_TestClass::test_DoesNothingWhenNotInit()
{
	std::cout << "[tc::crypto::Aes192Encryptor] test_DoesNothingWhenNotInit : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			tc::crypto::Aes192Encryptor cryptor;

			// create data
			tc::ByteData control_data = tc::io::PaddingSource(0xee, tc::crypto::Aes192Encryptor::kBlockSize).pullData(0, tc::crypto::Aes192Encryptor::kBlockSize);
			tc::ByteData data = tc::ByteData(control_data.data(), control_data.size());

			// try to decrypt without calling initialize()
			cryptor.decrypt(data.data(), data.data());

			// test plain text			
			if (memcmp(data.data(), control_data.data(), data.size()) != 0)
			{
				ss << "Failed: decrypt() operated on data when not initialized";
				throw tc::Exception(ss.str());
			}

			// try to encrypt without calling initialize()
			cryptor.encrypt(data.data(), data.data());

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

void crypto_Aes192Encryptor_TestClass::test_InitializeThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes192Encryptor] test_InitializeThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes192Encryptor cryptor;

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
				cryptor.initialize(tests[0].key.data(), tc::crypto::Aes192Encryptor::kKeySize-1);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key_size==tc::crypto::Aes192Encryptor::kKeySize-1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key.data(), tc::crypto::Aes192Encryptor::kKeySize+1);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key_size==tc::crypto::Aes192Encryptor::kKeySize+1");
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

void crypto_Aes192Encryptor_TestClass::test_EncryptThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes192Encryptor] test_EncryptThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes192Encryptor cryptor;

			cryptor.initialize(tests[0].key.data(), tests[0].key.size());

			tc::ByteData data = tc::ByteData(tests[0].plaintext.size());

			// reference encrypt call
			//cryptor.encrypt(data.data(), tests[0].plaintext.data());

			try {
				cryptor.encrypt(nullptr, tests[0].plaintext.data());
				throw tc::Exception("Failed to throw ArgumentNullException where dst==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.encrypt(data.data(), nullptr);
				throw tc::Exception("Failed to throw ArgumentNullException where src==nullptr");
			} catch(const tc::ArgumentNullException&) {
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

void crypto_Aes192Encryptor_TestClass::test_DecryptThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes192Encryptor] test_DecryptThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes192Encryptor cryptor;

			cryptor.initialize(tests[0].key.data(), tests[0].key.size());

			tc::ByteData data = tc::ByteData(tests[0].plaintext.size());

			// reference decrypt call
			//cryptor.decrypt(data.data(), tests[0].ciphertext.data());

			try {
				cryptor.decrypt(nullptr, tests[0].ciphertext.data());
				throw tc::Exception("Failed to throw ArgumentNullException where dst==nullptr");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.decrypt(data.data(), nullptr);
				throw tc::Exception("Failed to throw ArgumentNullException where src==nullptr");
			} catch(const tc::ArgumentNullException&) {
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

void crypto_Aes192Encryptor_TestClass::util_Setup_TestCases(std::vector<crypto_Aes192Encryptor_TestClass::TestCase>& test_cases)
{
	TestCase tmp;

	test_cases.clear();

	// Variable Key Known Answer Tests
	// taken from "ecb_vk.txt" from https://csrc.nist.gov/archive/aes/rijndael/rijndael-vals.zip
	tmp.plaintext = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000000");

	tmp.test_name = "Variable Key Known Answer Test 1";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("800000000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("DE885DC87F5A92594082D02CC1E1B42C");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 2";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("400000000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C749194F94673F9DD2AA1932849630C1");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 3";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("200000000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("0CEF643313912934D310297B90F56ECC");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 4";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("100000000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C4495D39D4A553B225FBA02A7B1B87E1");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 5";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("080000000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("636D10B1A0BCAB541D680A7970ADC830");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 6";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("040000000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("07CF045786BD6AFCC147D99E45A901A7");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 7";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("020000000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("6A8E3F425A7599348F95398448827976");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 8";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("010000000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("5518276836148A00D91089A20D8BFF57");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 9";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("008000000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F267E07B5E87E3BC20B969C61D4FCB06");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 10";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("004000000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("5A1CDE69571D401BFCD20DEBADA2212C");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 11";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("002000000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("70A9057263254701D12ADD7D74CD509E");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 12";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("001000000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("35713A7E108031279388A33A0FE2E190");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 13";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000800000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E74EDE82B1254714F0C7B4B243108655");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 14";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000400000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("39272E3100FAA37B55B862320D1B3EB3");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 15";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000200000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("6D6E24C659FC5AEF712F77BCA19C9DD0");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 16";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000100000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("76D18212F972370D3CC2C6C372C6CF2F");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 17";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000080000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B21A1F0BAE39E55C7594ED570A7783EA");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 18";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000040000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("77DE202111895AC48DD1C974B358B458");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 19";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000020000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("67810B311969012AAF7B504FFAF39FD1");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 20";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000010000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C22EA2344D3E9417A6BA07843E713AEA");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 21";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000008000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C79CAF4B97BEE0BD0630AB354539D653");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 22";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000004000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("135FD1AF761D9AE23DF4AA6B86760DB4");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 23";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000002000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("D4659D0B06ACD4D56AB8D11A16FD83B9");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 24";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000001000000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F7D270028FC188E4E4F35A4AAA25D4D4");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 25";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000800000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("345CAE5A8C9620A9913D5473985852FF");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 26";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000400000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("4E8980ADDE60B0E42C0B287FEA41E729");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 27";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000200000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F11B6D74E1F15155633DC39743C1A527");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 28";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000100000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("9C87916C0180064F9D3179C6F5DD8C35");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 29";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000080000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("71AB186BCAEA518E461D4F7FAD230E6A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 30";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000040000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C4A31BBC3DAAF742F9141C2A5001A49C");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 31";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000020000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E7C47B7B1D40F182A8928C8A55671D07");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 32";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000010000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("8E17F294B28FA373C6249538868A7EEF");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 33";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000008000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("754404096A5CBC08AF09491BE249141A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 34";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000004000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("101CB56E55F05D86369B6D1069204F0A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 35";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000002000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("73F19BB6604205C6EE227B9759791E41");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 36";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000001000000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("6270C0028F0D136C37A56B2CB64D24D6");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 37";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000800000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A3BF7C2C38D1114A087ECF212E694346");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 38";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000400000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("49CABFF2CEF7D9F95F5EFB1F7A1A7DDE");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 39";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000200000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("EC7F8A47CC59B849469255AD49F62752");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 40";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000100000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("68FAE55A13EFAF9B07B3552A8A0DC9D1");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 41";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000080000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("211E6B19C69FAEF481F64F24099CDA65");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 42";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000040000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("DBB918C75BC5732416F79FB0C8EE4C5C");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 43";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000020000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("98D494E5D963A6C8B92536D3EC35E3FD");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 44";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000010000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C9A873404D403D6F074190851D67781A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 45";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000008000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("073AEF4A7C77D921928CB0DD9D27CAE7");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 46";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000004000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("89BDE25CEE36FDE769A10E52298CF90F");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 47";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000002000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("26D0842D37EAD38557C65E0A5E5F122E");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 48";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000001000000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F8294BA375AF46B3F22905BBAFFAB107");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 49";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000800000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("2AD63EB4D0D43813B979CF72B35BDB94");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 50";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000400000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7710C171EE0F4EFA39BE4C995180181D");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 51";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000200000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C0CB2B40DBA7BE8C0698FAE1E4B80FF8");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 52";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000100000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("97970E505194622FD955CA1B80B784E9");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 53";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000080000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7CB1824B29F850900DF2CAD9CF04C1CF");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 54";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000040000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("FDF4F036BB988E42F2F62DE63FE19A64");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 55";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000020000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("08908CFE2C82606B2C15DF61B75CF3E2");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 56";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000010000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B3AA689EF2D07FF365ACB9ADBA2AF07A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 57";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000008000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F2672CD8EAA3B98776660D0263656F5C");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 58";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000004000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("5BDEAC00E986687B9E1D94A0DA7BF452");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 59";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000002000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E6D57BD66EA1627363EE0C4B711B0B21");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 60";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000001000000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("03730DD6ACB4AD9996A63BE7765EC06F");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 61";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000800000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A470E361AA5437B2BE8586D2F78DE582");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 62";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000400000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7567FEEFA559911FD479670246B484E3");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 63";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000200000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("29829DEA15A4E7A4C049045E7B106E29");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 64";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000100000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A407834C3D89D48A2CB7A152208FA4ED");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 65";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000080000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("68F948053F78FEF0D8F9FE7EF3A89819");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 66";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000040000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B605174CAB13AD8FE3B20DA3AE7B0234");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 67";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000020000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("CCAB8F0AEBFF032893996D383CBFDBFA");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 68";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000010000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("AF14BB8428C9730B7DC17B6C1CBEBCC8");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 69";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000008000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("5A41A21332040877EB7B89E8E80D19FE");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 70";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000004000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("AC1BA52EFCDDE368B1596F2F0AD893A0");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 71";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000002000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("41B890E31B9045E6ECDC1BC3F2DB9BCC");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 72";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000001000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("4D54A549728E55B19A23660424A0F146");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 73";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000800000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A917581F41C47C7DDCFFD5285E2D6A61");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 74";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000400000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("604DF24BA6099B93A7405A524D764FCB");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 75";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000200000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("78D9D156F28B190E232D1B7AE7FC730A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 76";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000100000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("5A12C39E442CD7F27B3CD77F5D029582");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 77";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000080000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("FF2BF2F47CF7B0F28EE25AF95DBF790D");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 78";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000040000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("1863BB7D193BDA39DF090659EB8AE48B");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 79";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000020000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("38178F2FB4CFCF31E87E1ABCDC023EB5");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 80";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000010000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F5B13DC690CC0D541C6BA533023DC8C9");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 81";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000008000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("48EC05238D7375D126DC9D08884D4827");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 82";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000004000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("ACD0D81139691B310B92A6E377BACC87");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 83";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000002000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("9A4AA43578B55CE9CC178F0D2E162C79");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 84";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000001000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("08AD94BC737DB3C87D49B9E01B720D81");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 85";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000800000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("3BCFB2D5D210E8332900C5991D551A2A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 86";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000400000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C5F0C6B9397ACB29635CE1A0DA2D8D96");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 87";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000200000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("844A29EFC693E2FA9900F87FBF5DCD5F");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 88";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000100000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("5126A1C41051FEA158BE41200E1EA59D");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 89";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000080000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("302123CA7B4F46D667FFFB0EB6AA7703");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 90";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000040000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A9D16BCE7DB5C024277709EE2A88D91A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 91";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000020000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F013C5EC123A26CFC34B598C992A996B");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 92";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000010000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E38A825CD971A1D2E56FB1DBA248F2A8");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 93";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000008000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("6E701773C0311E0BD4C5A097406D22B3");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 94";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000004000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("754262CEF0C64BE4C3E67C35ABE439F7");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 95";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000002000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C9C2D4C47DF7D55CFA0EE5F1FE5070F4");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 96";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000001000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("6AB4BEA85B172573D8BD2D5F4329F13D");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 97";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000800000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("11F03EF28E2CC9AE5165C587F7396C8C");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 98";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000400000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("0682F2EB1A68BAC7949922C630DD27FA");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 99";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000200000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("ABB0FEC0413D659AFE8E3DCF6BA873BB");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 100";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000100000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("FE86A32E19F805D6569B2EFADD9C92AA");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 101";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000080000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E434E472275D1837D3D717F2EECC88C3");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 102";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000040000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("74E57DCD12A21D26EF8ADAFA5E60469A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 103";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000020000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C275429D6DAD45DDD423FA63C816A9C1");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 104";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000010000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7F6EC1A9AE729E86F7744AED4B8F4F07");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 105";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000008000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("48B5A71AB9292BD4F9E608EF102636B2");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 106";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000004000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("076FB95D5F536C78CBED3181BCCF3CF1");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 107";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000002000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("BFA76BEA1E684FD3BF9256119EE0BC0F");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 108";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000001000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7D395923D56577F3FF8670998F8C4A71");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 109";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000800000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("BA02C986E529AC18A882C34BA389625F");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 110";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000400000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("3DFCF2D882AFE75D3A191193013A84B5");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 111";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000200000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("FAD1FDE1D0241784B63080D2C74D236C");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 112";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000100000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7D6C80D39E41F007A14FB9CD2B2C15CD");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 113";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000080000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7975F401FC10637BB33EA2DB058FF6EC");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 114";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000040000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("657983865C55A818F02B7FCD52ED7E99");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 115";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000020000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B32BEB1776F9827FF4C3AC9997E84B20");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 116";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000010000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("2AE2C7C374F0A41E3D46DBC3E66BB59F");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 117";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000008000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("4D835E4ABDD4BDC6B88316A6E931A07F");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 118";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000004000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E07EFABFF1C353F7384EBB87B435A3F3");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 119";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000002000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("ED3088DC3FAF89AD87B4356FF1BB09C2");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 120";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000001000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("4324D01140C156FC898C2E32BA03FB05");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 121";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000800000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("BE15D016FACB5BAFBC24FA9289132166");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 122";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000400000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("AC9B7048EDB1ACF4D97A5B0B3F50884B");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 123";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000200000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("448BECE1F86C7845DFA9A4BB2A016FB3");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 124";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000100000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("10DD445E87686EB46EA9B1ABC49257F0");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 125";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000080000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B7FCCF7659FA756D4B7303EEA6C07458");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 126";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000040000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("289117115CA3513BAA7640B1004872C2");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 127";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000020000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("57CB42F7EE7186051F50B93FFA7B35BF");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 128";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000010000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F2741BFBFB81663B9136802FB9C3126A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 129";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000008000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E32DDDC5C7398C096E3BD535B31DB5CE");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 130";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000004000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("81D3C204E608AF9CC713EAEBCB72433F");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 131";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000002000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("D4DEEF4BFC36AAA579496E6935F8F98E");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 132";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000001000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C356DB082B97802B038571C392C5C8F6");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 133";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000800000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A3919ECD4861845F2527B77F06AC6A4E");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 134";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000400000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A53858E17A2F802A20E40D44494FFDA0");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 135";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000200000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("5D989E122B78C758921EDBEEB827F0C0");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 136";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000100000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("4B1C0C8F9E7830CC3C4BE7BD226FA8DE");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 137";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000080000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("82C40C5FD897FBCA7B899C70713573A1");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 138";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000040000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("ED13EE2D45E00F75CCDB51EA8E3E36AD");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 139";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000020000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F121799EEFE8432423176A3CCF6462BB");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 140";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000010000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("4FA0C06F07997E98271DD86F7B355C50");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 141";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000008000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("849EB364B4E81D058649DC5B1BF029B9");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 142";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000004000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F48F9E0DE8DE7AD944A207809335D9B1");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 143";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000002000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E59E9205B5A81A4FD26DFCF308966022");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 144";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000001000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("3A91A1BE14AAE9ED700BDF9D70018804");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 145";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000800000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("8ABAD78DCB79A48D79070E7DA89664EC");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 146";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000400000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B68377D98AAE6044938A7457F6C649D9");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 147";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000200000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E4E1275C42F5F1B63D662C099D6CE33D");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 148";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000100000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7DEF32A34C6BE668F17DA1BB193B06EF");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 149";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000080000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("78B6000CC3D30CB3A74B68D0EDBD2B53");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 150";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000040000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("0A47531DE88DD8AE5C23EAE4F7D1F2D5");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 151";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000020000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("667B24E8000CF68231EC484581D922E5");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 152";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000010000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("39DAA5EBD4AACAE130E9C33236C52024");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 153";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000008000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E3C88760B3CB21360668A63E55BB45D1");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 154";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000004000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F131EE903C1CDB49D416866FD5D8DE51");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 155";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000002000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7A1916135B0447CF4033FC13047A583A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 156";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000001000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F7D55FB27991143DCDFA90DDF0424FCB");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 157";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000800000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("EA93E7D1CA1111DBD8F7EC111A848C0C");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 158";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000400000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("2A689E39DFD3CBCBE221326E95888779");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 159";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000200000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C1CE399CA762318AC2C40D1928B4C57D");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 160";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000100000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("D43FB6F2B2879C8BFAF0092DA2CA63ED");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 161";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000080000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("224563E617158DF97650AF5D130E78A5");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 162";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000040000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("6562FDF6833B7C4F7484AE6EBCC243DD");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 163";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000020000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("93D58BA7BED22615D661D002885A7457");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 164";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000010000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("9A0EF559003AD9E52D3E09ED3C1D3320");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 165";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000008000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("96BAF5A7DC6F3DD27EB4C717A85D261C");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 166";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000004000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B8762E06884900E8452293190E19CCDB");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 167";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000002000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("785416A22BD63CBABF4B1789355197D3");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 168";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000001000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A0D20CE1489BAA69A3612DCE90F7ABF6");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 169";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000800000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("700244E93DC94230CC607FFBA0E48F32");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 170";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000400000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("85329E476829F872A2B4A7E59F91FF2D");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 171";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000200000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E4219B4935D988DB719B8B8B2B53D247");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 172";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000100000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("6ACDD04FD13D4DB4409FE8DD13FD737B");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 173";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000080000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("9EB7A670AB59E15BE582378701C1EC14");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 174";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000040000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("29DF2D6935FE657763BC7A9F22D3D492");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 175";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000020000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("99303359D4A13AFDBE6C784028CE533A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 176";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000010000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("FF5C70A6334545F33B9DBF7BEA0417CA");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 177";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000008000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("289F58A17E4C50EDA4269EFB3DF55815");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 178";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000004000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("EA35DCB416E9E1C2861D1682F062B5EB");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 179";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000002000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("3A47BF354BE775383C50B0C0A83E3A58");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 180";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000001000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("BF6C1DC069FB95D05D43B01D8206D66B");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 181";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000000800");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("046D1D580D5898DA6595F32FD1F0C33D");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 182";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000000400");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("5F57803B7B82A110F7E9855D6A546082");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 183";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000000200");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("25336ECF34E7BE97862CDFF715FF05A8");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 184";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000000100");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("ACBAA2A943D8078022D693890E8C4FEF");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 185";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000000080");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("3947597879F6B58E4E2F0DF825A83A38");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 186";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000000040");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("4EB8CC3335496130655BF3CA570A4FC0");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 187";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000000020");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("BBDA7769AD1FDA425E18332D97868824");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 188";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000000010");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("5E7532D22DDB0829A29C868198397154");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 189";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000000008");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E66DA67B630AB7AE3E682855E1A1698E");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 190";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000000004");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("4D93800F671B48559A64D1EA030A590A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 191";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000000002");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F33159FCC7D9AE30C062CD3B322AC764");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 192";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("000000000000000000000000000000000000000000000001");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("8BAE4EFB70D33A9792EEA9BE70889D72");
	test_cases.push_back(tmp);
}