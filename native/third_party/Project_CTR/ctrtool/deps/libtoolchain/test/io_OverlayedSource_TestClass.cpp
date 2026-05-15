#include <iostream>
#include <sstream>

#include "io_OverlayedSource_TestClass.h"
#include "SourceTestUtil.h"

#include <tc.h>

void io_OverlayedSource_TestClass::runAllTests(void)
{
	std::cout << "[tc::io::OverlayedSource] START" << std::endl;
	testDefaultConstructor();
	testSingleOverlayConstructor();
	testMultiOverlayConstructor();
	testNullBaseStream();
	testNullOverlayStream();
	testOverlayStreamTooSmallForOverlayRegion();
	testOverlayRegionBeforeBaseStream();
	testOverlayRegionPartlyBeforeBaseStream();
	testOverlayRegionAfterBaseStream();
	testOverlayRegionPartlyAfterBaseStream();
	std::cout << "[tc::io::OverlayedSource] END" << std::endl;
}

void io_OverlayedSource_TestClass::testDefaultConstructor()
{
	std::cout << "[tc::io::OverlayedSource] testDefaultConstructor : " << std::flush;
	try
	{
		try
		{
			tc::io::OverlayedSource source;

			SourceTestUtil::testSourceLength(source, 0);
			SourceTestUtil::pullTestHelper(source, 0, 0xdead, 0, nullptr);

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

void io_OverlayedSource_TestClass::testSingleOverlayConstructor()
{
	std::cout << "[tc::io::OverlayedSource] testSingleOverlayConstructor : " << std::flush;
	try
	{
		try
		{
			// ## create overlay source
			size_t overlay_offset = 0x80;
			tc::io::PaddingSource base_source = tc::io::PaddingSource(0xff, 0x1000);
			tc::io::PaddingSource overlay_source = tc::io::PaddingSource(0xaa, 0x100);

			tc::io::OverlayedSource source(std::make_shared<tc::io::PaddingSource>(base_source), std::make_shared<tc::io::PaddingSource>(overlay_source), overlay_offset, overlay_source.length());

			// ## create ByteData with expected data to test against
			tc::ByteData expected_data = base_source.pullData(0, base_source.length());
			tc::ByteData overlay_data = overlay_source.pullData(0, overlay_source.length());
			
			memcpy(expected_data.data() + overlay_offset, overlay_data.data(), overlay_data.size());

			// ## validate overlay source
			// source length
			SourceTestUtil::testSourceLength(source, base_source.length());

			// pullData tests
			size_t pull_size;
			int64_t pull_offset;

			// pull full contents of source
			pull_size = base_source.length();
			pull_offset = 0;
			SourceTestUtil::pullTestHelper(source, pull_offset, pull_size, pull_size, expected_data.data() + pull_offset);

			// try to pull double the length of source
			pull_size = base_source.length() * 2;
			pull_offset = 0;
			SourceTestUtil::pullTestHelper(source, pull_offset, pull_size, pull_size/2, expected_data.data() + pull_offset);
			
			// pull source up to overlay source
			pull_size = overlay_offset;
			pull_offset = 0;
			SourceTestUtil::pullTestHelper(source, pull_offset, pull_size, pull_size, expected_data.data() + pull_offset);

			// pull just overlay source
			pull_size = overlay_source.length();
			pull_offset = overlay_offset;
			SourceTestUtil::pullTestHelper(source, pull_offset, pull_size, pull_size, expected_data.data() + pull_offset);

			// pull part of overlay
			pull_size = 0x20;
			pull_offset = overlay_offset - 0x30;
			SourceTestUtil::pullTestHelper(source, pull_offset, pull_size, pull_size, expected_data.data() + pull_offset);

			// pull part of base and part of overlay
			pull_size = 0x20;
			pull_offset = overlay_offset - 0x10;
			SourceTestUtil::pullTestHelper(source, pull_offset, pull_size, pull_size, expected_data.data() + pull_offset);

			// pull part of overlay and part of base
			pull_size = 0x20;
			pull_offset = overlay_offset + overlay_source.length() - 0x10;
			SourceTestUtil::pullTestHelper(source, pull_offset, pull_size, pull_size, expected_data.data() + pull_offset);

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

void io_OverlayedSource_TestClass::testMultiOverlayConstructor()
{
	std::cout << "[tc::io::OverlayedSource] testMultiOverlayConstructor : " << std::flush;
	try
	{
		try
		{
			
			// ## create overlay source
			tc::io::PaddingSource base_source = tc::io::PaddingSource(0xff, 0x1000);

			std::vector<tc::io::OverlayedSource::OverlaySourceInfo> overlay_info = {
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xaa, 0x1000)), 0x20, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xbb, 0x1000)), 0x100, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xcc, 0x1000)), 0x320, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xdd, 0x1000)), 0x420, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xee, 0x1000)), 0x520, 0x100 },
			};

			tc::io::OverlayedSource source(std::make_shared<tc::io::PaddingSource>(base_source), overlay_info);

			// ## create ByteData with expected data to test against
			tc::ByteData expected_data = base_source.pullData(0, base_source.length());
			for (auto itr = overlay_info.begin(); itr != overlay_info.end(); itr++)
			{
				tc::ByteData tmp = itr->overlay_source->pullData(0, itr->length);
				memcpy(expected_data.data() + itr->offset, tmp.data(), tmp.size());
			}
					
			// ## validate overlay source
			// source length
			SourceTestUtil::testSourceLength(source, base_source.length());

			// pullData tests
			size_t pull_size;
			int64_t pull_offset;

			// pull full contents of source
			pull_size = base_source.length();
			pull_offset = 0;
			SourceTestUtil::pullTestHelper(source, pull_offset, pull_size, pull_size, expected_data.data() + pull_offset);

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

