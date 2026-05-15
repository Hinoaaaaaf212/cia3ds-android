#pragma once
#include "ITestClass.h"
#include <tc/types.h>

class bn_bitarrayByteLEBitLE_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_Size();
	void test_TestBit();
	void test_SetBit();
	void test_ResetBit();
	void test_FlipBit();

	using testtype_t = tc::bn::bitarray<sizeof(uint32_t), true, true>;

	void helper_TestBit(const std::string& test_name, const testtype_t& bitarray, const std::vector<size_t>& expected_set_bits);
};