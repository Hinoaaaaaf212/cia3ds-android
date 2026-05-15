#include <iostream>
#include <sstream>
#include <fstream>

#include "crypto_HmacSha512Generator_TestClass.h"

#include <tc/Exception.h>
#include <tc/crypto/HmacSha512Generator.h>
#include <tc/cli/FormatUtil.h>

void crypto_HmacSha512Generator_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::HmacSha512Generator] START" << std::endl;
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
	std::cout << "[tc::crypto::HmacSha512Generator] END" << std::endl;
}

void crypto_HmacSha512Generator_TestClass::test_Constants()
{
	std::cout << "[tc::crypto::HmacSha512Generator] test_Constants : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// check mac size
			static const size_t kExpectedHashSize = 64;
			if (tc::crypto::HmacSha512Generator::kMacSize != kExpectedHashSize)
			{
				ss << "kMacSize had value " << std::dec << tc::crypto::HmacSha512Generator::kMacSize << " (expected " << kExpectedHashSize << ")";
				throw tc::Exception(ss.str());
			}

			// check block size
			static const size_t kExpectedBlockSize = 128;
			if (tc::crypto::HmacSha512Generator::kBlockSize != kExpectedBlockSize)
			{
				ss << "kBlockSize had value " << std::dec << tc::crypto::HmacSha512Generator::kBlockSize << " (expected " << kExpectedBlockSize << ")";
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

void crypto_HmacSha512Generator_TestClass::test_SingleUpdateCall()
{
	std::cout << "[tc::crypto::HmacSha512Generator] test_SingleUpdateCall : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc4231_TestCases(tests);

			tc::crypto::HmacSha512Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha512Generator::kMacSize);

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

void crypto_HmacSha512Generator_TestClass::test_MultiUpdateCall()
{
	std::cout << "[tc::crypto::HmacSha512Generator] test_MultiUpdateCall : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc4231_TestCases(tests);

			tc::crypto::HmacSha512Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha512Generator::kMacSize);

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

void crypto_HmacSha512Generator_TestClass::test_UtilFunc()
{
	std::cout << "[tc::crypto::HmacSha512Generator] test_UtilFunc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc4231_TestCases(tests);

			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha512Generator::kMacSize);

			for (auto test = tests.begin(); test != tests.end(); test++)
			{
				memset(mac.data(), 0xff, mac.size());
				tc::crypto::GenerateHmacSha512Mac(mac.data(), (const byte_t*)test->in_data.data(), test->in_data.size(), test->in_key.data(), test->in_key.size());
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

void crypto_HmacSha512Generator_TestClass::test_NoInitNoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacSha512Generator] test_NoInitNoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc4231_TestCases(tests);

			tc::crypto::HmacSha512Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha512Generator::kMacSize);
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

void crypto_HmacSha512Generator_TestClass::test_NoInitDoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacSha512Generator] test_NoInitDoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc4231_TestCases(tests);

			tc::crypto::HmacSha512Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha512Generator::kMacSize);
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

void crypto_HmacSha512Generator_TestClass::test_DoInitNoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacSha512Generator] test_DoInitNoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc4231_TestCases(tests);

			tc::crypto::HmacSha512Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha512Generator::kMacSize);
			
			// override expected MAC for when no update() is called
			tests[0].out_mac = tc::cli::FormatUtil::hexStringToBytes("AD8DA3D882AF6E9B872457ADCDD638E9B87AF44825425085F8CE81A4122BAB781B92F5AB92AC24948AD369F86558FD469CA3F4861CB0F0DFB33154428ED03DFB");
			tests[1].out_mac = tc::cli::FormatUtil::hexStringToBytes("B9D14C51A6D4DD41604EB06C9C240F1F64F143B5CFDEA37129B28BB75D1371D326FC219216171261A84E6C05707CD3BE0F61E0A973A33F706D190DB9ACFFC68F");
			tests[2].out_mac = tc::cli::FormatUtil::hexStringToBytes("6393B26D6D510B77741BEC0037A40B3A31071DB90ABF9A9E857B9CA643F7AF5F2AF35201608D2E3F14394AAAEA46DBAEC3E7A4EBAD54D310DB7659D102F4C73A");
			tests[3].out_mac = tc::cli::FormatUtil::hexStringToBytes("10CDF916AEE81A887E7F46BF329A18715A959E5E4EAAED386E7AE8779FC56F1CA6543219476B55D767D6E34D8EF1C76F251C95719346A588FBDDEED2FC3F013E");
			tests[4].out_mac = tc::cli::FormatUtil::hexStringToBytes("2BAD6402A42592B704DAC5BD90A559ACF444D75B7E6C9C18D44BC02FF04C94CCD180F6B71F86B0A2D853F9146F5605B8F5807006EDE0F7F35370EEF7D50FBA16");
			tests[5].out_mac = tc::cli::FormatUtil::hexStringToBytes("264428A9B95825C9ACD9D71AAC552BD95E9BD00169C02D3921DBD53A0FDF6F0E81A19EF54E7B143F131CC5FFD5896EEF3CDCF57066A70DA3698D826A1B6366DD");
			tests[6].out_mac = tc::cli::FormatUtil::hexStringToBytes("264428A9B95825C9ACD9D71AAC552BD95E9BD00169C02D3921DBD53A0FDF6F0E81A19EF54E7B143F131CC5FFD5896EEF3CDCF57066A70DA3698D826A1B6366DD");

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

