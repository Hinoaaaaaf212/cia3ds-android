#include <iostream>
#include <sstream>
#include <fstream>

#include "crypto_HmacSha1Generator_TestClass.h"

#include <tc/Exception.h>
#include <tc/crypto/HmacSha1Generator.h>
#include <tc/cli/FormatUtil.h>

void crypto_HmacSha1Generator_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::HmacSha1Generator] START" << std::endl;
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
	std::cout << "[tc::crypto::HmacSha1Generator] END" << std::endl;
}

void crypto_HmacSha1Generator_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::HmacSha1Generator] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check mac size
			static const size_t kExpectedHashSize = 20;
			if (tc::crypto::HmacSha1Generator::kMacSize != kExpectedHashSize)
			{
				ss << "kMacSize had value " << std::dec << tc::crypto::HmacSha1Generator::kMacSize << " (expected " << kExpectedHashSize << ")";
				throw tc::Exception(ss.str());
			}

			// check block size
			static const size_t kExpectedBlockSize = 64;
			if (tc::crypto::HmacSha1Generator::kBlockSize != kExpectedBlockSize)
			{
				ss << "kBlockSize had value " << std::dec << tc::crypto::HmacSha1Generator::kBlockSize << " (expected " << kExpectedBlockSize << ")";
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

void crypto_HmacSha1Generator_TestClass::test_SingleUpdateCall()
{
	std::cout << "[tc::crypto::HmacSha1Generator] test_SingleUpdateCall : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacSha1Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha1Generator::kMacSize);

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

void crypto_HmacSha1Generator_TestClass::test_MultiUpdateCall()
{
	std::cout << "[tc::crypto::HmacSha1Generator] test_MultiUpdateCall : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacSha1Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha1Generator::kMacSize);

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

void crypto_HmacSha1Generator_TestClass::test_UtilFunc()
{
	std::cout << "[tc::crypto::HmacSha1Generator] test_UtilFunc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha1Generator::kMacSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				memset(mac.data(), 0xff, mac.size());
				tc::crypto::GenerateHmacSha1Mac(mac.data(), (const byte_t*)test->in_data.data(), test->in_data.size(), test->in_key.data(), test->in_key.size());
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

void crypto_HmacSha1Generator_TestClass::test_NoInitNoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacSha1Generator] test_NoInitNoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacSha1Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha1Generator::kMacSize);
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

void crypto_HmacSha1Generator_TestClass::test_NoInitDoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacSha1Generator] test_NoInitDoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacSha1Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha1Generator::kMacSize);
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

void crypto_HmacSha1Generator_TestClass::test_DoInitNoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacSha1Generator] test_DoInitNoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacSha1Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha1Generator::kMacSize);
			
			// override expected MAC for when no update() is called
			tests[0].out_mac = tc::cli::FormatUtil::hexStringToBytes("123FD78BDA0100786AE86B76F50F01BD18E477F3");
			tests[1].out_mac = tc::cli::FormatUtil::hexStringToBytes("09D9E59D72239E62A8155C583D52743DE9B7231A");
			tests[2].out_mac = tc::cli::FormatUtil::hexStringToBytes("4C2E4B8144C34521B2487190F0862E7E0978D42A");
			tests[3].out_mac = tc::cli::FormatUtil::hexStringToBytes("932C2468DB83EEB23B6B8F1EDE8BE57136BA9C99");
			tests[4].out_mac = tc::cli::FormatUtil::hexStringToBytes("3DF4B2ABD4FCFE0FF5EFA57D0E98CCA85F4B8C17");
			tests[5].out_mac = tc::cli::FormatUtil::hexStringToBytes("6FB70A345B84A347A0CA3B58B4F8DDC6AB1EC61F");
			tests[6].out_mac = tc::cli::FormatUtil::hexStringToBytes("6FB70A345B84A347A0CA3B58B4F8DDC6AB1EC61F");

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

