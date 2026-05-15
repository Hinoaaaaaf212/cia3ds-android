#include <iostream>
#include <sstream>
#include <fstream>

#include <mbedtls/aes.h>

#include "crypto_Aes128Encryptor_TestClass.h"

#include <tc/Exception.h>
#include <tc/crypto/AesEncryptor.h>
#include <tc/cli/FormatUtil.h>

#include <tc/io/PaddingSource.h>

void crypto_Aes128Encryptor_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::Aes128Encryptor] START" << std::endl;
	test_Constants();
	test_UseClassEnc();
	test_UseClassDec();

	test_DoesNothingWhenNotInit();
	test_InitializeThrowsExceptionOnBadInput();
	test_EncryptThrowsExceptionOnBadInput();
	test_DecryptThrowsExceptionOnBadInput();
	std::cout << "[tc::crypto::Aes128Encryptor] END" << std::endl;
}

void crypto_Aes128Encryptor_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::Aes128Encryptor] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check block size
			static const size_t kExpectedBlockSize = 16;
			if (tc::crypto::Aes128Encryptor::kBlockSize != kExpectedBlockSize)
			{
				ss << "kBlockSize had value " << std::dec << tc::crypto::Aes128Encryptor::kBlockSize << " (expected " << kExpectedBlockSize << ")";
				throw tc::Exception(ss.str());
			}

			// check key size
			static const size_t kExpectedKeySize = 16;
			if (tc::crypto::Aes128Encryptor::kKeySize != kExpectedKeySize)
			{
				ss << "kKeySize had value " << std::dec << tc::crypto::Aes128Encryptor::kKeySize << " (expected " << kExpectedKeySize << ")";
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

void crypto_Aes128Encryptor_TestClass::test_UseClassEnc()
{
	std::cout << "[tc::crypto::Aes128Encryptor] test_UseClassEnc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes128Encryptor cryptor;

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

void crypto_Aes128Encryptor_TestClass::test_UseClassDec()
{
	std::cout << "[tc::crypto::Aes128Encryptor] test_UseClassDec : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes128Encryptor cryptor;

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

void crypto_Aes128Encryptor_TestClass::test_DoesNothingWhenNotInit()
{
	std::cout << "[tc::crypto::Aes128Encryptor] test_DoesNothingWhenNotInit : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			tc::crypto::Aes128Encryptor cryptor;

			// create data
			tc::ByteData control_data = tc::io::PaddingSource(0xee, tc::crypto::Aes128Encryptor::kBlockSize).pullData(0, tc::crypto::Aes128Encryptor::kBlockSize);
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

void crypto_Aes128Encryptor_TestClass::test_InitializeThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes128Encryptor] test_InitializeThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes128Encryptor cryptor;

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
				cryptor.initialize(tests[0].key.data(), tc::crypto::Aes128Encryptor::kKeySize-1);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key_size==tc::crypto::Aes128Encryptor::kKeySize-1");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(tests[0].key.data(), tc::crypto::Aes128Encryptor::kKeySize+1);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where key_size==tc::crypto::Aes128Encryptor::kKeySize+1");
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

void crypto_Aes128Encryptor_TestClass::test_EncryptThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes128Encryptor] test_EncryptThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes128Encryptor cryptor;

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

void crypto_Aes128Encryptor_TestClass::test_DecryptThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Aes128Encryptor] test_DecryptThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_TestCases(tests);

			tc::crypto::Aes128Encryptor cryptor;

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

