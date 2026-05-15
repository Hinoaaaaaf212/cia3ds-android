#include <tc/Exception.h>
#include <tc/io.h>
#include <tc/cli.h>
#include <fmt/core.h>

#include "crypto_Aes128CbcEncryptedStream_TestClass.h"
#include "StreamTestUtil.h"

void crypto_Aes128CbcEncryptedStream_TestClass::runAllTests(void)
{
	fmt::print("[tc::crypto::Aes128CbcEncryptedStream] START\n");
	test_CreateEmptyStream_DefaultConstructor();
	test_CreateValidStream_CreateConstructor();
	test_RunTestCases();
	fmt::print("[tc::crypto::Aes128CbcEncryptedStream] END\n");
}

void crypto_Aes128CbcEncryptedStream_TestClass::test_CreateEmptyStream_DefaultConstructor()
{
	fmt::print("[tc::crypto::Aes128CbcEncryptedStream] test_CreateEmptyStream_DefaultConstructor : ");
	try
	{
		try 
		{
			auto stream = tc::crypto::Aes128CbcEncryptedStream();

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, false, false, false);

			try
			{
				stream.read(nullptr, 0);
				throw tc::Exception(".read() did not throw tc::ObjectDisposedException for uninitialized Aes128CbcEncryptedStream");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				stream.write(nullptr, 0);
				throw tc::Exception(".write() did not throw tc::ObjectDisposedException for uninitialized Aes128CbcEncryptedStream");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				stream.seek(0, tc::io::SeekOrigin::Begin);
				throw tc::Exception(".seek() did not throw tc::ObjectDisposedException for uninitialized Aes128CbcEncryptedStream");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				stream.setLength(0);
				throw tc::Exception(".setLength() did not throw tc::ObjectDisposedException for uninitialized Aes128CbcEncryptedStream");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				stream.flush();
				throw tc::Exception(".flush() did not throw tc::ObjectDisposedException for uninitialized Aes128CbcEncryptedStream");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void crypto_Aes128CbcEncryptedStream_TestClass::test_CreateValidStream_CreateConstructor()
{
	fmt::print("[tc::crypto::Aes128CbcEncryptedStream] test_CreateValidStream_CreateConstructor : ");
	try
	{
		try 
		{
			tc::crypto::Aes128CbcEncryptedStream::key_t key;
			tc::crypto::Aes128CbcEncryptedStream::iv_t iv;

			std::shared_ptr<tc::io::IStream> base_stream;
			base_stream = std::shared_ptr<tc::io::MemoryStream>(new tc::io::MemoryStream(tc::ByteData(0x100)));

			auto stream = tc::crypto::Aes128CbcEncryptedStream(base_stream, key, iv);

			try
			{
				stream.write(nullptr, 0);
				throw tc::Exception(".write() did not throw tc::NotImplementedException for initialized Aes128CbcEncryptedStream");
			}
			catch (tc::NotImplementedException&) {
				// do nothing
			}

			try
			{
				stream.setLength(0);
				throw tc::Exception(".setLength() did not throw tc::NotImplementedException for initialized Aes128CbcEncryptedStream");
			}
			catch (tc::NotImplementedException&) {
				// do nothing
			}

			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void crypto_Aes128CbcEncryptedStream_TestClass::test_RunTestCases()
{
	fmt::print("[tc::crypto::Aes128CbcEncryptedStream] test_RunTestCases : ");
	try
	{
		try 
		{
			// get test cases
			std::vector<crypto_Aes128CbcEncryptedStream_TestClass::TestCase> test_cases;
			util_Setup_TestCases(test_cases);

			for (auto itr = test_cases.begin(); itr != test_cases.end(); itr++)
			{
				tc::crypto::Aes128CbcEncryptedStream::key_t key;
				memcpy(key.data(), itr->key.data(), itr->key.size());

				tc::crypto::Aes128CbcEncryptedStream::iv_t iv;
				memcpy(iv.data(), itr->iv.data(), itr->iv.size());

				std::shared_ptr<tc::io::IStream> base_stream;
				base_stream = std::shared_ptr<tc::io::MemoryStream>(new tc::io::MemoryStream(itr->ciphertext));


				auto stream = tc::crypto::Aes128CbcEncryptedStream(base_stream, key, iv);

				try 
				{
					StreamTestUtil::constructor_TestHelper(stream, itr->ciphertext.size(), 0, true, false, true);
					StreamTestUtil::read_TestHelper(stream, itr->read_offset, tc::io::SeekOrigin::Begin, itr->read_size, itr->read_size, itr->read_plaintext.size(), itr->read_offset + int64_t(itr->read_size), itr->read_plaintext.data());
				}
				catch (const tc::Exception& e)
				{
					throw tc::Exception(fmt::format("{} Failed: {}", itr->test_name, e.error()));
				}
				
			}

			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void crypto_Aes128CbcEncryptedStream_TestClass::util_Setup_TestCases(std::vector<crypto_Aes128CbcEncryptedStream_TestClass::TestCase>& test_cases)
{
	TestCase tmp;

	test_cases.clear();

	// Test vectors taken from NIST SP 800-38A
	tmp.key = tc::cli::FormatUtil::hexStringToBytes("2b7e151628aed2a6abf7158809cf4f3c");
	tmp.iv  = tc::cli::FormatUtil::hexStringToBytes("000102030405060708090A0B0C0D0E0F");

	tc::ByteData plaintext =  tc::cli::FormatUtil::hexStringToBytes("6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710");
	tc::ByteData ciphertext = tc::cli::FormatUtil::hexStringToBytes("7649abac8119b246cee98e9b12e9197d5086cb9b507219ee95db113a917678b273bed6b8e3c1743b7116e69e222295163ff1caa1681fac09120eca307586e1a7");

	tmp.test_name  = "Test 1";
	tmp.ciphertext = ciphertext;
	tmp.read_offset = 0x00;
	tmp.read_size = 0x10;
	tmp.read_plaintext = tc::ByteData(plaintext.data() + tmp.read_offset, tmp.read_size);
	test_cases.push_back(tmp);

	tmp.test_name  = "Test 2";
	tmp.ciphertext = ciphertext;
	tmp.read_offset = 0x10;
	tmp.read_size = 0x10;
	tmp.read_plaintext = tc::ByteData(plaintext.data() + tmp.read_offset, tmp.read_size);
	test_cases.push_back(tmp);

	tmp.test_name  = "Test 3";
	tmp.ciphertext = ciphertext;
	tmp.read_offset = 0x20;
	tmp.read_size = 0x10;
	tmp.read_plaintext = tc::ByteData(plaintext.data() + tmp.read_offset, tmp.read_size);
	test_cases.push_back(tmp);

	tmp.test_name  = "Test 4";
	tmp.ciphertext = ciphertext;
	tmp.read_offset = 0x30;
	tmp.read_size = 0x10;
	tmp.read_plaintext = tc::ByteData(plaintext.data() + tmp.read_offset, tmp.read_size);
	test_cases.push_back(tmp);

	tmp.test_name  = "Test read all blocks";
	tmp.ciphertext = ciphertext;
	tmp.read_offset = 0x0;
	tmp.read_size = 0x40;
	tmp.read_plaintext = tc::ByteData(plaintext.data() + tmp.read_offset, tmp.read_size);
	test_cases.push_back(tmp);

	tmp.test_name  = "Test un-aligned read over blocks 1-2";
	tmp.ciphertext = ciphertext;
	tmp.read_offset = 0x17;
	tmp.read_size = 0x11;
	tmp.read_plaintext = tc::ByteData(plaintext.data() + tmp.read_offset, tmp.read_size);
	test_cases.push_back(tmp);

	tmp.test_name  = "Test un-aligned read over blocks 0-3";
	tmp.ciphertext = ciphertext;
	tmp.read_offset = 0x07;
	tmp.read_size = 0x31;
	tmp.read_plaintext = tc::ByteData(plaintext.data() + tmp.read_offset, tmp.read_size);
	test_cases.push_back(tmp);

	tmp.test_name  = "Test partial block 0 read";
	tmp.ciphertext = ciphertext;
	tmp.read_offset = 0x2;
	tmp.read_size = 0x8;
	tmp.read_plaintext = tc::ByteData(plaintext.data() + tmp.read_offset, tmp.read_size);
	test_cases.push_back(tmp);

	tmp.test_name  = "Test partial block 1 read";
	tmp.ciphertext = ciphertext;
	tmp.read_offset = 0x12;
	tmp.read_size = 0x8;
	tmp.read_plaintext = tc::ByteData(plaintext.data() + tmp.read_offset, tmp.read_size);
	test_cases.push_back(tmp);

	tmp.test_name  = "Test partial block 3 read";
	tmp.ciphertext = ciphertext;
	tmp.read_offset = 0x32;
	tmp.read_size = 0x8;
	tmp.read_plaintext = tc::ByteData(plaintext.data() + tmp.read_offset, tmp.read_size);
	test_cases.push_back(tmp);
}