void crypto_HmacSha1Generator_TestClass::test_DoInitNoKeyDoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacSha1Generator] test_DoInitNoKeyDoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacSha1Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha1Generator::kMacSize);
			
			// override expected MAC for when no key is used during initialize()
			tests[0].in_key = tc::ByteData();
			tests[0].out_mac = tc::cli::FormatUtil::hexStringToBytes("69536CC84EEE5FE51C5B051AFF8485F5C9EF0B58");
			tests[1].in_key = tc::ByteData();
			tests[1].out_mac = tc::cli::FormatUtil::hexStringToBytes("22E999C60F94D0F2D635CA4CF1B174E5CB514D38");
			tests[2].in_key = tc::ByteData();
			tests[2].out_mac = tc::cli::FormatUtil::hexStringToBytes("3EA23ADD7131920CEDD9FA0B7ED130F9E0320B1B");
			tests[3].in_key = tc::ByteData();
			tests[3].out_mac = tc::cli::FormatUtil::hexStringToBytes("74C78AEB607055FF54D8DFCCF1A82CA011ED7A77");
			tests[4].in_key = tc::ByteData();
			tests[4].out_mac = tc::cli::FormatUtil::hexStringToBytes("4ED74D69F9942A1852037F4D8F4F8D0AA680AFDC");
			tests[5].in_key = tc::ByteData();
			tests[5].out_mac = tc::cli::FormatUtil::hexStringToBytes("061B442BBD9AAC68E1AC811EDD8CA83D0586C766");
			tests[6].in_key = tc::ByteData();
			tests[6].out_mac = tc::cli::FormatUtil::hexStringToBytes("69C4969EBA33E494509E07AE006234EEF4CD7624");

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

void crypto_HmacSha1Generator_TestClass::test_DoInitNoKeyNoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacSha1Generator] test_DoInitNoKeyNoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacSha1Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha1Generator::kMacSize);
			
			// override expected MAC for when no key is used during initialize() and update is not called
			tests[0].in_key = tc::ByteData();
			tests[0].out_mac = tc::cli::FormatUtil::hexStringToBytes("FBDB1D1B18AA6C08324B7D64B71FB76370690E1D");
			tests[1].in_key = tc::ByteData();
			tests[1].out_mac = tc::cli::FormatUtil::hexStringToBytes("FBDB1D1B18AA6C08324B7D64B71FB76370690E1D");
			tests[2].in_key = tc::ByteData();
			tests[2].out_mac = tc::cli::FormatUtil::hexStringToBytes("FBDB1D1B18AA6C08324B7D64B71FB76370690E1D");
			tests[3].in_key = tc::ByteData();
			tests[3].out_mac = tc::cli::FormatUtil::hexStringToBytes("FBDB1D1B18AA6C08324B7D64B71FB76370690E1D");
			tests[4].in_key = tc::ByteData();
			tests[4].out_mac = tc::cli::FormatUtil::hexStringToBytes("FBDB1D1B18AA6C08324B7D64B71FB76370690E1D");
			tests[5].in_key = tc::ByteData();
			tests[5].out_mac = tc::cli::FormatUtil::hexStringToBytes("FBDB1D1B18AA6C08324B7D64B71FB76370690E1D");
			tests[6].in_key = tc::ByteData();
			tests[6].out_mac = tc::cli::FormatUtil::hexStringToBytes("FBDB1D1B18AA6C08324B7D64B71FB76370690E1D");

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

void crypto_HmacSha1Generator_TestClass::test_CallGetMacRepeatedly()
{
	std::cout << "[tc::crypto::HmacSha1Generator] test_CallGetMacRepeatedly : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc2202_TestCases(tests);

			tc::crypto::HmacSha1Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha1Generator::kMacSize);

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

void crypto_HmacSha1Generator_TestClass::util_Setup_Rfc2202_TestCases(std::vector<crypto_HmacSha1Generator_TestClass::TestCase>& test_cases)
{
	TestCase tmp;

	test_cases.clear();

	tmp.test_name = "RFC 2202 Test 1";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b");
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("4869205468657265"); // "Hi There"
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("b617318655057264e28bc0b6fb378c8ef146be00");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 2";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("4a656665"); // "Jefe"
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("7768617420646f2079612077616e7420666f72206e6f7468696e673f"); // "what do ya want for nothing?"
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("effcdf6ae5eb2fa2d27416d5f184df9c259a7c79");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 3";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"); // 50 x 0xdd
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("125d7342b9ac11cd91a39af48aa17b4f63f175d3");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 4";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("0102030405060708090a0b0c0d0e0f10111213141516171819");
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"); // 50 x 0xcd
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("4c9007f4026250c6bc8414f9bf50c86c2d7235da");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 5";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c");
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("546573742057697468205472756e636174696f6e"); // "Test With Truncation"
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("4c1a03424b55e07fe7f27be1d58bb9324a9a5a04");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 6";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); // 80 x 0xaa
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b6579202d2048617368204b6579204669727374"); // "Test Using Larger Than Block-Size Key - Hash Key First"
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("aa4ae5e15272d00e95705637ce8a3b55ed402112");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 7";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); // 80 x 0xaa
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b657920616e64204c6172676572205468616e204f6e6520426c6f636b2d53697a652044617461"); // "Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data"
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("e8e99d0f45237d786d6bbaa7965c7808bbff1a91");
	test_cases.push_back(tmp);
}