#include <tc.h>
#include <fmt/core.h>

#include "bn_endian_TestClass.h"

void bn_endian_TestClass::runAllTests()
{
	fmt::print("[tc::bn endian_types] START\n");
	testLocalBSwap16();
	testLocalBSwap32();
	testLocalBSwap64();
	testBeUint64Inline();
	testBeUint32Inline();
	testBeUint16Inline();
	testLeUint64Inline();
	testLeUint32Inline();
	testLeUint16Inline();
	testBeSwap64Inline();
	testBeSwap32Inline();
	testBeSwap16Inline();
	testLeSwap64Inline();
	testLeSwap32Inline();
	testLeSwap16Inline();
	testBe64TemplateClass();
	testBe32TemplateClass();
	testBe16TemplateClass();
	testLe64TemplateClass();
	testLe32TemplateClass();
	testLe16TemplateClass();
	fmt::print("[tc::bn endian_types] END\n");
}

void bn_endian_TestClass::testLocalBSwap16()
{
	fmt::print("[tc::bn::detail::__local_bswap16] testLocalBSwap16 : ");
	try 
	{
		uint16_t x = 0xabcd;
		uint16_t x_inv = 0xcdab;

		uint16_t swap_ret = tc::bn::detail::__local_bswap16(x);
		if (swap_ret != x_inv)
		{
			throw tc::Exception(fmt::format("tc::bn::detail::__local_bswap16(uint16_t) returned 0x{:x} (expected: 0x{:x}", swap_ret, x_inv));
		}

		uint16_t x_test = x;
		tc::bn::detail::__local_bswap16(&x_test);
		if (x_test != x_inv)
		{
			throw tc::Exception(fmt::format("tc::bn::detail::__local_bswap16(void*) transformed 0x{:x} -> 0x{:x} (expected: 0x{:x}", x, x_test, x_inv));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testLocalBSwap32()
{
	fmt::print("[tc::bn::detail::__local_bswap32] testLocalBSwap32 : ");
	try 
	{
		uint32_t x = 0xabcd1234;
		uint32_t x_inv = 0x3412cdab;

		uint32_t swap_ret = tc::bn::detail::__local_bswap32(x);
		if (swap_ret != x_inv)
		{
			throw tc::Exception(fmt::format("tc::bn::detail::__local_bswap32(uint32_t) returned 0x{:x} (expected: 0x{:x}", swap_ret, x_inv));
		}

		uint32_t x_test = x;
		tc::bn::detail::__local_bswap32(&x_test);
		if (x_test != x_inv)
		{
			throw tc::Exception(fmt::format("tc::bn::detail::__local_bswap32(void*) transformed 0x{:x} -> 0x{:x} (expected: 0x{:x}", x, x_test, x_inv));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testLocalBSwap64()
{
	fmt::print("[tc::bn::detail::__local_bswap64] testLocalBSwap64 : ");
	try 
	{
		uint64_t x = 0x0123456789abcdef;
		uint64_t x_inv = 0xefcdab8967452301;

		uint64_t swap_ret = tc::bn::detail::__local_bswap64(x);
		if (swap_ret != x_inv)
		{
			throw tc::Exception(fmt::format("tc::bn::detail::__local_bswap64(uint64_t) returned 0x{:x} (expected: 0x{:x}", swap_ret, x_inv));
		}

		uint64_t x_test = x;
		tc::bn::detail::__local_bswap64(&x_test);
		if (x_test != x_inv)
		{
			throw tc::Exception(fmt::format("tc::bn::detail::__local_bswap64(void*) transformed 0x{:x} -> 0x{:x} (expected: 0x{:x}", x, x_test, x_inv));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testBeUint64Inline()
{
	fmt::print("[tc::bn::detail::__be_uint64] testBeUint64Inline : ");
	try 
	{
		uint8_t x_raw[8] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
		uint64_t* x_raw_ptr = (uint64_t*)&x_raw;
		uint64_t x_expected = 0x0123456789abcdef;

		uint64_t x_ret = tc::bn::detail::__be_uint64(*x_raw_ptr);
		if (x_ret != x_expected)
		{
			throw tc::Exception(fmt::format("transformed 0x{:x} -> 0x{:x} (expected 0x{:x})", *x_raw_ptr, x_ret, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testBeUint32Inline()
{
	fmt::print("[tc::bn::detail::__be_uint32] testBeUint32Inline : ");
	try 
	{
		uint8_t x_raw[4] = { 0x01, 0x23, 0x45, 0x67 };
		uint32_t* x_raw_ptr = (uint32_t*)&x_raw;
		uint32_t x_expected = 0x01234567;

		uint32_t x_ret = tc::bn::detail::__be_uint32(*x_raw_ptr);
		if (x_ret != x_expected)
		{
			throw tc::Exception(fmt::format("transformed 0x{:x} -> 0x{:x} (expected 0x{:x})", *x_raw_ptr, x_ret, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testBeUint16Inline()
{
	fmt::print("[tc::bn::detail::__be_uint16] testBeUint16Inline : ");
	try 
	{
		uint8_t x_raw[2] = { 0x01, 0x23 };
		uint16_t* x_raw_ptr = (uint16_t*)&x_raw;
		uint16_t x_expected = 0x0123;

		uint16_t x_ret = tc::bn::detail::__be_uint16(*x_raw_ptr);
		if (x_ret != x_expected)
		{
			throw tc::Exception(fmt::format("transformed 0x{:x} -> 0x{:x} (expected 0x{:x})", *x_raw_ptr, x_ret, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testLeUint64Inline()
{
	fmt::print("[tc::bn::detail::__le_uint64] testLeUint64Inline : ");
	try 
	{
		uint8_t x_raw[8] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
		uint64_t* x_raw_ptr = (uint64_t*)&x_raw;
		uint64_t x_expected = 0xefcdab8967452301;

		uint64_t x_ret = tc::bn::detail::__le_uint64(*x_raw_ptr);
		if (x_ret != x_expected)
		{
			throw tc::Exception(fmt::format("transformed 0x{:x} -> 0x{:x} (expected 0x{:x})", *x_raw_ptr, x_ret, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testLeUint32Inline()
{
	fmt::print("[tc::bn::detail::__le_uint32] testLeUint32Inline : ");
	try 
	{
		uint8_t x_raw[4] = { 0x01, 0x23, 0x45, 0x67 };
		uint32_t* x_raw_ptr = (uint32_t*)&x_raw;
		uint32_t x_expected = 0x67452301;

		uint32_t x_ret = tc::bn::detail::__le_uint32(*x_raw_ptr);
		if (x_ret != x_expected)
		{
			throw tc::Exception(fmt::format("transformed 0x{:x} -> 0x{:x} (expected 0x{:x})", *x_raw_ptr, x_ret, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testLeUint16Inline()
{
	fmt::print("[tc::bn::detail::__le_uint16] testLeUint16Inline : ");
	try 
	{
		uint8_t x_raw[2] = { 0x01, 0x23 };
		uint16_t* x_raw_ptr = (uint16_t*)&x_raw;
		uint16_t x_expected = 0x2301;

		uint16_t x_ret = tc::bn::detail::__le_uint16(*x_raw_ptr);
		if (x_ret != x_expected)
		{
			throw tc::Exception(fmt::format("transformed 0x{:x} -> 0x{:x} (expected 0x{:x})", *x_raw_ptr, x_ret, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testBeSwap64Inline()
{
	fmt::print("[tc::bn::detail::__be_swap64] testBeSwap64Inline : ");
	try 
	{
		uint8_t x_raw[8] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
		uint64_t* x_raw_ptr = (uint64_t*)&x_raw;
		uint64_t x_expected = 0x0123456789abcdef;

		uint64_t x_before = *x_raw_ptr;
		tc::bn::detail::__be_swap64(x_raw);
		uint64_t x_after = *x_raw_ptr;
		if (x_after != x_expected)
		{
			throw tc::Exception(fmt::format("transformed 0x{:x} -> 0x{:x} (expected 0x{:x})", x_before, x_after, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testBeSwap32Inline()
{
	fmt::print("[tc::bn::detail::__be_swap32] testBeSwap32Inline : ");
	try 
	{
		uint8_t x_raw[4] = { 0x01, 0x23, 0x45, 0x67 };
		uint32_t* x_raw_ptr = (uint32_t*)&x_raw;
		uint32_t x_expected = 0x01234567;

		uint32_t x_before = *x_raw_ptr;
		tc::bn::detail::__be_swap32(x_raw);
		uint32_t x_after = *x_raw_ptr;
		if (x_after != x_expected)
		{
			throw tc::Exception(fmt::format("transformed 0x{:x} -> 0x{:x} (expected 0x{:x})", x_before, x_after, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testBeSwap16Inline()
{
	fmt::print("[tc::bn::detail::__be_swap16] testBeSwap16Inline : ");
	try 
	{
		uint8_t x_raw[2] = { 0x01, 0x23 };
		uint16_t* x_raw_ptr = (uint16_t*)&x_raw;
		uint16_t x_expected = 0x0123;

		uint16_t x_before = *x_raw_ptr;
		tc::bn::detail::__be_swap16(x_raw);
		uint16_t x_after = *x_raw_ptr;
		if (x_after != x_expected)
		{
			throw tc::Exception(fmt::format("transformed 0x{:x} -> 0x{:x} (expected 0x{:x})", x_before, x_after, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testLeSwap64Inline()
{
	fmt::print("[tc::bn::detail::__le_swap64] testLeSwap64Inline : ");
	try 
	{
		uint8_t x_raw[8] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
		uint64_t* x_raw_ptr = (uint64_t*)&x_raw;
		uint64_t x_expected = 0xefcdab8967452301;

		uint64_t x_before = *x_raw_ptr;
		tc::bn::detail::__le_swap64(x_raw);
		uint64_t x_after = *x_raw_ptr;
		if (x_after != x_expected)
		{
			throw tc::Exception(fmt::format("transformed 0x{:x} -> 0x{:x} (expected 0x{:x})", x_before, x_after, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testLeSwap32Inline()
{
	fmt::print("[tc::bn::detail::__le_swap32] testLeSwap32Inline : ");
	try 
	{
		uint8_t x_raw[4] = { 0x01, 0x23, 0x45, 0x67 };
		uint32_t* x_raw_ptr = (uint32_t*)&x_raw;
		uint32_t x_expected = 0x67452301;

		uint32_t x_before = *x_raw_ptr;
		tc::bn::detail::__le_swap32(x_raw);
		uint32_t x_after = *x_raw_ptr;
		if (x_after != x_expected)
		{
			throw tc::Exception(fmt::format("transformed 0x{:x} -> 0x{:x} (expected 0x{:x})", x_before, x_after, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testLeSwap16Inline()
{
	fmt::print("[tc::bn::detail::__le_swap16] testLeSwap16Inline : ");
	try 
	{
		uint8_t x_raw[2] = { 0x01, 0x23 };
		uint16_t* x_raw_ptr = (uint16_t*)&x_raw;
		uint16_t x_expected = 0x2301;

		uint16_t x_before = *x_raw_ptr;
		tc::bn::detail::__le_swap16(x_raw);
		uint16_t x_after = *x_raw_ptr;
		if (x_after != x_expected)
		{
			throw tc::Exception(fmt::format("transformed 0x{:x} -> 0x{:x} (expected 0x{:x})", x_before, x_after, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testBe64TemplateClass()
{
	fmt::print("[tc::bn::be64<uint64_t>] testBe64TemplateClass : ");
	try 
	{
		uint8_t x_raw[sizeof(uint64_t)] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
		uint8_t x_raw_expected[sizeof(uint64_t)] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
		tc::bn::be64<uint64_t>* x_raw_ptr = (tc::bn::be64<uint64_t>*)&x_raw;
		uint64_t x_expected = 0x0123456789abcdef;
		uint64_t unwrap_ret;

		// explicit unwrap/wrap
		unwrap_ret = x_raw_ptr->unwrap();
		if (unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::be64<uint64_t>::unwrap() returned 0x{:x} (expected 0x{:x}", unwrap_ret, x_expected));
		}

		x_raw_ptr->wrap(0);
		unwrap_ret = x_raw_ptr->unwrap();
		if (unwrap_ret != 0)
		{
			throw tc::Exception(fmt::format("tc::bn::be64<uint64_t>::wrap() failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", 0, unwrap_ret, 0));
		}

		x_raw_ptr->wrap(x_expected);
		unwrap_ret = x_raw_ptr->unwrap();
		if (memcmp(x_raw, x_raw_expected, sizeof(uint64_t)) != 0 || unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::be64<uint64_t>::wrap() failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", x_expected, unwrap_ret, x_expected));
		}

		// implicit unwrap/wrap
		x_raw_ptr = (tc::bn::be64<uint64_t>*)&x_raw;
		unwrap_ret = (*x_raw_ptr);
		if (unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::be64<uint64_t> implicit unwrap returned 0x{:x} (expected 0x{:x})", unwrap_ret, x_expected));
		}

		(*x_raw_ptr) = 0;
		unwrap_ret = (*x_raw_ptr);
		if (unwrap_ret != 0)
		{
			throw tc::Exception(fmt::format("tc::bn::be64<uint64_t> implicit wrap failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", 0x0, unwrap_ret, 0x0));
		}

		(*x_raw_ptr) = x_expected;
		unwrap_ret = (*x_raw_ptr);
		if (memcmp(x_raw, x_raw_expected, sizeof(uint64_t)) != 0 || unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::be64<uint64_t> implicit wrap failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", x_expected, unwrap_ret, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testBe32TemplateClass()
{
	fmt::print("[tc::bn::be32<uint32_t>] testBe32TemplateClass : ");
	try 
	{
		uint8_t x_raw[sizeof(uint32_t)] = { 0x01, 0x23, 0x45, 0x67 };
		uint8_t x_raw_expected[sizeof(uint32_t)] = { 0x01, 0x23, 0x45, 0x67 };
		tc::bn::be32<uint32_t>* x_raw_ptr = (tc::bn::be32<uint32_t>*)&x_raw;
		uint32_t x_expected = 0x01234567;
		uint32_t unwrap_ret;

		// explicit unwrap/wrap
		unwrap_ret = x_raw_ptr->unwrap();
		if (unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::be32<uint32_t>::unwrap() returned 0x{:x} (expected 0x{:x}", unwrap_ret, x_expected));
		}

		x_raw_ptr->wrap(0);
		unwrap_ret = x_raw_ptr->unwrap();
		if (unwrap_ret != 0)
		{
			throw tc::Exception(fmt::format("tc::bn::be32<uint32_t>::wrap() failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", 0, unwrap_ret, 0));
		}

		x_raw_ptr->wrap(x_expected);
		unwrap_ret = x_raw_ptr->unwrap();
		if (memcmp(x_raw, x_raw_expected, sizeof(uint32_t)) != 0 || unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::be32<uint32_t>::wrap() failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", x_expected, unwrap_ret, x_expected));
		}

		// implicit unwrap/wrap
		x_raw_ptr = (tc::bn::be32<uint32_t>*)&x_raw;
		unwrap_ret = (*x_raw_ptr);
		if (unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::be32<uint32_t> implicit unwrap returned 0x{:x} (expected 0x{:x})", unwrap_ret, x_expected));
		}

		(*x_raw_ptr) = 0;
		unwrap_ret = (*x_raw_ptr);
		if (unwrap_ret != 0)
		{
			throw tc::Exception(fmt::format("tc::bn::be32<uint32_t> implicit wrap failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", 0x0, unwrap_ret, 0x0));
		}

		(*x_raw_ptr) = x_expected;
		unwrap_ret = (*x_raw_ptr);
		if (memcmp(x_raw, x_raw_expected, sizeof(uint32_t)) != 0 || unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::be32<uint32_t> implicit wrap failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", x_expected, unwrap_ret, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testBe16TemplateClass()
{
	fmt::print("[tc::bn::be16<uint16_t>] testBe16TemplateClass : ");
	try 
	{
		uint8_t x_raw[sizeof(uint16_t)] = { 0x01, 0x23 };
		uint8_t x_raw_expected[sizeof(uint16_t)] = { 0x01, 0x23 };
		tc::bn::be16<uint16_t>* x_raw_ptr = (tc::bn::be16<uint16_t>*)&x_raw;
		uint16_t x_expected = 0x0123;
		uint16_t unwrap_ret;

		// explicit unwrap/wrap
		unwrap_ret = x_raw_ptr->unwrap();
		if (unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::be16<uint16_t>::unwrap() returned 0x{:x} (expected 0x{:x}", unwrap_ret, x_expected));
		}

		x_raw_ptr->wrap(0);
		unwrap_ret = x_raw_ptr->unwrap();
		if (unwrap_ret != 0)
		{
			throw tc::Exception(fmt::format("tc::bn::be16<uint16_t>::wrap() failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", 0, unwrap_ret, 0));
		}

		x_raw_ptr->wrap(x_expected);
		unwrap_ret = x_raw_ptr->unwrap();
		if (memcmp(x_raw, x_raw_expected, sizeof(uint16_t)) != 0 || unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::be16<uint16_t>::wrap() failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", x_expected, unwrap_ret, x_expected));
		}

		// implicit unwrap/wrap
		x_raw_ptr = (tc::bn::be16<uint16_t>*)&x_raw;
		unwrap_ret = (*x_raw_ptr);
		if (unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::be16<uint16_t> implicit unwrap returned 0x{:x} (expected 0x{:x})", unwrap_ret, x_expected));
		}

		(*x_raw_ptr) = 0;
		unwrap_ret = (*x_raw_ptr);
		if (unwrap_ret != 0)
		{
			throw tc::Exception(fmt::format("tc::bn::be16<uint16_t> implicit wrap failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", 0x0, unwrap_ret, 0x0));
		}

		(*x_raw_ptr) = x_expected;
		unwrap_ret = (*x_raw_ptr);
		if (memcmp(x_raw, x_raw_expected, sizeof(uint16_t)) != 0 || unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::be16<uint16_t> implicit wrap failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", x_expected, unwrap_ret, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testLe64TemplateClass()
{
	fmt::print("[tc::bn::le64<uint64_t>] testLe64TemplateClass : ");
	try 
	{
		uint8_t x_raw[sizeof(uint64_t)] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
		uint8_t x_raw_expected[sizeof(uint64_t)] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
		tc::bn::le64<uint64_t>* x_raw_ptr = (tc::bn::le64<uint64_t>*)&x_raw;
		uint64_t x_expected = 0xefcdab8967452301;
		uint64_t unwrap_ret;

		// explicit unwrap/wrap
		unwrap_ret = x_raw_ptr->unwrap();
		if (unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::le64<uint64_t>::unwrap() returned 0x{:x} (expected 0x{:x}", unwrap_ret, x_expected));
		}

		x_raw_ptr->wrap(0);
		unwrap_ret = x_raw_ptr->unwrap();
		if (unwrap_ret != 0)
		{
			throw tc::Exception(fmt::format("tc::bn::le64<uint64_t>::wrap() failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", 0, unwrap_ret, 0));
		}

		x_raw_ptr->wrap(x_expected);
		unwrap_ret = x_raw_ptr->unwrap();
		if (memcmp(x_raw, x_raw_expected, sizeof(uint64_t)) != 0 || unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::le64<uint64_t>::wrap() failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", x_expected, unwrap_ret, x_expected));
		}

		// implicit unwrap/wrap
		x_raw_ptr = (tc::bn::le64<uint64_t>*)&x_raw;
		unwrap_ret = (*x_raw_ptr);
		if (unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::le64<uint64_t> implicit unwrap returned 0x{:x} (expected 0x{:x})", unwrap_ret, x_expected));
		}

		(*x_raw_ptr) = 0;
		unwrap_ret = (*x_raw_ptr);
		if (unwrap_ret != 0)
		{
			throw tc::Exception(fmt::format("tc::bn::le64<uint64_t> implicit wrap failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", 0x0, unwrap_ret, 0x0));
		}

		(*x_raw_ptr) = x_expected;
		unwrap_ret = (*x_raw_ptr);
		if (memcmp(x_raw, x_raw_expected, sizeof(uint64_t)) != 0 || unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::le64<uint64_t> implicit wrap failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", x_expected, unwrap_ret, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testLe32TemplateClass()
{
	fmt::print("[tc::bn::le32<uint32_t>] testLe32TemplateClass : ");
	try 
	{
		uint8_t x_raw[sizeof(uint32_t)] = { 0x01, 0x23, 0x45, 0x67 };
		uint8_t x_raw_expected[sizeof(uint32_t)] = { 0x01, 0x23, 0x45, 0x67 };
		tc::bn::le32<uint32_t>* x_raw_ptr = (tc::bn::le32<uint32_t>*)&x_raw;
		uint32_t x_expected = 0x67452301;
		uint32_t unwrap_ret;

		// explicit unwrap/wrap
		unwrap_ret = x_raw_ptr->unwrap();
		if (unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::le32<uint32_t>::unwrap() returned 0x{:x} (expected 0x{:x}", unwrap_ret, x_expected));
		}

		x_raw_ptr->wrap(0);
		unwrap_ret = x_raw_ptr->unwrap();
		if (unwrap_ret != 0)
		{
			throw tc::Exception(fmt::format("tc::bn::le32<uint32_t>::wrap() failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", 0, unwrap_ret, 0));
		}

		x_raw_ptr->wrap(x_expected);
		unwrap_ret = x_raw_ptr->unwrap();
		if (memcmp(x_raw, x_raw_expected, sizeof(uint32_t)) != 0 || unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::le32<uint32_t>::wrap() failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", x_expected, unwrap_ret, x_expected));
		}

		// implicit unwrap/wrap
		x_raw_ptr = (tc::bn::le32<uint32_t>*)&x_raw;
		unwrap_ret = (*x_raw_ptr);
		if (unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::le32<uint32_t> implicit unwrap returned 0x{:x} (expected 0x{:x})", unwrap_ret, x_expected));
		}

		(*x_raw_ptr) = 0;
		unwrap_ret = (*x_raw_ptr);
		if (unwrap_ret != 0)
		{
			throw tc::Exception(fmt::format("tc::bn::le32<uint32_t> implicit wrap failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", 0x0, unwrap_ret, 0x0));
		}

		(*x_raw_ptr) = x_expected;
		unwrap_ret = (*x_raw_ptr);
		if (memcmp(x_raw, x_raw_expected, sizeof(uint32_t)) != 0 || unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::le32<uint32_t> implicit wrap failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", x_expected, unwrap_ret, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}

void bn_endian_TestClass::testLe16TemplateClass()
{
	fmt::print("[tc::bn::le16<uint16_t>] testLe16TemplateClass : ");
	try 
	{
		uint8_t x_raw[sizeof(uint16_t)] = { 0x01, 0x23 };
		uint8_t x_raw_expected[sizeof(uint16_t)] = { 0x01, 0x23 };
		tc::bn::le16<uint16_t>* x_raw_ptr = (tc::bn::le16<uint16_t>*)&x_raw;
		uint16_t x_expected = 0x2301;
		uint16_t unwrap_ret;

		// explicit unwrap/wrap
		unwrap_ret = x_raw_ptr->unwrap();
		if (unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::le16<uint16_t>::unwrap() returned 0x{:x} (expected 0x{:x}", unwrap_ret, x_expected));
		}

		x_raw_ptr->wrap(0);
		unwrap_ret = x_raw_ptr->unwrap();
		if (unwrap_ret != 0)
		{
			throw tc::Exception(fmt::format("tc::bn::le16<uint16_t>::wrap() failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", 0, unwrap_ret, 0));
		}

		x_raw_ptr->wrap(x_expected);
		unwrap_ret = x_raw_ptr->unwrap();
		if (memcmp(x_raw, x_raw_expected, sizeof(uint16_t)) != 0 || unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::le16<uint16_t>::wrap() failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", x_expected, unwrap_ret, x_expected));
		}

		// implicit unwrap/wrap
		x_raw_ptr = (tc::bn::le16<uint16_t>*)&x_raw;
		unwrap_ret = (*x_raw_ptr);
		if (unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::le16<uint16_t> implicit unwrap returned 0x{:x} (expected 0x{:x})", unwrap_ret, x_expected));
		}

		(*x_raw_ptr) = 0;
		unwrap_ret = (*x_raw_ptr);
		if (unwrap_ret != 0)
		{
			throw tc::Exception(fmt::format("tc::bn::le16<uint16_t> implicit wrap failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", 0x0, unwrap_ret, 0x0));
		}

		(*x_raw_ptr) = x_expected;
		unwrap_ret = (*x_raw_ptr);
		if (memcmp(x_raw, x_raw_expected, sizeof(uint16_t)) != 0 || unwrap_ret != x_expected)
		{
			throw tc::Exception(fmt::format("tc::bn::le16<uint16_t> implicit wrap failed to wrap 0x{:x} (unwrap returned 0x{:x}, expected 0x{:x})", x_expected, unwrap_ret, x_expected));
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({:s})\n", e.error());
	}
}