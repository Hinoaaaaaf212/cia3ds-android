#include <iostream>
#include <sstream>
#include <fstream>

#include "crypto_Rsa2048OaepSha256Encryptor_TestClass.h"
#include "RsaOaepUtil.h"

#include <tc/Exception.h>
#include <tc/crypto/RsaOaepSha256Encryptor.h>
#include <tc/cli/FormatUtil.h>

#include <tc/io/PaddingSource.h>

void crypto_Rsa2048OaepSha256Encryptor_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::Rsa2048OaepSha256Encryptor] START" << std::endl;
	test_Constants();
	test_UseClassDec();
	test_UseClassEnc();
	test_UseUtilFuncDec();
	test_UseUtilFuncEnc();
	test_UnspecifiedSeedProducesDifferentBlock();

	test_DoesNothingWhenNotInit();
	test_InitializeThrowsExceptionOnBadInput();
	test_EncryptReturnsFalseOnBadInput();
	test_DecryptReturnsFalseOnBadInput();
	std::cout << "[tc::crypto::Rsa2048OaepSha256Encryptor] END" << std::endl;
}

void crypto_Rsa2048OaepSha256Encryptor_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::Rsa2048OaepSha256Encryptor] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check block size
			static const size_t kExpectedBlockSize = 2048 >> 3;
			if (tc::crypto::Rsa2048OaepSha256Encryptor::kBlockSize != kExpectedBlockSize)
			{
				ss << "kBlockSize had value " << std::dec << tc::crypto::Rsa2048OaepSha256Encryptor::kBlockSize << " (expected " << kExpectedBlockSize << ")";
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

void crypto_Rsa2048OaepSha256Encryptor_TestClass::test_UseClassDec()
{
	std::cout << "[tc::crypto::Rsa2048OaepSha256Encryptor] test_UseClassDec : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<RsaOaepUtil::TestVector> tests;
			RsaOaepUtil::generateRsaOaepTestVectors_Custom(tests, 2048, RsaOaepUtil::SHA256);

			if (tests.begin() == tests.end())
			{
				throw tc::Exception("No tests");
			}

			tc::crypto::Rsa2048OaepSha256Encryptor cryptor;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->dec_message.size());
				
				// initialize
				cryptor.initialize(tc::crypto::RsaPrivateKey(test->key_modulus.data(), test->key_modulus.size(), test->key_private_exponent.data(), test->key_private_exponent.size()), test->label.data(), test->label.size(), test->label_is_digested);
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// test decryption
				size_t message_size = 0;
				cryptor.decrypt(data.data(), message_size, data.size(), test->enc_message.data());
				if (message_size != test->dec_message.size())
				{
					ss << "Test \"" << test->test_name << "\" Failed: message_size = " << message_size << " (expected " << test->dec_message.size() << ")";
					throw tc::Exception(ss.str());
				}		
				if (memcmp(data.data(), test->dec_message.data(), data.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed: " << tc::cli::FormatUtil::formatBytesAsString(data, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->dec_message, true, "") << ")";
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

void crypto_Rsa2048OaepSha256Encryptor_TestClass::test_UseClassEnc()
{
	std::cout << "[tc::crypto::Rsa2048OaepSha256Encryptor] test_UseClassEnc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<RsaOaepUtil::TestVector> tests;
			RsaOaepUtil::generateRsaOaepTestVectors_Custom(tests, 2048, RsaOaepUtil::SHA256);

			if (tests.begin() == tests.end())
			{
				throw tc::Exception("No tests");
			}

			tc::crypto::Rsa2048OaepSha256Encryptor cryptor;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->enc_message.size());
				
				// initialize
				cryptor.initialize(tc::crypto::RsaPublicKey(test->key_modulus.data(), test->key_modulus.size()), test->label.data(), test->label.size(), test->label_is_digested);
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// test encryption
				cryptor.encrypt(data.data(), test->dec_message.data(), test->dec_message.size(), test->enc_seed.data(), test->enc_seed.size());
				if (memcmp(data.data(), test->enc_message.data(), data.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed: " << tc::cli::FormatUtil::formatBytesAsString(data, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->enc_message, true, "");
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

void crypto_Rsa2048OaepSha256Encryptor_TestClass::test_UseUtilFuncDec()
{
	std::cout << "[tc::crypto::Rsa2048OaepSha256Encryptor] test_UseUtilFuncDec : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<RsaOaepUtil::TestVector> tests;
			RsaOaepUtil::generateRsaOaepTestVectors_Custom(tests, 2048, RsaOaepUtil::SHA256);

			if (tests.begin() == tests.end())
			{
				throw tc::Exception("No tests");
			}

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData data = tc::ByteData(test->dec_message.size());
				
				// clear data
				memset(data.data(), 0xff, data.size());

				// test decryption
				size_t message_size = 0;
				bool res = tc::crypto::DecryptRsa2048OaepSha256(data.data(), message_size, data.size(), test->enc_message.data(), tc::crypto::RsaPrivateKey(test->key_modulus.data(), test->key_modulus.size(), test->key_private_exponent.data(), test->key_private_exponent.size()), test->label.data(), test->label.size(), test->label_is_digested);
				if (res != true)
				{
					ss << "Test \"" << test->test_name << "\" Failed: DecryptRsa2048OaepSha256 returned " << std::boolalpha << res << " (expected " << std::boolalpha << true << ")";
					throw tc::Exception(ss.str());
				}
				if (message_size != test->dec_message.size())
				{
					ss << "Test \"" << test->test_name << "\" Failed: message_size = " << message_size << " (expected " << test->dec_message.size() << ")";
					throw tc::Exception(ss.str());
				}		
				if (memcmp(data.data(), test->dec_message.data(), data.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed: " << tc::cli::FormatUtil::formatBytesAsString(data, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->dec_message, true, "") << ")";
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

void crypto_Rsa2048OaepSha256Encryptor_TestClass::test_UseUtilFuncEnc()
{
	std::cout << "[tc::crypto::Rsa2048OaepSha256Encryptor] test_UseUtilFuncEnc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<RsaOaepUtil::TestVector> tests;
			RsaOaepUtil::generateRsaOaepTestVectors_Custom(tests, 2048, RsaOaepUtil::SHA256);

			if (tests.begin() == tests.end())
			{
				throw tc::Exception("No tests");
			}

			// the utility function doesn't support specifying a seed, so we'll have to do some `lite` validation using the already validated decryption utility.
			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData dec_data = tc::ByteData(test->dec_message.size());
				tc::ByteData enc_data = tc::ByteData(test->enc_message.size());
					
				// clear data
				memset(dec_data.data(), 0xff, dec_data.size());
				memset(enc_data.data(), 0xff, enc_data.size());

				// test encryption
				bool res = tc::crypto::EncryptRsa2048OaepSha256(enc_data.data(), test->dec_message.data(), test->dec_message.size(), tc::crypto::RsaPublicKey(test->key_modulus.data(), test->key_modulus.size()), test->label.data(), test->label.size(), test->label_is_digested);
				if (res != true)
				{
					ss << "Test \"" << test->test_name << "\" Failed: EncryptRsa2048OaepSha256 returned " << std::boolalpha << res << " (expected " << std::boolalpha << true << ")";
					throw tc::Exception(ss.str());
				}

				// try to decrypt message
				size_t message_size = 0;
				res = tc::crypto::DecryptRsa2048OaepSha256(dec_data.data(), message_size, dec_data.size(), enc_data.data(), tc::crypto::RsaPrivateKey(test->key_modulus.data(), test->key_modulus.size(), test->key_private_exponent.data(), test->key_private_exponent.size()), test->label.data(), test->label.size(), test->label_is_digested);
				if (res != true)
				{
					ss << "Test \"" << test->test_name << "\" Failed: encrypted message could not be decrypted";
					throw tc::Exception(ss.str());
				}
				if (message_size != test->dec_message.size())
				{
					ss << "Test \"" << test->test_name << "\" Failed: message_size = " << message_size << " (expected " << test->dec_message.size() << ")";
					throw tc::Exception(ss.str());
				}	
				if (memcmp(dec_data.data(), test->dec_message.data(), dec_data.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed: decrypted message was not expected " << tc::cli::FormatUtil::formatBytesAsString(dec_data, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->dec_message, true, "") << ")";
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

void crypto_Rsa2048OaepSha256Encryptor_TestClass::test_UnspecifiedSeedProducesDifferentBlock()
{
	std::cout << "[tc::crypto::Rsa2048OaepSha256Encryptor] test_UnspecifiedSeedProducesDifferentBlock : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<RsaOaepUtil::TestVector> tests;
			RsaOaepUtil::generateRsaOaepTestVectors_Custom(tests, 2048, RsaOaepUtil::SHA256);

			if (tests.begin() == tests.end())
			{
				throw tc::Exception("No tests");
			}

			tc::crypto::Rsa2048OaepSha256Encryptor cryptor;

			// initialize
			cryptor.initialize(tc::crypto::RsaPublicKey(tests[0].key_modulus.data(), tests[0].key_modulus.size()), tests[0].label.data(), tests[0].label.size(), tests[0].label_is_digested);

			static const size_t kTestNum = 20;
			std::vector<tc::ByteData> enc_messages;

			int difference_score = 0;
			for (size_t i = 0; i < kTestNum; i++)
			{
				// create message buffer
				tc::ByteData msg = tc::ByteData(tests[0].enc_message.size());
				
				// encrypt using auto seed generator
				cryptor.encrypt(msg.data(), tests[0].dec_message.data(), tests[0].dec_message.size());
				
				// add message to vector
				enc_messages.push_back(msg);

				// compare this message to the previous messages
				for (size_t j = 0; j < i; j++)
				{
					difference_score += memcmp(enc_messages[i].data(), enc_messages[j].data(), tests[0].enc_message.size()) == 0;
				}
			}
		
			if (difference_score != 0)
			{
				throw tc::Exception("Failed to generate unique encrypted messages.");
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

void crypto_Rsa2048OaepSha256Encryptor_TestClass::test_DoesNothingWhenNotInit()
{
	std::cout << "[tc::crypto::Rsa2048OaepSha256Encryptor] test_DoesNothingWhenNotInit : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;
			
			bool res;
			tc::crypto::Rsa2048OaepSha256Encryptor cryptor;

			// create data
			tc::ByteData control_message = tc::io::PaddingSource(0xee, 0x20).pullData(0, 0x20);
			tc::ByteData message = tc::ByteData(control_message.data(), control_message.size());
			tc::ByteData control_block = tc::io::PaddingSource(0xdd, tc::crypto::Rsa2048OaepSha256Encryptor::kBlockSize).pullData(0, tc::crypto::Rsa2048OaepSha256Encryptor::kBlockSize);
			tc::ByteData block = tc::ByteData(control_block);
			size_t control_message_size = 0x1337;
			size_t message_size = control_message_size;

			// try to decrypt without calling initialize()
			res = cryptor.decrypt(message.data(), message_size, message.size(), block.data());

			if (res != false)
			{
				ss << "Failed: decrypt() returned true when not initialized";
				throw tc::Exception(ss.str());
			}
			if (message_size != control_message_size)
			{
				ss << "Failed: decrypt() modified message_size when not initialized";
				throw tc::Exception(ss.str());
			}
			if (memcmp(message.data(), control_message.data(), message.size()) != 0)
			{
				ss << "Failed: decrypt() operated on message when not initialized";
				throw tc::Exception(ss.str());
			}

			// try to encrypt without calling initialize()
			res = cryptor.encrypt(block.data(), message.data(), message.size());

			if (res != false)
			{
				ss << "Failed: encrypt() returned true when not initialized";
				throw tc::Exception(ss.str());
			}
			if (memcmp(block.data(), control_block.data(), block.size()) != 0)
			{
				ss << "Failed: encrypt() operated on block when not initialized";
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

void crypto_Rsa2048OaepSha256Encryptor_TestClass::test_InitializeThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Rsa2048OaepSha256Encryptor] test_InitializeThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;
			
			

			// create tests
			std::vector<RsaOaepUtil::TestVector> tests;
			RsaOaepUtil::generateRsaOaepTestVectors_Custom(tests, 2048, RsaOaepUtil::SHA256);

			tc::crypto::Rsa2048OaepSha256Encryptor cryptor;

			// reference initialize call
			//cryptor.initialize(tc::crypto::RsaPrivateKey(tests[0].key_modulus.data(), tests[0].key_modulus.size(), tests[0].key_private_exponent.data(), tests[0].key_private_exponent.size()), tests[0].label.data(), tests[0].label.size(), false);
			
			auto label = tc::io::PaddingSource(0xab, tc::crypto::Sha256Generator::kHashSize).pullData(0, tc::crypto::Sha256Generator::kHashSize-2);
			auto empty_label = tc::ByteData();

			auto key = tc::crypto::RsaPrivateKey(tests[0].key_modulus.data(), tests[0].key_modulus.size(), tests[0].key_private_exponent.data(), tests[0].key_private_exponent.size());
			auto empty_key = tc::crypto::RsaKey();
			auto bad_modulus_size_key = tc::crypto::RsaPrivateKey(tests[0].key_modulus.data(), tests[0].key_modulus.size()-2, tests[0].key_private_exponent.data(), tests[0].key_private_exponent.size());
			auto no_modulus_key = key;
			no_modulus_key.n = tc::ByteData();
			auto bad_privexp_size_key = tc::crypto::RsaPrivateKey(tests[0].key_modulus.data(), tests[0].key_modulus.size(), tests[0].key_private_exponent.data(), tests[0].key_private_exponent.size()-2);
			auto bad_pubexp_size_key = key;
			bad_pubexp_size_key.e = tc::ByteData(5);
			auto no_exponent_key = key;
			no_exponent_key.d = tc::ByteData();
			no_exponent_key.e = tc::ByteData();

			try {
				cryptor.initialize(empty_key, label.data(), label.size(), false);
				throw tc::Exception("Failed to throw ArgumentNullException where RsaKey empty");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(bad_modulus_size_key, label.data(), label.size(), false);
				throw tc::Exception("Failed to throw ArgumentNullException where RsaKey had a bad modulus size");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(no_modulus_key, label.data(), label.size(), false);
				throw tc::Exception("Failed to throw ArgumentNullException where RsaKey had a no modulus");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(bad_privexp_size_key, label.data(), label.size(), false);
				throw tc::Exception("Failed to throw ArgumentNullException where RsaKey had a bad private exponent size");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(bad_pubexp_size_key, label.data(), label.size(), false);
				throw tc::Exception("Failed to throw ArgumentNullException where RsaKey had a bad public exponent size");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(no_exponent_key, label.data(), label.size(), false);
				throw tc::Exception("Failed to throw ArgumentNullException where RsaKey had a neither public nor private exponents");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(key, label.data(), label.size(), true);
				throw tc::Exception("Failed to throw ArgumentOutOfRangeException where isLabelDigested==true but the label isn't the correct size");
			} catch(const tc::ArgumentOutOfRangeException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(key, label.data(), 0, false);
				throw tc::Exception("Failed to throw ArgumentNullException where label != nullptr but label_size == 0");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				cryptor.initialize(key, nullptr, label.size(), false);
				throw tc::Exception("Failed to throw ArgumentNullException where label == nullptr but label_size != 0");
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

void crypto_Rsa2048OaepSha256Encryptor_TestClass::test_EncryptReturnsFalseOnBadInput()
{
	std::cout << "[tc::crypto::Rsa2048OaepSha256Encryptor] test_EncryptReturnsFalseOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;
			
			// create tests
			std::vector<RsaOaepUtil::TestVector> tests;
			RsaOaepUtil::generateRsaOaepTestVectors_Custom(tests, 2048, RsaOaepUtil::SHA256);

			tc::crypto::Rsa2048OaepSha256Encryptor cryptor;

			cryptor.initialize(tc::crypto::RsaPublicKey(tests[0].key_modulus.data(), tests[0].key_modulus.size()), tests[0].label.data(), tests[0].label.size(), false);

			tc::ByteData data = tc::ByteData(tests[0].enc_message.size());

			// reference encrypt call
			//cryptor.encrypt(data.data(), tests[0].dec_message.data(), tests[0].dec_message.size(), tests[0].enc_seed.data(), tests[0].enc_seed.size());

			bool result = false;

			result = cryptor.encrypt(nullptr, tests[0].dec_message.data(), tests[0].dec_message.size(), tests[0].enc_seed.data(), tests[0].enc_seed.size());
			if (result != false)
			{
				throw tc::Exception("encrypt() did not return false where block==nullptr"); // ArgumentNullException
			}

			result = cryptor.encrypt(data.data(), nullptr, tests[0].dec_message.size(), tests[0].enc_seed.data(), tests[0].enc_seed.size());
			if (result != false)
			{
				throw tc::Exception("encrypt() did not return false where message==nullptr"); // ArgumentNullException
			}

			result = cryptor.encrypt(data.data(), tests[0].dec_message.data(), 0, tests[0].enc_seed.data(), tests[0].enc_seed.size());
			if (result != false)
			{
				throw tc::Exception("encrypt() did not return false where message_size==0"); // ArgumentOutOfRangeException
			}

			size_t max_message_size = tc::crypto::Rsa2048OaepSha256Encryptor::kBlockSize - (2 * tc::crypto::Sha256Generator::kHashSize) - 2;
			result = cryptor.encrypt(data.data(), tests[0].dec_message.data(), max_message_size+1, tests[0].enc_seed.data(), tests[0].enc_seed.size());
			if (result != false)
			{
				throw tc::Exception("encrypt() did not return false where message_size was too large to be encrypted in one RSA-OAEP block"); // ArgumentOutOfRangeException
			}

			result = cryptor.encrypt(data.data(), tests[0].dec_message.data(), tests[0].dec_message.size(), tests[0].enc_seed.data(), 0);
			if (result != false)
			{
				throw tc::Exception("encrypt() did not return false where seed_size!=HashCalculator::kHashSize"); // ArgumentOutOfRangeException
			}

			result = cryptor.encrypt(data.data(), tests[0].dec_message.data(), tests[0].dec_message.size(), nullptr, tests[0].enc_seed.size());
			if (result != false)
			{
				throw tc::Exception("encrypt() did not return false where seed==nullptr"); // ArgumentNullException
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

void crypto_Rsa2048OaepSha256Encryptor_TestClass::test_DecryptReturnsFalseOnBadInput()
{
	std::cout << "[tc::crypto::Rsa2048OaepSha256Encryptor] test_DecryptReturnsFalseOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;
			
			// create tests
			std::vector<RsaOaepUtil::TestVector> tests;
			RsaOaepUtil::generateRsaOaepTestVectors_Custom(tests, 2048, RsaOaepUtil::SHA256);

			tc::crypto::Rsa2048OaepSha256Encryptor cryptor;

			cryptor.initialize(tc::crypto::RsaPrivateKey(tests[0].key_modulus.data(), tests[0].key_modulus.size(), tests[0].key_private_exponent.data(), tests[0].key_private_exponent.size()), tests[0].label.data(), tests[0].label.size(), false);

			tc::ByteData data = tc::ByteData(tests[0].dec_message.size());
			size_t message_size = 0;

			// reference encrypt call
			//cryptor.decrypt(data.data(), message_size, data.size(), tests[0].enc_message.data());

			bool result = false;

			result = cryptor.decrypt(nullptr, message_size, data.size(), tests[0].enc_message.data());
			if (result != false)
			{
				throw tc::Exception("decrypt() did not return false where message==nullptr"); // ArgumentNullException
			}

			result = cryptor.decrypt(data.data(), message_size, 0, tests[0].enc_message.data());
			if (result != false)
			{
				throw tc::Exception("decrypt() did not return false where message_capacity==0"); // ArgumentOutOfRangeException
			}

			result = cryptor.decrypt(data.data(), message_size, data.size(), nullptr);
			if (result != false)
			{
				throw tc::Exception("decrypt() did not return false where block==nullptr"); // ArgumentNullException
			}

			result = cryptor.decrypt(data.data(), message_size, tests[0].dec_message.size()-1, tests[0].enc_message.data());
			if (result != false)
			{
				throw tc::Exception("decrypt() did not return false where message_capacity was not large enough"); // ArgumentOutOfRangeException
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