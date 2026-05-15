#include <iostream>
#include <sstream>
#include <fstream>

#include "crypto_HmacMd5Generator_TestClass.h"

#include <tc/Exception.h>
#include <tc/crypto/HmacMd5Generator.h>
#include <tc/cli/FormatUtil.h>

void crypto_HmacMd5Generator_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::HmacMd5Generator] START" << std::endl;
	test_Constants();
	test_SingleUpdateCall();
	test_MultiUpdateCall();
	test_UtilFunc();

	test_NoInitNoUpdateDoMac();
	test_NoInitDoUpdateDoMac();
	test_DoInitNoUpdateDoMac();
	test_DoInitNoKeyDoUpdateDoMac();
	test_DoInitNoKeyNoUpdateDoMac();

	test_CallGetMacRepeatedly();
	std::cout << "[tc::crypto::HmacMd5Generator] END" << std::endl;
}

void crypto_HmacMd5Generator_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::HmacMd5Generator] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check mac size
			static const size_t kExpectedHashSize = 16;
			if (tc::crypto::HmacMd5Generator::kMacSize != kExpectedHashSize)
			{
				ss << "kMacSize had value " << std::dec << tc::crypto::HmacMd5Generator::kMacSize << " (expected " << kExpectedHashSize << ")";
				throw tc::Exception(ss.str());
			}

			// check block size
			static const size_t kExpectedBlockSize = 64;
			if (tc::crypto::HmacMd5Generator::kBlockSize != kExpectedBlockSize)
			{
				ss << "kBlockSize had value " << std::dec << tc::crypto::HmacMd5Generator::kBlockSize << " (expected " << kExpectedBlockSize << ")";
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

void crypto_HmacMd5Generator_TestClass::test_SingleUpdateCall()
{
	std::cout << "[tc::crypto::HmacMd5Generator] test_SingleUpdateCall : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacMd5Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacMd5Generator::kMacSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				calc.initialize(test->in_key.data(), test->in_key.size());
				calc.update(test->in_data.data(), test->in_data.size());
				memset(mac.data(), 0xff, mac.size());
				calc.getMac(mac.data());
				if (memcmp(mac.data(), test->out_mac.data(), mac.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong MAC: " << tc::cli::FormatUtil::formatBytesAsString(mac, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_mac, true, "");
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

void crypto_HmacMd5Generator_TestClass::test_MultiUpdateCall()
{
	std::cout << "[tc::crypto::HmacMd5Generator] test_MultiUpdateCall : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacMd5Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacMd5Generator::kMacSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				calc.initialize(test->in_key.data(), test->in_key.size());

				// pick an offset to split the in_string at
				size_t offset = test->in_data.size() / 2;

				// update with first half
				calc.update(test->in_data.data(), offset);

				// update with second half
				calc.update(test->in_data.data() + offset, test->in_data.size() - offset);
				
				memset(mac.data(), 0xff, mac.size());
				calc.getMac(mac.data());
				if (memcmp(mac.data(), test->out_mac.data(), mac.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong MAC: " << tc::cli::FormatUtil::formatBytesAsString(mac, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_mac, true, "");
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

void crypto_HmacMd5Generator_TestClass::test_UtilFunc()
{
	std::cout << "[tc::crypto::HmacMd5Generator] test_UtilFunc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::ByteData mac = tc::ByteData(tc::crypto::HmacMd5Generator::kMacSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				memset(mac.data(), 0xff, mac.size());
				tc::crypto::GenerateHmacMd5Mac(mac.data(), (const byte_t*)test->in_data.data(), test->in_data.size(), test->in_key.data(), test->in_key.size());
				if (memcmp(mac.data(), test->out_mac.data(), mac.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong MAC: " << tc::cli::FormatUtil::formatBytesAsString(mac, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_mac, true, "");
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

void crypto_HmacMd5Generator_TestClass::test_NoInitNoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacMd5Generator] test_NoInitNoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacMd5Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacMd5Generator::kMacSize);
			tc::ByteData expected_uninitialized_mac = tc::ByteData(mac.size());
			memset(expected_uninitialized_mac.data(), 0xff, expected_uninitialized_mac.size());

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				//calc.initialize(test->in_key.data(), test->in_key.size());
				//calc.update(test->in_data.size(), test->in_data.size());
				memcpy(mac.data(), expected_uninitialized_mac.data(), mac.size());
				calc.getMac(mac.data());
				if (memcmp(mac.data(), expected_uninitialized_mac.data(), mac.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong MAC: " << tc::cli::FormatUtil::formatBytesAsString(mac, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(expected_uninitialized_mac, true, "");
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

void crypto_HmacMd5Generator_TestClass::test_NoInitDoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacMd5Generator] test_NoInitDoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacMd5Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacMd5Generator::kMacSize);
			tc::ByteData expected_uninitialized_mac = tc::ByteData(mac.size());
			memset(expected_uninitialized_mac.data(), 0xff, expected_uninitialized_mac.size());

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				//calc.initialize(test->in_key.data(), test->in_key.size());
				calc.update(test->in_data.data(), test->in_data.size());
				memcpy(mac.data(), expected_uninitialized_mac.data(), mac.size());
				calc.getMac(mac.data());
				if (memcmp(mac.data(), expected_uninitialized_mac.data(), mac.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong MAC: " << tc::cli::FormatUtil::formatBytesAsString(mac, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(expected_uninitialized_mac, true, "");
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

void crypto_HmacMd5Generator_TestClass::test_DoInitNoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacMd5Generator] test_DoInitNoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacMd5Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacMd5Generator::kMacSize);
			
			// override expected MAC for when no update() is called
			tests[0].out_mac = tc::cli::FormatUtil::hexStringToBytes("C9E99A43CD8FA24A840AA85C7CCA0061");
			tests[1].out_mac = tc::cli::FormatUtil::hexStringToBytes("60B57DA4237ED7C91B475EDDF0E798D3");
			tests[2].out_mac = tc::cli::FormatUtil::hexStringToBytes("68333B4B8FCBAD8D64D914430788E601");
			tests[3].out_mac = tc::cli::FormatUtil::hexStringToBytes("24CFC1B34D4FD3388EC723F7B6214669");
			tests[4].out_mac = tc::cli::FormatUtil::hexStringToBytes("8AAFFA8F035AF4C09CA7D1635F8CF716");
			tests[5].out_mac = tc::cli::FormatUtil::hexStringToBytes("172C1869F3E854DC888D3B2D3ADA639F");
			tests[6].out_mac = tc::cli::FormatUtil::hexStringToBytes("172C1869F3E854DC888D3B2D3ADA639F");

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				calc.initialize(test->in_key.data(), test->in_key.size());
				//calc.update(test->in_data.size(), test->in_data.size());
				memset(mac.data(), 0xff, mac.size());
				calc.getMac(mac.data());
				if (memcmp(mac.data(), test->out_mac.data(), mac.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong MAC: " << tc::cli::FormatUtil::formatBytesAsString(mac, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_mac, true, "");
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

void crypto_HmacMd5Generator_TestClass::test_DoInitNoKeyDoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacMd5Generator] test_DoInitNoKeyDoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacMd5Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacMd5Generator::kMacSize);
			
			// override expected MAC for when no key is used during initialize()
			tests[0].in_key = tc::ByteData();
			tests[0].out_mac = tc::cli::FormatUtil::hexStringToBytes("72C33C78CAC0B7A581AC263A344ED01D");
			tests[1].in_key = tc::ByteData();
			tests[1].out_mac = tc::cli::FormatUtil::hexStringToBytes("AE2E4B39F3B5EE2C8B585994294201EA");
			tests[2].in_key = tc::ByteData();
			tests[2].out_mac = tc::cli::FormatUtil::hexStringToBytes("1F265B5F8E396420867BA340A8B3AE2F");
			tests[3].in_key = tc::ByteData();
			tests[3].out_mac = tc::cli::FormatUtil::hexStringToBytes("EC0AE3C21F1BC5DD136C488FC11E62E4");
			tests[4].in_key = tc::ByteData();
			tests[4].out_mac = tc::cli::FormatUtil::hexStringToBytes("6F9F9B09EE74ABC55B72EA1003A5AE2B");
			tests[5].in_key = tc::ByteData();
			tests[5].out_mac = tc::cli::FormatUtil::hexStringToBytes("647DF53417E4E001CBD1842FB13C9AE2");
			tests[6].in_key = tc::ByteData();
			tests[6].out_mac = tc::cli::FormatUtil::hexStringToBytes("172C0788A36B21774D60D2D3B911C5D7");

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				calc.initialize(test->in_key.data(), test->in_key.size());
				calc.update(test->in_data.data(), test->in_data.size());
				memset(mac.data(), 0xff, mac.size());
				calc.getMac(mac.data());
				if (memcmp(mac.data(), test->out_mac.data(), mac.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong MAC: " << tc::cli::FormatUtil::formatBytesAsString(mac, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_mac, true, "");
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

void crypto_HmacMd5Generator_TestClass::test_DoInitNoKeyNoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacMd5Generator] test_DoInitNoKeyNoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacMd5Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacMd5Generator::kMacSize);
			
			// override expected MAC for when no key is used during initialize() and update is not called
			tests[0].in_key = tc::ByteData();
			tests[0].out_mac = tc::cli::FormatUtil::hexStringToBytes("74E6F7298A9C2D168935F58C001BAD88");
			tests[1].in_key = tc::ByteData();
			tests[1].out_mac = tc::cli::FormatUtil::hexStringToBytes("74E6F7298A9C2D168935F58C001BAD88");
			tests[2].in_key = tc::ByteData();
			tests[2].out_mac = tc::cli::FormatUtil::hexStringToBytes("74E6F7298A9C2D168935F58C001BAD88");
			tests[3].in_key = tc::ByteData();
			tests[3].out_mac = tc::cli::FormatUtil::hexStringToBytes("74E6F7298A9C2D168935F58C001BAD88");
			tests[4].in_key = tc::ByteData();
			tests[4].out_mac = tc::cli::FormatUtil::hexStringToBytes("74E6F7298A9C2D168935F58C001BAD88");
			tests[5].in_key = tc::ByteData();
			tests[5].out_mac = tc::cli::FormatUtil::hexStringToBytes("74E6F7298A9C2D168935F58C001BAD88");
			tests[6].in_key = tc::ByteData();
			tests[6].out_mac = tc::cli::FormatUtil::hexStringToBytes("74E6F7298A9C2D168935F58C001BAD88");

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				calc.initialize(test->in_key.data(), test->in_key.size());
				//calc.update(test->in_data.data(), test->in_data.size());
				memset(mac.data(), 0xff, mac.size());
				calc.getMac(mac.data());
				if (memcmp(mac.data(), test->out_mac.data(), mac.size()) != 0)
				{
					ss << "Test \"" << test->test_name << "\" Failed. Had wrong MAC: " << tc::cli::FormatUtil::formatBytesAsString(mac, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_mac, true, "");
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

void crypto_HmacMd5Generator_TestClass::test_CallGetMacRepeatedly()
{
	std::cout << "[tc::crypto::HmacMd5Generator] test_CallGetMacRepeatedly : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacMd5Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacMd5Generator::kMacSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				calc.initialize(test->in_key.data(), test->in_key.size());
				calc.update(test->in_data.data(), test->in_data.size());
				for (size_t i = 0; i < 100; i++)
				{
					// by resetting the MAC here we can tell if it is updated each time
					memset(mac.data(), 0xff, mac.size());
					calc.getMac(mac.data());
					if (memcmp(mac.data(), test->out_mac.data(), mac.size()) != 0)
					{
						ss << "Test \"" << test->test_name << "\" Failed. Had wrong MAC: " << tc::cli::FormatUtil::formatBytesAsString(mac, true, "") << " (expected " << tc::cli::FormatUtil::formatBytesAsString(test->out_mac, true, "");
						throw tc::Exception(ss.str());
					}
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

void crypto_HmacMd5Generator_TestClass::util_Setup_Rfc2202_TestCases(std::vector<crypto_HmacMd5Generator_TestClass::TestCase>& test_cases)
{
	TestCase tmp;

	test_cases.clear();

	tmp.test_name = "RFC 2202 Test 1";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b");
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("4869205468657265"); // "Hi There"
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("9294727a3638bb1c13f48ef8158bfc9d");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 2";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("4a656665"); // "Jefe"
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("7768617420646f2079612077616e7420666f72206e6f7468696e673f"); // "what do ya want for nothing?"
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("750c783e6ab0b503eaa86e310a5db738");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 3";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"); // 50 x 0xdd
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("56be34521d144c88dbb8c733f0e8b3f6");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 4";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("0102030405060708090a0b0c0d0e0f10111213141516171819");
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"); // 50 x 0xcd
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("697eaf0aca3a3aea3a75164746ffaa79");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 5";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c");
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("546573742057697468205472756e636174696f6e"); // "Test With Truncation"
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("56461ef2342edc00f9bab995690efd4c");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 6";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); // 80 x 0xaa
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b6579202d2048617368204b6579204669727374"); // "Test Using Larger Than Block-Size Key - Hash Key First"
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("6b1ab7fe4bd7bf8f0b62e6ce61b9d0cd");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 7";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); // 80 x 0xaa
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b657920616e64204c6172676572205468616e204f6e6520426c6f636b2d53697a652044617461"); // "Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data"
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("6f630fad67cda0ee1fb1f562db3aa53e");
	test_cases.push_back(tmp);
}