void crypto_HmacSha512Generator_TestClass::test_DoInitNoKeyDoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacSha512Generator] test_DoInitNoKeyDoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc4231_TestCases(tests);

			tc::crypto::HmacSha512Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha512Generator::kMacSize);
			
			// override expected MAC for when no key is used during initialize()
			tests[0].in_key = tc::ByteData();
			tests[0].out_mac = tc::cli::FormatUtil::hexStringToBytes("F7688A104326D36C1940F6D28D746C0661D383E0D14FE8A04649444777610F5DD9565A36846AB9E9E734CF380D3A070D8EF021B5F3A50C481710A464968E3419");
			tests[1].in_key = tc::ByteData();
			tests[1].out_mac = tc::cli::FormatUtil::hexStringToBytes("1263AC6489324BDD55FEAD956DC2BE547BECE2B699214B63B0270143D740AD638F3CC6FEEDC740F8D9AC44866DF12A0073D57214EB393F692B4AE17EDF30D861");
			tests[2].in_key = tc::ByteData();
			tests[2].out_mac = tc::cli::FormatUtil::hexStringToBytes("364113906C7268F6C0C3F4977411CF34151DA703DBF6E0D3CD2A2179FA0446060BF5017D7F41330429AF17FAA4BCD4C3DA1E654767670EC177239C39B3EEF086");
			tests[3].in_key = tc::ByteData();
			tests[3].out_mac = tc::cli::FormatUtil::hexStringToBytes("17FD4AD25B6D1979AB38D833053B1B44FA99E9B410D5BC35541D6AA18A5AC8964B6B86C9C01ED10212BA45EFFB161C8676FD26EF28C557A27003294325275E9D");
			tests[4].in_key = tc::ByteData();
			tests[4].out_mac = tc::cli::FormatUtil::hexStringToBytes("6D3B39DCD123E86481D138C3237EE179329BEFB7FC82515FB6B694B6DF6E88A1827150E31366BB133A622527DAAA865E9E06BDFD0D9BDB574645292DDB7FE49E");
			tests[5].in_key = tc::ByteData();
			tests[5].out_mac = tc::cli::FormatUtil::hexStringToBytes("7E129DE32C9CBF193A88BF5107EC58A702D646F3A1ACF1ADE5631D950E96CAEC035C05DA4953F3063917954419889EFC27BA89E0B5B7767A19D36780F646BA24");
			tests[6].in_key = tc::ByteData();
			tests[6].out_mac = tc::cli::FormatUtil::hexStringToBytes("D9B8EE3286A49C2B649624D93E04322F0D29E9F49CE77BEF1E13C40553A3D01EAB341F67F1F208CE10FCBC696DE5BB24EC3E1F361A12485060CE561C54CC85CB");

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

void crypto_HmacSha512Generator_TestClass::test_DoInitNoKeyNoUpdateDoMac()
{
	std::cout << "[tc::crypto::HmacSha512Generator] test_DoInitNoKeyNoUpdateDoMac : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc4231_TestCases(tests);

			tc::crypto::HmacSha512Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha512Generator::kMacSize);
			
			// override expected MAC for when no key is used during initialize() and update is not called
			tests[0].in_key = tc::ByteData();
			tests[0].out_mac = tc::cli::FormatUtil::hexStringToBytes("B936CEE86C9F87AA5D3C6F2E84CB5A4239A5FE50480A6EC66B70AB5B1F4AC6730C6C515421B327EC1D69402E53DFB49AD7381EB067B338FD7B0CB22247225D47");
			tests[1].in_key = tc::ByteData();
			tests[1].out_mac = tc::cli::FormatUtil::hexStringToBytes("B936CEE86C9F87AA5D3C6F2E84CB5A4239A5FE50480A6EC66B70AB5B1F4AC6730C6C515421B327EC1D69402E53DFB49AD7381EB067B338FD7B0CB22247225D47");
			tests[2].in_key = tc::ByteData();
			tests[2].out_mac = tc::cli::FormatUtil::hexStringToBytes("B936CEE86C9F87AA5D3C6F2E84CB5A4239A5FE50480A6EC66B70AB5B1F4AC6730C6C515421B327EC1D69402E53DFB49AD7381EB067B338FD7B0CB22247225D47");
			tests[3].in_key = tc::ByteData();
			tests[3].out_mac = tc::cli::FormatUtil::hexStringToBytes("B936CEE86C9F87AA5D3C6F2E84CB5A4239A5FE50480A6EC66B70AB5B1F4AC6730C6C515421B327EC1D69402E53DFB49AD7381EB067B338FD7B0CB22247225D47");
			tests[4].in_key = tc::ByteData();
			tests[4].out_mac = tc::cli::FormatUtil::hexStringToBytes("B936CEE86C9F87AA5D3C6F2E84CB5A4239A5FE50480A6EC66B70AB5B1F4AC6730C6C515421B327EC1D69402E53DFB49AD7381EB067B338FD7B0CB22247225D47");
			tests[5].in_key = tc::ByteData();
			tests[5].out_mac = tc::cli::FormatUtil::hexStringToBytes("B936CEE86C9F87AA5D3C6F2E84CB5A4239A5FE50480A6EC66B70AB5B1F4AC6730C6C515421B327EC1D69402E53DFB49AD7381EB067B338FD7B0CB22247225D47");
			tests[6].in_key = tc::ByteData();
			tests[6].out_mac = tc::cli::FormatUtil::hexStringToBytes("B936CEE86C9F87AA5D3C6F2E84CB5A4239A5FE50480A6EC66B70AB5B1F4AC6730C6C515421B327EC1D69402E53DFB49AD7381EB067B338FD7B0CB22247225D47");

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