void crypto_Aes128Encryptor_TestClass::util_Setup_TestCases(std::vector<crypto_Aes128Encryptor_TestClass::TestCase>& test_cases)
{
	TestCase tmp;

	test_cases.clear();

	// Variable Key Known Answer Tests
	// taken from "ecb_vk.txt" from https://csrc.nist.gov/archive/aes/rijndael/rijndael-vals.zip
	tmp.plaintext = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000000");

	tmp.test_name = "Variable Key Known Answer Test 1";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("80000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("0EDD33D3C621E546455BD8BA1418BEC8");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 2";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("40000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C0CC0C5DA5BD63ACD44A80774FAD5222");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 3";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("20000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("2F0B4B71BC77851B9CA56D42EB8FF080");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 4";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("10000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("6B1E2FFFE8A114009D8FE22F6DB5F876");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 5";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("08000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("9AA042C315F94CBB97B62202F83358F5");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 6";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("04000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("DBE01DE67E346A800C4C4B4880311DE4");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 7";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("02000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C117D2238D53836ACD92DDCDB85D6A21");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 8";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("01000000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("DC0ED85DF9611ABB7249CDD168C5467E");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 9";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00800000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("807D678FFF1F56FA92DE3381904842F2");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 10";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00400000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("0E53B3FCAD8E4B130EF73AEB957FB402");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 11";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00200000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("969FFD3B7C35439417E7BDE923035D65");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 12";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00100000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A99B512C19CA56070491166A1503BF15");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 13";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00080000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("6E9985252126EE344D26AE369D2327E3");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 14";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00040000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B85F4809F904C275491FCDCD1610387E");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 15";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00020000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("ED365B8D7D20C1F5D53FB94DD211DF7B");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 16";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00010000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B3A575E86A8DB4A7135D604C43304896");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 17";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00008000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("89704BCB8E69F846259EB0ACCBC7F8A2");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 18";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00004000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C56EE7C92197861F10D7A92B90882055");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 19";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00002000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("92F296F6846E0EAF9422A5A24A08B069");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 20";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00001000000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E67E32BB8F11DEB8699318BEE9E91A60");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 21";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000800000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B08EEF85EAF626DD91B65C4C3A97D92B");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 22";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000400000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("661083A6ADDCE79BB4E0859AB5538013");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 23";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000200000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("55DFE2941E0EB10AFC0B333BD34DE1FE");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 24";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000100000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("6BFE5945E715C9662609770F8846087A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 25";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000080000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("79848E9C30C2F8CDA8B325F7FED2B139");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 26";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000040000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7A713A53B99FEF34AC04DEEF80965BD0");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 27";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000020000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("18144A2B46620D32C3C32CE52D49257F");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 28";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000010000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("872E827C70887C80749F7B8BB1847C7E");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 29";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000008000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("6B86C6A4FE6A60C59B1A3102F8DE49F3");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 30";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000004000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("9848BB3DFDF6F532F094679A4C231A20");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 31";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000002000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("925AD528E852E329B2091CD3F1C2BCEE");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 32";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000001000000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("80DF436544B0DD596722E46792A40CD8");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 33";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000800000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("525DAF18F93E83E1E74BBBDDE4263BBA");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 34";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000400000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F65C9D2EE485D24701FFA3313B9D5BE6");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 35";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000200000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E4FC8D8BCA06425BDF94AFA40FCC14BA");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 36";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000100000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A53F0A5CA1E4E6440BB975FF320DE6F8");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 37";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000080000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("D55313B9394080462E87E02899B553F0");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 38";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000040000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("34A71D761F71BCD344384C7F97D27906");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 39";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000020000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("233F3D819599612EBC89580245C996A8");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 40";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000010000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B4F1374E5268DBCB676E447529E53F89");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 41";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000008000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("0816BD27861D2BA891D1044E39951E96");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 42";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000004000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F3BE9EA3F10C73CA64FDE5DB13A951D1");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 43";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000002000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("2448086A8106FBD03048DDF857D3F1C8");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 44";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000001000000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("670756E65BEC8B68F03D77CDCDCE7B91");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 45";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000800000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("EF968CF0D36FD6C6EFFD225F6FB44CA9");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 46";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000400000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("2E8767157922E3826DDCEC1B0CC1E105");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 47";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000200000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("78CE7EEC670E45A967BAB17E26A1AD36");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 48";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000100000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("3C5CEE825655F098F6E81A2F417DA3FB");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 49";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000080000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("67BFDB431DCE1292200BC6F5207ADB12");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 50";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000040000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7540FD38E447C0779228548747843A6F");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 51";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000020000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B85E513301F8A936EA9EC8A21A85B5E6");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 52";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000010000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("04C67DBF16C11427D507A455DE2C9BC5");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 53";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000008000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("03F75EB8959E55079CFFB4FF149A37B6");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 54";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000004000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("74550287F666C63BB9BC7838433434B0");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 55";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000002000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7D537200195EBC3AEFD1EAAB1C385221");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 56";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000001000000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("CE24E4D40C68A82B535CBD3C8E21652A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 57";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000800000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("AB20072405AA8FC40265C6F1F3DC8BC0");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 58";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000400000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("6CFD2CF688F566B093F67B9B3839E80A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 59";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000200000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("BD95977E6B7239D407A012C5544BF584");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 60";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000100000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("DF9C0130AC77E7C72C997F587B46DBE0");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 61";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000080000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E7F1B82CADC53A648798945B34EFEFF2");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 62";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000040000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("932C6DBF69255CF13EDCDB72233ACEA3");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 63";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000020000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("5C76002BC7206560EFE550C80B8F12CC");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 64";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000010000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F6B7BDD1CAEEBAB574683893C4475484");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 65";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000008000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A920E37CC6DC6B31DA8C0169569F5034");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 66";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000004000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("919380ECD9C778BC513148B0C28D65FD");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 67";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000002000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("EE67308DD3F2D9E6C2170755E5784BE1");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 68";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000001000000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("3CC73E53B85609023A05E149B223AE09");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 69";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000800000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("983E8AF7CF05EBB28D71EB841C9406E6");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 70";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000400000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("0F3099B2D31FA5299EE5BF43193287FC");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 71";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000200000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B763D84F38C27FE6931DCEB6715D4DB6");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 72";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000100000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("5AE3C9B0E3CC29C0C61565CD01F8A248");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 73";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000080000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F58083572CD90981958565D48D2DEE25");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 74";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000040000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7E6255EEF8F70C0EF10337AAB1CCCEF8");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 75";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000020000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("AAD4BAC34DB22821841CE2F631961902");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 76";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000010000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("D7431C0409BB1441BA9C6858DC7D4E81");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 77";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000008000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("EF9298C65E339F6E801A59C626456993");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 78";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000004000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("53FE29F68FF541ABC3F0EF3350B72F7E");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 79";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000002000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F6BBA5C10DB02529E2C2DA3FB582CC14");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 80";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000001000000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("E4239AA37FC531A386DAD1126FC0E9CD");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 81";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000800000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("8F7758F857D15BBE7BFD0E416404C365");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 82";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000400000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("D273EB57C687BCD1B4EA7218A509E7B8");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 83";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000200000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("65D64F8D76E8B3423FA25C4EB58A210A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 84";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000100000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("623D802B4EC450D66A16625702FCDBE0");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 85";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000080000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7496460CB28E5791BAEAF9B68FB00022");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 86";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000040000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("34EA600F18BB0694B41681A49D510C1D");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 87";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000020000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("5F8FF0D47D5766D29B5D6E8F46423BD8");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 88";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000010000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("225F9286C5928BF09F84D3F93F541959");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 89";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000008000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B21E90D25DF383416A5F072CEBEB1FFB");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 90";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000004000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("4AEFCDA089318125453EB9E8EB5E492E");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 91";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000002000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("4D3E75C6CD40EC4869BC85158591ADB8");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 92";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000001000000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("63A8B904405436A1B99D7751866771B7");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 93";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000800000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("64F0DAAE47529199792EAE172BA53293");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 94";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000400000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C3EEF84BEA18225D515A8C852A9047EE");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 95";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000200000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A44AC422B47D47B81AF73B3E9AC9596E");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 96";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000100000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("D16E04A8FBC435094F8D53ADF25F5084");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 97";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000080000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("EF13DC34BAB03E124EEAD8B6BF44B532");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 98";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000040000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("D94799075C24DCC067AF0D392049250D");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 99";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000020000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("14F431771EDDCE4764C21A2254B5E3C8");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 100";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000010000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7039329F36F2ED682B02991F28D64679");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 101";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000008000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("124EE24EDE5551639DB8B8B941F6141D");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 102";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000004000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C2852879A34D5184E478EC918B993FEE");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 103";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000002000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("86A806A3525B93E432053C9AB5ABBEDF");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 104";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000001000000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("C1609BF5A4F07E37C17A36366EC23ECC");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 105";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000800000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("7E81E7CB92159A51FFCEA331B1E8EA53");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 106";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000400000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("37A7BE002856C5A59A6E03EAFCE7729A");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 107";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000200000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("BDF98A5A4F91E890C9A1D1E5FAAB138F");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 108";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000100000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("4E96ACB66E051F2BC739CC3D3E34A26B");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 109";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000080000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("EE996CDD120EB86E21ECFA49E8E1FCF1");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 110";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000040000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("61B9E6B579DBF6070C351A1440DD85FF");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 111";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000020000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("AC369E484316440B40DFC83AA96E28E7");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 112";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000010000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("0A2D16DE985C76D45C579C1159413BBE");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 113";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000008000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("DA3FDC38DA1D374FA4802CDA1A1C6B0F");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 114";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000004000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B842523D4C41C2211AFE43A5800ADCE3");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 115";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000002000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("9E2CDA90D8E992DBA6C73D8229567192");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 116";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000001000");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("D49583B781D9E20F5BE101415957FC49");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 117";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000800");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("EF09DA5C12B376E458B9B8670032498E");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 118";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000400");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A96BE0463DA774461A5E1D5A9DD1AC10");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 119";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000200");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("32CEE3341060790D2D4B1362EF397090");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 120";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000100");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("21CEA416A3D3359D2C4D58FB6A035F06");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 121";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000080");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("172AEAB3D507678ECAF455C12587ADB7");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 122";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000040");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("B6F897941EF8EBFF9FE80A567EF38478");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 123";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000020");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("A9723259D94A7DC662FB0C782CA3F1DD");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 124";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000010");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("2F91C984B9A4839F30001B9F430493B4");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 125";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000008");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("0472406345A610B048CB99EE0EF3FA0F");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 126";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000004");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("F5F39086646F8C05ED16EFA4B617957C");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 127";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000002");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("26D50F485A30408D5AF47A5736292450");
	test_cases.push_back(tmp);

	tmp.test_name = "Variable Key Known Answer Test 128";
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("00000000000000000000000000000001");
	tmp.ciphertext = tc::cli::FormatUtil::hexStringToBytes("0545AAD56DA2A97C3663D1432A3D1C84");
	test_cases.push_back(tmp);
}