#include <iostream>
#include <sstream>
#include <fstream>

#include "crypto_Rsa1024PssSha256Signer_TestClass.h"
#include "RsaPssUtil.h"

#include <tc/Exception.h>
#include <tc/crypto/RsaPssSha256Signer.h>
#include <tc/cli/FormatUtil.h>

#include <tc/io/PaddingSource.h>

void crypto_Rsa1024PssSha256Signer_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::Rsa1024PssSha256Signer] START" << std::endl;
	test_Constants();
	test_UseClassSign();
	test_UseClassVerify();
	test_UseUtilFuncSign();
	test_UseUtilFuncVerify();

	test_DoesNothingWhenNotInit();
	test_InitializeThrowsExceptionOnBadInput();
	test_SignReturnsFalseOnBadInput();
	test_VerifyReturnsFalseOnBadInput();
	std::cout << "[tc::crypto::Rsa1024PssSha256Signer] END" << std::endl;
}

void crypto_Rsa1024PssSha256Signer_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::Rsa1024PssSha256Signer] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check signature size
			static const size_t kExpectedSignatureSize = 1024 >> 3;
			if (tc::crypto::Rsa1024PssSha256Signer::kSignatureSize != kExpectedSignatureSize)
			{
				ss << "kSignatureSize had value " << std::dec << tc::crypto::Rsa1024PssSha256Signer::kSignatureSize << " (expected " << kExpectedSignatureSize << ")";
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

void crypto_Rsa1024PssSha256Signer_TestClass::test_UseClassSign()
{
	std::cout << "[tc::crypto::Rsa1024PssSha256Signer] test_UseClassSign : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<RsaPssUtil::TestVector> tests;
			RsaPssUtil::generateRsaPssTestVectors_Custom(tests, 1024, RsaPssUtil::SHA256);

			if (tests.begin() == tests.end())
			{
				throw tc::Exception("No tests");
			}

			tc::crypto::Rsa1024PssSha256Signer signer;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData signature = tc::ByteData(test->signature.size());
				
				// initialize
				signer.initialize(tc::crypto::RsaPrivateKey(test->key_modulus.data(), test->key_modulus.size(), test->key_private_exponent.data(), test->key_private_exponent.size()));
				
				// clear data
				memset(signature.data(), 0xff, signature.size());

				// test sign
				bool result = signer.sign(signature.data(), test->message_digest.data(), test->salt.data(), test->salt.size());
				if (result == false)
				{
					ss << "Test \"" << test->test_name << "\" Failed: sign() returned false";
					throw tc::Exception(ss.str());
				}
				if (memcmp(signature.data(), test->signature.data(), signature.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed: " << tc::cli::FormatUtil::formatBytesAsString(signature, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->signature, true, "") << ")";
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

void crypto_Rsa1024PssSha256Signer_TestClass::test_UseClassVerify()
{
	std::cout << "[tc::crypto::Rsa1024PssSha256Signer] test_UseClassVerify : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<RsaPssUtil::TestVector> tests;
			RsaPssUtil::generateRsaPssTestVectors_Custom(tests, 1024, RsaPssUtil::SHA256);

			if (tests.begin() == tests.end())
			{
				throw tc::Exception("No tests");
			}

			tc::crypto::Rsa1024PssSha256Signer signer;

			for (auto test = tests.begin(); test != tests.end(); test++)
			{				
				// initialize
				signer.initialize(tc::crypto::RsaPublicKey(test->key_modulus.data(), test->key_modulus.size()));

				// test verify
				bool result = signer.verify(test->signature.data(), test->message_digest.data());
				if (result == false)
				{
					ss << "Test \"" << test->test_name << "\" Failed: verify() returned false";
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

void crypto_Rsa1024PssSha256Signer_TestClass::test_UseUtilFuncSign()
{
	std::cout << "[tc::crypto::Rsa1024PssSha256Signer] test_UseUtilFuncSign : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<RsaPssUtil::TestVector> tests;
			RsaPssUtil::generateRsaPssTestVectors_Custom(tests, 1024, RsaPssUtil::SHA256);

			if (tests.begin() == tests.end())
			{
				throw tc::Exception("No tests");
			}

			// signing using the util function does not support specifying the salt, so we'll have to do some `lite` validation using the verify utility.
			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				tc::ByteData signature = tc::ByteData(test->signature.size());
				
				// clear data
				memset(signature.data(), 0xff, signature.size());

				// test sign
				bool result = tc::crypto::SignRsa1024PssSha256(signature.data(), test->message_digest.data(), tc::crypto::RsaPrivateKey(test->key_modulus.data(), test->key_modulus.size(), test->key_private_exponent.data(), test->key_private_exponent.size()));
				if (result == false)
				{
					ss << "Test \"" << test->test_name << "\" Failed: sign() returned false";
					throw tc::Exception(ss.str());
				}

				// try to verify signature
				result = tc::crypto::VerifyRsa1024PssSha256(signature.data(), test->message_digest.data(), tc::crypto::RsaPublicKey(test->key_modulus.data(), test->key_modulus.size())); //(dec_data.data(), message_size, dec_data.size(), enc_data.data(), tc::crypto::RsaPrivateKey(test->key_modulus.data(), test->key_modulus.size(), test->key_private_exponent.data(), test->key_private_exponent.size()), test->label.data(), test->label.size(), test->label_is_digested);
				if (result != true)
				{
					ss << "Test \"" << test->test_name << "\" Failed: signature could not be verified";
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

void crypto_Rsa1024PssSha256Signer_TestClass::test_UseUtilFuncVerify()
{
	std::cout << "[tc::crypto::Rsa1024PssSha256Signer] test_UseUtilFuncVerify : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<RsaPssUtil::TestVector> tests;
			RsaPssUtil::generateRsaPssTestVectors_Custom(tests, 1024, RsaPssUtil::SHA256);

			if (tests.begin() == tests.end())
			{
				throw tc::Exception("No tests");
			}

			for (auto test = tests.begin(); test != tests.end(); test++)
			{				
				// test verify
				bool result = tc::crypto::VerifyRsa1024PssSha256(test->signature.data(), test->message_digest.data(), tc::crypto::RsaPublicKey(test->key_modulus.data(), test->key_modulus.size()));
				if (result == false)
				{
					ss << "Test \"" << test->test_name << "\" Failed: verify() returned false";
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

void crypto_Rsa1024PssSha256Signer_TestClass::test_DoesNothingWhenNotInit()
{
	std::cout << "[tc::crypto::Rsa1024PssSha256Signer] test_DoesNothingWhenNotInit : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<RsaPssUtil::TestVector> tests;
			RsaPssUtil::generateRsaPssTestVectors_Custom(tests, 1024, RsaPssUtil::SHA256);

			if (tests.begin() == tests.end())
			{
				throw tc::Exception("No tests");
			}
			
			bool res;
			tc::crypto::Rsa1024PssSha256Signer signer;

			// create data
			tc::ByteData hash = tc::ByteData(tests[0].message_digest.size());
			tc::ByteData control_signature = tc::io::PaddingSource(0xee, tests[0].signature.size()).pullData(0, 0x20);
			tc::ByteData signature = tc::ByteData(control_signature.data(), control_signature.size());
			

			// try to sign without calling initialize()
			res = signer.sign(signature.data(), hash.data());
			if (res != false)
			{
				ss << "Failed: sign() returned true when not initialized";
				throw tc::Exception(ss.str());
			}
			if (memcmp(signature.data(), control_signature.data(), signature.size()) != 0)
			{
				ss << "Failed: sign() operated on message when not initialized";
				throw tc::Exception(ss.str());
			}

			// try to verify without calling initialize()
			res = signer.verify(signature.data(), hash.data());
			if (res != false)
			{
				ss << "Failed: verify() returned true when not initialized";
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

void crypto_Rsa1024PssSha256Signer_TestClass::test_InitializeThrowsExceptionOnBadInput()
{
	std::cout << "[tc::crypto::Rsa1024PssSha256Signer] test_InitializeThrowsExceptionOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<RsaPssUtil::TestVector> tests;
			RsaPssUtil::generateRsaPssTestVectors_Custom(tests, 1024, RsaPssUtil::SHA256);

			tc::crypto::Rsa1024PssSha256Signer signer;

			// reference initialize call
			//signer.initialize(tc::crypto::RsaPrivateKey(tests[0].key_modulus.data(), tests[0].key_modulus.size(), tests[0].key_private_exponent.data(), tests[0].key_private_exponent.size()));

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
				signer.initialize(empty_key);
				throw tc::Exception("Failed to throw ArgumentNullException where RsaKey empty");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				signer.initialize(bad_modulus_size_key);
				throw tc::Exception("Failed to throw ArgumentNullException where RsaKey had a bad modulus size");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				signer.initialize(no_modulus_key);
				throw tc::Exception("Failed to throw ArgumentNullException where RsaKey had a no modulus");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				signer.initialize(bad_privexp_size_key);
				throw tc::Exception("Failed to throw ArgumentNullException where RsaKey had a bad private exponent size");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				signer.initialize(bad_pubexp_size_key);
				throw tc::Exception("Failed to throw ArgumentNullException where RsaKey had a bad public exponent size");
			} catch(const tc::ArgumentNullException&) {
				// all good if this was thrown.
			}

			try {
				signer.initialize(no_exponent_key);
				throw tc::Exception("Failed to throw ArgumentNullException where RsaKey had a neither public nor private exponents");
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

void crypto_Rsa1024PssSha256Signer_TestClass::test_SignReturnsFalseOnBadInput()
{
	std::cout << "[tc::crypto::Rsa1024PssSha256Signer] test_SignReturnsFalseOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;
			
			// create tests
			std::vector<RsaPssUtil::TestVector> tests;
			RsaPssUtil::generateRsaPssTestVectors_Custom(tests, 1024, RsaPssUtil::SHA256);

			tc::crypto::Rsa1024PssSha256Signer signer;

			signer.initialize(tc::crypto::RsaPrivateKey(tests[0].key_modulus.data(), tests[0].key_modulus.size(), tests[0].key_private_exponent.data(), tests[0].key_private_exponent.size()));

			tc::ByteData signature = tc::ByteData(tests[0].signature.size());

			// reference sign call
			//signer.sign(signature.data(), tests[0].message_digest.data());

			bool result = false;

			result = signer.sign(nullptr, tests[0].message_digest.data());
			if (result != false)
			{
				throw tc::Exception("sign() did not return false where signature==nullptr"); // ArgumentNullException
			}

			result = signer.sign(signature.data(), nullptr);
			if (result != false)
			{
				throw tc::Exception("sign() did not return false where message_digest==nullptr"); // ArgumentNullException
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

void crypto_Rsa1024PssSha256Signer_TestClass::test_VerifyReturnsFalseOnBadInput()
{
	std::cout << "[tc::crypto::Rsa1024PssSha256Signer] test_VerifyReturnsFalseOnBadInput : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;
			
			// create tests
			std::vector<RsaPssUtil::TestVector> tests;
			RsaPssUtil::generateRsaPssTestVectors_Custom(tests, 1024, RsaPssUtil::SHA256);

			tc::crypto::Rsa1024PssSha256Signer signer;

			signer.initialize(tc::crypto::RsaPublicKey(tests[0].key_modulus.data(), tests[0].key_modulus.size()));

			// reference verify call
			//signer.verify(tests[0].signature.data(), tests[0].message_digest.data());

			bool result = false;

			result = signer.verify(nullptr, tests[0].message_digest.data());
			if (result != false)
			{
				throw tc::Exception("encrypt() did not return false where signature==nullptr"); // ArgumentNullException
			}

			result = signer.verify(tests[0].signature.data(), nullptr);
			if (result != false)
			{
				throw tc::Exception("encrypt() did not return false where message_digest==nullptr"); // ArgumentNullException
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