void io_OverlayedSource_TestClass::testNullBaseStream()
{
	std::cout << "[tc::io::OverlayedSource] testNullBaseStream : " << std::flush;
	try
	{
		try
		{
			tc::io::PaddingSource base_source = tc::io::PaddingSource(0xff, 0x1000);

			std::vector<tc::io::OverlayedSource::OverlaySourceInfo> overlay_info = {
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xaa, 0x1000)), 0x20, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xbb, 0x1000)), 0x100, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xcc, 0x1000)), 0x320, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xdd, 0x1000)), 0x420, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xee, 0x1000)), 0x520, 0x100 },
			};

			tc::io::OverlayedSource source(std::shared_ptr<tc::io::PaddingSource>(nullptr), overlay_info);


			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_OverlayedSource_TestClass::testNullOverlayStream()
{
	std::cout << "[tc::io::OverlayedSource] testNullOverlayStream : " << std::flush;
	try
	{
		try
		{
			tc::io::PaddingSource base_source = tc::io::PaddingSource(0xff, 0x1000);

			std::vector<tc::io::OverlayedSource::OverlaySourceInfo> overlay_info = {
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xaa, 0x1000)), 0x20, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xbb, 0x1000)), 0x100, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(nullptr), 0x320, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xdd, 0x1000)), 0x420, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xee, 0x1000)), 0x520, 0x100 },
			};

			tc::io::OverlayedSource source(std::make_shared<tc::io::PaddingSource>(base_source), overlay_info);


			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_OverlayedSource_TestClass::testOverlayStreamTooSmallForOverlayRegion()
{
	std::cout << "[tc::io::OverlayedSource] testOverlayStreamTooSmallForOverlayRegion : " << std::flush;
	try
	{
		try
		{
			tc::io::PaddingSource base_source = tc::io::PaddingSource(0xff, 0x1000);

			std::vector<tc::io::OverlayedSource::OverlaySourceInfo> overlay_info = {
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xaa, 0x1000)), 0x20, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xbb, 0x1000)), 0x100, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xcc, 0x10)), 0x300, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xdd, 0x1000)), 0x420, 0x100 },
				{ std::shared_ptr<tc::io::PaddingSource>(new tc::io::PaddingSource(0xee, 0x1000)), 0x520, 0x100 },
			};

			tc::io::OverlayedSource source(std::make_shared<tc::io::PaddingSource>(base_source), overlay_info);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_OverlayedSource_TestClass::testOverlayRegionBeforeBaseStream()
{
	std::cout << "[tc::io::OverlayedSource] testOverlayRegionBeforeBaseStream : " << std::flush;
	try
	{
		try
		{
			tc::io::PaddingSource base_source = tc::io::PaddingSource(0xff, 0x1000);
			tc::io::PaddingSource padding_source = tc::io::PaddingSource(0xee, 0x1000);

			tc::io::OverlayedSource source(std::make_shared<tc::io::PaddingSource>(base_source), std::make_shared<tc::io::PaddingSource>(padding_source), -1, 1);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_OverlayedSource_TestClass::testOverlayRegionPartlyBeforeBaseStream()
{
	std::cout << "[tc::io::OverlayedSource] testOverlayRegionPartlyBeforeBaseStream : " << std::flush;
	try
	{
		try
		{
			tc::io::PaddingSource base_source = tc::io::PaddingSource(0xff, 0x1000);
			tc::io::PaddingSource padding_source = tc::io::PaddingSource(0xee, 0x1000);

			tc::io::OverlayedSource source(std::make_shared<tc::io::PaddingSource>(base_source), std::make_shared<tc::io::PaddingSource>(padding_source), -1, 20);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_OverlayedSource_TestClass::testOverlayRegionAfterBaseStream()
{
	std::cout << "[tc::io::OverlayedSource] testOverlayRegionAfterBaseStream : " << std::flush;
	try
	{
		try
		{
			tc::io::PaddingSource base_source = tc::io::PaddingSource(0xff, 0x1000);
			tc::io::PaddingSource padding_source = tc::io::PaddingSource(0xee, 0x1000);

			tc::io::OverlayedSource source(std::make_shared<tc::io::PaddingSource>(base_source), std::make_shared<tc::io::PaddingSource>(padding_source), 0x1000, 0x100);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_OverlayedSource_TestClass::testOverlayRegionPartlyAfterBaseStream()
{
	std::cout << "[tc::io::OverlayedSource] testOverlayRegionPartlyAfterBaseStream : " << std::flush;
	try
	{
		try
		{
			tc::io::PaddingSource base_source = tc::io::PaddingSource(0xff, 0x1000);
			tc::io::PaddingSource padding_source = tc::io::PaddingSource(0xee, 0x1000);

			tc::io::OverlayedSource source(std::make_shared<tc::io::PaddingSource>(base_source), std::make_shared<tc::io::PaddingSource>(padding_source), 0xfff, 0x100);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}