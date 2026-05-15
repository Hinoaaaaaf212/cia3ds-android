#include <iostream>
#include <sstream>
#include <fstream>

#include "crypto_PseudoRandomByteGenerator_TestClass.h"

#include <tc/crypto/PseudoRandomByteGenerator.h>
#include <tc/io/PaddingSource.h>
#include <tc/cli/FormatUtil.h>

void crypto_PseudoRandomByteGenerator_TestClass::runAllTests(void)
{
	std::cout << "[tc::crypto::PseudoRandomByteGenerator] START" << std::endl;
	test_Class();
	test_UtilFunc();
	test_MultipleObjectsCreateDifferentData();
	test_RepeatedCallsCreateDifferentData();
	std::cout << "[tc::crypto::PseudoRandomByteGenerator] END" << std::endl;
}

void crypto_PseudoRandomByteGenerator_TestClass::test_Class()
{
	std::cout << "[tc::crypto::PseudoRandomByteGenerator] test_Class : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create buffer to hold random data
			auto random_data = tc::ByteData(0x20);

			// generate control_data to compare to random_data to ensure that it is populated with random bytes
			auto control_data = tc::io::PaddingSource(0xbe, random_data.size()).pullData(0, random_data.size());

			// copy control data
			memcpy(random_data.data(), control_data.data(), random_data.size());

			// generate random bytes
			tc::crypto::PseudoRandomByteGenerator prbg;
			prbg.getBytes(random_data.data(), random_data.size());

			// compare with control data to see if the data changed
			if (memcmp(random_data.data(), control_data.data(), random_data.size()) == 0)
			{
				throw tc::Exception(".getBytes() did not populate array");
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

void crypto_PseudoRandomByteGenerator_TestClass::test_UtilFunc()
{
	std::cout << "[tc::crypto::PseudoRandomByteGenerator] test_UtilFunc : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// create buffer to hold random data
			auto random_data = tc::ByteData(0x20);

			// generate control_data to compare to random_data to ensure that it is populated with random bytes
			auto control_data = tc::io::PaddingSource(0xbe, random_data.size()).pullData(0, random_data.size());

			// copy control data
			memcpy(random_data.data(), control_data.data(), random_data.size());

			// generate random bytes
			tc::crypto::GeneratePseudoRandomBytes(random_data.data(), random_data.size());

			// compare with control data to see if the data changed
			if (memcmp(random_data.data(), control_data.data(), random_data.size()) == 0)
			{
				throw tc::Exception("GeneratePseudoRandomBytes() did not populate array");
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

void crypto_PseudoRandomByteGenerator_TestClass::test_MultipleObjectsCreateDifferentData()
{
	std::cout << "[tc::crypto::PseudoRandomByteGenerator] test_MultipleObjectsCreateDifferentData : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// generate allocate storage for each test
			static const size_t kRandomDataSize = 0x20;
			auto random_data1 = tc::ByteData(kRandomDataSize);
			auto random_data2 = tc::ByteData(kRandomDataSize);
			auto random_data3 = tc::ByteData(kRandomDataSize);
			
			// create PRBG objects
			tc::crypto::PseudoRandomByteGenerator prbg1, prbg2, prbg3;
			
			// generate random data
			prbg1.getBytes(random_data1.data(), random_data1.size());
			prbg2.getBytes(random_data2.data(), random_data2.size());
			prbg3.getBytes(random_data3.data(), random_data3.size());

			size_t cmp12 = 0, cmp13 = 0, cmp23 = 0;

			for (size_t i = 0; i < kRandomDataSize; i++)
			{
				cmp12 += random_data1[i] == random_data2[i];
				cmp13 += random_data1[i] == random_data3[i];
				cmp23 += random_data2[i] == random_data3[i];
			}

			// check to see if any of the tests were similar
			static const size_t kSimilarityThreshold = 2;
			if (cmp12 > kSimilarityThreshold)
			{
				ss << "case 1 & case 2 has " << std::dec << cmp12 << " similar bytes" ;
				throw tc::Exception(ss.str());
			}
			if (cmp13 > kSimilarityThreshold)
			{
				ss << "case 1 & case 3 has " << std::dec << cmp13 << " similar bytes" ;
				throw tc::Exception(ss.str());
			}
			if (cmp23 > kSimilarityThreshold)
			{
				ss << "case 2 & case 3 has " << std::dec << cmp23 << " similar bytes" ;
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

void crypto_PseudoRandomByteGenerator_TestClass::test_RepeatedCallsCreateDifferentData()
{
	std::cout << "[tc::crypto::PseudoRandomByteGenerator] test_RepeatedCallsCreateDifferentData : " << std::flush;
	try
	{
		try 
		{
			std::stringstream ss;

			// generate allocate storage for each test
			static const size_t kRandomDataSize = 0x20;
			auto random_data1 = tc::ByteData(kRandomDataSize);
			auto random_data2 = tc::ByteData(kRandomDataSize);
			auto random_data3 = tc::ByteData(kRandomDataSize);
			
			// create PRBG object
			tc::crypto::PseudoRandomByteGenerator prbg;
			
			// generate random data
			prbg.getBytes(random_data1.data(), random_data1.size());
			prbg.getBytes(random_data2.data(), random_data2.size());
			prbg.getBytes(random_data3.data(), random_data3.size());

			size_t cmp12 = 0, cmp13 = 0, cmp23 = 0;

			for (size_t i = 0; i < kRandomDataSize; i++)
			{
				cmp12 += random_data1[i] == random_data2[i];
				cmp13 += random_data1[i] == random_data3[i];
				cmp23 += random_data2[i] == random_data3[i];
			}

			// check to see if any of the tests were similar
			static const size_t kSimilarityThreshold = 2;
			if (cmp12 > kSimilarityThreshold)
			{
				ss << "case 1 & case 2 has " << std::dec << cmp12 << " similar bytes" ;
				throw tc::Exception(ss.str());
			}
			if (cmp13 > kSimilarityThreshold)
			{
				ss << "case 1 & case 3 has " << std::dec << cmp13 << " similar bytes" ;
				throw tc::Exception(ss.str());
			}
			if (cmp23 > kSimilarityThreshold)
			{
				ss << "case 2 & case 3 has " << std::dec << cmp23 << " similar bytes" ;
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