void crypto_HmacSha512Generator_TestClass::test_CallGetMacRepeatedly()
{
	std::cout << "[tc::crypto::HmacSha512Generator] test_CallGetMacRepeatedly : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create tests
			std::vector<TestCase> tests;
			util_Setup_Rfc4231_TestCases(tests);

			tc::crypto::HmacSha512Generator calc;
			tc::ByteData mac = tc::ByteData(tc::crypto::HmacSha512Generator::kMacSize);

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

void crypto_HmacSha512Generator_TestClass::util_Setup_Rfc4231_TestCases(std::vector<crypto_HmacSha512Generator_TestClass::TestCase>& test_cases)
{
	TestCase tmp;

	test_cases.clear();

	tmp.test_name = "RFC 2202 Test 1";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b");
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("4869205468657265"); // "Hi There"
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("87aa7cdea5ef619d4ff0b4241a1d6cb02379f4e2ce4ec2787ad0b30545e17cdedaa833b7d6b8a702038b274eaea3f4e4be9d914eeb61f1702e696c203a126854");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 2";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("4a656665"); // "Jefe"
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("7768617420646f2079612077616e7420666f72206e6f7468696e673f"); // "what do ya want for nothing?"
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("164b7a7bfcf819e2e395fbe73b56e0a387bd64222e831fd610270cd7ea2505549758bf75c05a994a6d034f65f8f0e6fdcaeab1a34d4a6b4b636e070a38bce737");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 3";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"); // 50 x 0xdd
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("fa73b0089d56a284efb0f0756c890be9b1b5dbdd8ee81a3655f83e33b2279d39bf3e848279a722c806b485a47e67c807b946a337bee8942674278859e13292fb");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 4";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("0102030405060708090a0b0c0d0e0f10111213141516171819");
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"); // 50 x 0xcd
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("b0ba465637458c6990e5a8c5f61d4af7e576d97ff94b872de76f8050361ee3dba91ca5c11aa25eb4d679275cc5788063a5f19741120c4f2de2adebeb10a298dd");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 5";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c");
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("546573742057697468205472756e636174696f6e"); // "Test With Truncation"
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("415FAD6271580A531D4179BC891D87A650188707922A4FBB36663A1EB16DA008711C5B50DDD0FC235084EB9D3364A1454FB2EF67CD1D29FE6773068EA266E96B");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 6";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); // 131 x 0xaa
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b6579202d2048617368204b6579204669727374"); // "Test Using Larger Than Block-Size Key - Hash Key First"
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("80b24263c7c1a3ebb71493c1dd7be8b49b46d1f41b4aeec1121b013783f8f3526b56d037e05f2598bd0fd2215d6a1e5295e64f73f63f0aec8b915a985d786598");
	test_cases.push_back(tmp);

	tmp.test_name = "RFC 2202 Test 7";
	tmp.in_key = tc::cli::FormatUtil::hexStringToBytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); // 131 x 0xaa
	tmp.in_data = tc::cli::FormatUtil::hexStringToBytes("5468697320697320612074657374207573696e672061206c6172676572207468616e20626c6f636b2d73697a65206b657920616e642061206c6172676572207468616e20626c6f636b2d73697a6520646174612e20546865206b6579206e6565647320746f20626520686173686564206265666f7265206265696e6720757365642062792074686520484d414320616c676f726974686d2e"); // "This is a test using a larger than block-size key and a larger than block-size data. The key neds to be hashed before being used by the HMAC algorithm."
	tmp.out_mac = tc::cli::FormatUtil::hexStringToBytes("e37b6a775dc87dbaa4dfa9f96e5e3ffddebd71f8867289865df5a32d20cdc944b6022cac3c4982b10d5eeb55c3e4de15134676fb6de0446065c97440fa8c6a58");
	test_cases.push_back(tmp);
}