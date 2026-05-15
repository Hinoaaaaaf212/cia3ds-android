#include <iostream>
#include <sstream>
#include <fstream>

#include "io_FileStream_TestClass.h"
#include "StreamTestUtil.h"

#include <tc.h>

std::string io_FileStream_TestClass::kAsciiFilePath = "LocalFileTest.bin";
std::string io_FileStream_TestClass::kUtf8TestPath = "ЀЁЂЃЄЅ-מבחן-тест-テスト.bin";
std::string io_FileStream_TestClass::kNotExistFilePath = "ThisDoesNotExist.bin";
std::string io_FileStream_TestClass::kTestPhrase = "Hello world!\n";
std::string io_FileStream_TestClass::kRandomString = "uUkMx4MYhJdwUnr38Jk7nZvXQnW0IhGNQqjMRyKoRuxXwmxBS3p2Alzrv7BijPN2LDI1QGkEfQ3vrpoOGwKciwidTyuOPRRg9sj8QggPk7QSvJrrWKN3PfzN7JvEwax3vX3QaHIoX0afJtUiulzVf9SMlotimwrdOHbeAhLzQUSCAz6moIHhZd6DO0hFxjCxGpHUnDKE";

void io_FileStream_TestClass::runAllTests(void)
{
	std::cout << "[tc::io::FileStream] START" << std::endl;
	
	test_DefaultConstructor();

	test_Constructor_CreateNew_Read_FileExists();
	test_Constructor_CreateNew_Read_FileNotExist();
	test_Constructor_CreateNew_Write_FileExists();
	test_Constructor_CreateNew_Write_FileNotExist();
	test_Constructor_CreateNew_ReadWrite_FileExists();
	test_Constructor_CreateNew_ReadWrite_FileNotExist();

	test_Constructor_Create_Read_FileExists();
	test_Constructor_Create_Read_FileNotExist();
	test_Constructor_Create_Write_FileExists();
	test_Constructor_Create_Write_FileNotExist();
	test_Constructor_Create_ReadWrite_FileExists();
	test_Constructor_Create_ReadWrite_FileNotExist();

	test_Constructor_Open_Read_FileExists();
	test_Constructor_Open_Read_FileNotExist();
	test_Constructor_Open_Write_FileExists();
	test_Constructor_Open_Write_FileNotExist();
	test_Constructor_Open_ReadWrite_FileExists();
	test_Constructor_Open_ReadWrite_FileNotExist();

	test_Constructor_OpenOrCreate_Read_FileExists();
	test_Constructor_OpenOrCreate_Read_FileNotExist();
	test_Constructor_OpenOrCreate_Write_FileExists();
	test_Constructor_OpenOrCreate_Write_FileNotExist();
	test_Constructor_OpenOrCreate_ReadWrite_FileExists();
	test_Constructor_OpenOrCreate_ReadWrite_FileNotExist();

	test_Constructor_Truncate_Read_FileExists();
	test_Constructor_Truncate_Read_FileNotExist();
	test_Constructor_Truncate_Write_FileExists();
	test_Constructor_Truncate_Write_FileNotExist();
	test_Constructor_Truncate_ReadWrite_FileExists();
	test_Constructor_Truncate_ReadWrite_FileNotExist();

	test_Constructor_Append_Read_FileExists();
	test_Constructor_Append_Read_FileNotExist();
	test_Constructor_Append_Write_FileExists();
	test_Constructor_Append_Write_FileNotExist();
	test_Constructor_Append_ReadWrite_FileExists();
	test_Constructor_Append_ReadWrite_FileNotExist();

	test_Constructor_IllegalMode();
	test_Constructor_IllegalAccess();

	test_Constructor_DirectoryPath();
	test_Constructor_CreateThenReopenFileWithUnicodePath();

	test_Seek_EmptyFile();
	test_Seek_CreatedFile();
	test_Seek_AppendMode();
	test_Seek_PositionBeforeFileBegin();
	test_Seek_PositionAfterFileEnd();

	test_Read_NoData();
	test_Read_SomeDataFromZero();
	test_Read_SomeDataFromMiddle();
	test_Read_AllData();
	test_Read_TooMuchData();
	test_Read_BeyondEnd();
	test_Read_CanReadFalse();
	test_Read_NullDstPointer();

	test_Write_NoData();
	test_Write_OverwriteSomeDataFromZero();
	test_Write_OverwriteSomeDataFromMiddle();
	test_Write_ExtendStreamSizeThruWritingDataFromZero();
	test_Write_ExtendStreamSizeThruWritingDataFromMiddle();
	test_Write_BeyondEnd();
	test_Write_CanWriteFalse();
	test_Write_NullSrcPointer();

	std::cout << "[tc::io::FileStream] END" << std::endl;
}

void io_FileStream_TestClass::test_DefaultConstructor()
{
	std::cout << "[tc::io::FileStream] test_DefaultConstructor : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream();

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, false, false, false);

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

// Constructor_CreateNew

void io_FileStream_TestClass::test_Constructor_CreateNew_Read_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_CreateNew_Read_FileExists : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::CreateNew, tc::io::FileAccess::Read);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, false, true);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_CreateNew_Read_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_CreateNew_Read_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::CreateNew, tc::io::FileAccess::Read);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, false, true);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_CreateNew_Write_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_CreateNew_Write_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::CreateNew, tc::io::FileAccess::Write);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, false, true, true);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::io::FileExistsException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_CreateNew_Write_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_CreateNew_Write_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::CreateNew, tc::io::FileAccess::Write);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, false, true, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_CreateNew_ReadWrite_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_CreateNew_ReadWrite_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::CreateNew, tc::io::FileAccess::ReadWrite);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, true, true);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::io::FileExistsException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_CreateNew_ReadWrite_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_CreateNew_ReadWrite_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::CreateNew, tc::io::FileAccess::ReadWrite);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, true, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

// Constructor_Create

void io_FileStream_TestClass::test_Constructor_Create_Read_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Create_Read_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Create, tc::io::FileAccess::Read);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, false, true);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Create_Read_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Create_Read_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Create, tc::io::FileAccess::Read);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, false, true);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Create_Write_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Create_Write_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Create, tc::io::FileAccess::Write);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, false, true, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Create_Write_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Create_Write_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Create, tc::io::FileAccess::Write);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, false, true, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Create_ReadWrite_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Create_ReadWrite_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Create, tc::io::FileAccess::ReadWrite);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, true, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Create_ReadWrite_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Create_ReadWrite_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Create, tc::io::FileAccess::ReadWrite);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, true, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

// Constructor_Open

void io_FileStream_TestClass::test_Constructor_Open_Read_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Open_Read_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			StreamTestUtil::constructor_TestHelper(stream, kRandomString.size(), 0, true, false, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Open_Read_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Open_Read_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, false, true);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::io::FileNotFoundException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Open_Write_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Open_Write_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Write);

			StreamTestUtil::constructor_TestHelper(stream, kRandomString.size(), 0, false, true, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Open_Write_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Open_Write_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Write);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, false, true, true);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::io::FileNotFoundException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Open_ReadWrite_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Open_ReadWrite_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::ReadWrite);

			StreamTestUtil::constructor_TestHelper(stream, kRandomString.size(), 0, true, true, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Open_ReadWrite_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Open_ReadWrite_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::ReadWrite);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, true, true);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::io::FileNotFoundException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

// Constructor_OpenOrCreate

void io_FileStream_TestClass::test_Constructor_OpenOrCreate_Read_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_OpenOrCreate_Read_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::OpenOrCreate, tc::io::FileAccess::Read);

			StreamTestUtil::constructor_TestHelper(stream, kRandomString.size(), 0, true, false, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_OpenOrCreate_Read_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_OpenOrCreate_Read_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::OpenOrCreate, tc::io::FileAccess::Read);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, false, true);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::io::FileNotFoundException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_OpenOrCreate_Write_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_OpenOrCreate_Write_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::OpenOrCreate, tc::io::FileAccess::Write);

			StreamTestUtil::constructor_TestHelper(stream, kRandomString.size(), 0, false, true, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_OpenOrCreate_Write_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_OpenOrCreate_Write_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::OpenOrCreate, tc::io::FileAccess::Write);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, false, true, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_OpenOrCreate_ReadWrite_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_OpenOrCreate_ReadWrite_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::OpenOrCreate, tc::io::FileAccess::ReadWrite);

			StreamTestUtil::constructor_TestHelper(stream, kRandomString.size(), 0, true, true, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_OpenOrCreate_ReadWrite_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_OpenOrCreate_ReadWrite_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::OpenOrCreate, tc::io::FileAccess::ReadWrite);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, true, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

// Constructor_Truncate

void io_FileStream_TestClass::test_Constructor_Truncate_Read_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Truncate_Read_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Truncate, tc::io::FileAccess::Read);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, false, true);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Truncate_Read_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Truncate_Read_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Truncate, tc::io::FileAccess::Read);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, false, true);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Truncate_Write_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Truncate_Write_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Truncate, tc::io::FileAccess::Write);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, false, true, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Truncate_Write_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Truncate_Write_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Truncate, tc::io::FileAccess::Write);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, false, true, true);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::io::FileNotFoundException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Truncate_ReadWrite_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Truncate_ReadWrite_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Truncate, tc::io::FileAccess::ReadWrite);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, true, true);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Truncate_ReadWrite_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Truncate_ReadWrite_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Truncate, tc::io::FileAccess::ReadWrite);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, true, true, true);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::io::FileNotFoundException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

// Constructor_Append

void io_FileStream_TestClass::test_Constructor_Append_Read_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Append_Read_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Append, tc::io::FileAccess::Read);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Append_Read_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Append_Read_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Append, tc::io::FileAccess::Read);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Append_Write_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Append_Write_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Append, tc::io::FileAccess::Write);

			StreamTestUtil::constructor_TestHelper(stream, kRandomString.size(), kRandomString.size(), false, true, false);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Append_Write_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Append_Write_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Append, tc::io::FileAccess::Write);

			StreamTestUtil::constructor_TestHelper(stream, 0, 0, false, true, false);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Append_ReadWrite_FileExists()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Append_ReadWrite_FileExists : " << std::flush;
	try
	{
	
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.length());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Append, tc::io::FileAccess::ReadWrite);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_Append_ReadWrite_FileNotExist()
{
	std::cout << "[tc::io::FileStream] test_Constructor_Append_ReadWrite_FileNotExist : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Append, tc::io::FileAccess::ReadWrite);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

// illegal constructor args

void io_FileStream_TestClass::test_Constructor_IllegalMode()
{
	std::cout << "[tc::io::FileStream] test_Constructor_IllegalMode : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode(55), tc::io::FileAccess::ReadWrite);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentOutOfRangeException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_IllegalAccess()
{
	std::cout << "[tc::io::FileStream] test_Constructor_IllegalAccess : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::CreateNew, tc::io::FileAccess(55));

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentOutOfRangeException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_DirectoryPath()
{
	std::cout << "[tc::io::FileStream] test_Constructor_DirectoryPath : " << std::flush;
	try
	{
		helper_CreateDirectory(kAsciiFilePath);

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::io::IOException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::UnauthorisedAccessException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}

		helper_DeleteDirectory(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Constructor_CreateThenReopenFileWithUnicodePath()
{
	std::cout << "[tc::io::FileStream] test_Constructor_CreateThenReopenFileWithUnicodePath : " << std::flush;
	try
	{
		try 
		{
			auto stream = tc::io::FileStream(kUtf8TestPath, tc::io::FileMode::Create, tc::io::FileAccess::ReadWrite);

			stream.dispose();

			stream = tc::io::FileStream(kUtf8TestPath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kUtf8TestPath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

// seek() tests
void io_FileStream_TestClass::test_Seek_EmptyFile()
{
	std::cout << "[tc::io::FileStream] test_Seek_EmptyFile : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, nullptr, 0);

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			StreamTestUtil::seek_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, 0, 0);
			StreamTestUtil::seek_TestHelper(stream, 0, tc::io::SeekOrigin::Current, 0, 0);
			StreamTestUtil::seek_TestHelper(stream, 0, tc::io::SeekOrigin::End, 0, 0);

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

void io_FileStream_TestClass::test_Seek_CreatedFile()
{
	std::cout << "[tc::io::FileStream] test_Seek_CreatedFile : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			StreamTestUtil::seek_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, 0, 0);
			StreamTestUtil::seek_TestHelper(stream, 0, tc::io::SeekOrigin::Current, 0, 0);
			StreamTestUtil::seek_TestHelper(stream, 100, tc::io::SeekOrigin::Current, 100, 100);
			StreamTestUtil::seek_TestHelper(stream, 50, tc::io::SeekOrigin::Current, 150, 150);
			StreamTestUtil::seek_TestHelper(stream, 0, tc::io::SeekOrigin::End, 200, 200);
			StreamTestUtil::seek_TestHelper(stream, 0, tc::io::SeekOrigin::Current, 200, 200);
			StreamTestUtil::seek_TestHelper(stream, 1, tc::io::SeekOrigin::Begin, 1, 1);
			StreamTestUtil::seek_TestHelper(stream, 0, tc::io::SeekOrigin::Current, 1, 1);
			StreamTestUtil::seek_TestHelper(stream, -1, tc::io::SeekOrigin::End, 199, 199);
			StreamTestUtil::seek_TestHelper(stream, -198, tc::io::SeekOrigin::Current, 1, 1);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Seek_AppendMode()
{
	std::cout << "[tc::io::FileStream] test_Seek_AppendMode : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Append, tc::io::FileAccess::Write);

			StreamTestUtil::seek_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, 0, 0);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::io::IOException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Seek_PositionBeforeFileBegin()
{
	std::cout << "[tc::io::FileStream] test_Seek_PositionBeforeFileBegin : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			StreamTestUtil::seek_TestHelper(stream, -1, tc::io::SeekOrigin::Begin, -1, -1);

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentOutOfRangeException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Seek_PositionAfterFileEnd()
{
	std::cout << "[tc::io::FileStream] test_Seek_PositionAfterFileEnd : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			StreamTestUtil::seek_TestHelper(stream, stream.length(), tc::io::SeekOrigin::Begin, stream.length(), stream.length());

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

// read tests

void io_FileStream_TestClass::test_Read_NoData()
{
	std::cout << "[tc::io::FileStream] test_Read_NoData : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			StreamTestUtil::read_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, kRandomString.size(), 0, 0, 0);
			StreamTestUtil::read_TestHelper(stream, stream.length()/2, tc::io::SeekOrigin::Begin, kRandomString.size(), 0, 0, stream.length()/2);
			StreamTestUtil::read_TestHelper(stream, 0, tc::io::SeekOrigin::End, kRandomString.size(), 0, 0, stream.length());

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Read_SomeDataFromZero()
{
	std::cout << "[tc::io::FileStream] test_Read_SomeDataFromZero : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			StreamTestUtil::read_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, kRandomString.size(), 0x01, 0x01, 0x01, (const byte_t*)kRandomString.c_str());
			StreamTestUtil::read_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, kRandomString.size(), 0x20, 0x20, 0x20, (const byte_t*)kRandomString.c_str());
			StreamTestUtil::read_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, kRandomString.size(), kRandomString.size()/2, kRandomString.size()/2, kRandomString.size()/2, (const byte_t*)kRandomString.c_str());

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Read_SomeDataFromMiddle()
{
	std::cout << "[tc::io::FileStream] test_Read_SomeDataFromMiddle : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			int64_t offset;
			size_t size;

			offset = 01;
			size = 20;
			StreamTestUtil::read_TestHelper(stream, offset, tc::io::SeekOrigin::Begin, kRandomString.size(), size, size, offset + size, (const byte_t*)kRandomString.c_str() + offset);

			offset = 67;
			size = 100;
			StreamTestUtil::read_TestHelper(stream, offset, tc::io::SeekOrigin::Begin, kRandomString.size(), size, size, offset + size, (const byte_t*)kRandomString.c_str() + offset);

			offset = stream.length() / 2;
			size = 80;
			StreamTestUtil::read_TestHelper(stream, offset, tc::io::SeekOrigin::Begin, kRandomString.size(), size, size, offset + size, (const byte_t*)kRandomString.c_str() + offset);
			

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Read_AllData()
{
	std::cout << "[tc::io::FileStream] test_Read_AllData : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			StreamTestUtil::read_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, kRandomString.size(), kRandomString.size(), kRandomString.size(), kRandomString.size(), (const byte_t*)kRandomString.c_str());

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Read_TooMuchData()
{
	std::cout << "[tc::io::FileStream] test_Read_TooMuchData : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			StreamTestUtil::read_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, kRandomString.size()*2, kRandomString.size()*2, kRandomString.size(), kRandomString.size(), (const byte_t*)kRandomString.c_str());

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Read_BeyondEnd()
{
	std::cout << "[tc::io::FileStream] test_Read_BeyondEnd : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			StreamTestUtil::read_TestHelper(stream, 1, tc::io::SeekOrigin::End, kRandomString.size(), kRandomString.size(), 0, kRandomString.size() + 1, (const byte_t*)kRandomString.c_str());

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Read_CanReadFalse()
{
	std::cout << "[tc::io::FileStream] test_Read_CanReadFalse : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Write);

			StreamTestUtil::read_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, kRandomString.size(), kRandomString.size(), 0, kRandomString.size(), (const byte_t*)kRandomString.c_str());

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::NotSupportedException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Read_NullDstPointer()
{
	std::cout << "[tc::io::FileStream] test_Read_NullDstPointer : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), kRandomString.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			StreamTestUtil::read_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, 0, kRandomString.size(), 0, kRandomString.size(), (const byte_t*)kRandomString.c_str());

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentNullException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

// write() test

void io_FileStream_TestClass::test_Write_NoData()
{
	std::cout << "[tc::io::FileStream] test_Write_NoData : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, nullptr, 0);

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Write);

			StreamTestUtil::write_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, (const byte_t*)kRandomString.c_str(), 0, 0);

			stream.dispose();

			helper_ValidateFileContents(kAsciiFilePath, (const byte_t*)kRandomString.c_str(), 0);

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Write_OverwriteSomeDataFromZero()
{
	std::cout << "[tc::io::FileStream] test_Write_OverwriteSomeDataFromZero : " << std::flush;
	try
	{
		auto padding = tc::io::PaddingSource(0xff, kRandomString.size());
		auto padding_data = padding.pullData(0, padding.length());
		helper_CreateFileForReading(kAsciiFilePath, padding_data.data(), padding_data.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Write);

			const byte_t* original_ptr = padding_data.data();
			size_t original_size = padding_data.size();

			const byte_t* written_data_ptr = (const byte_t*)kRandomString.c_str();
			size_t written_data_size = 30;
			size_t written_data_offset = 0;

			StreamTestUtil::write_TestHelper(stream, written_data_offset, tc::io::SeekOrigin::Begin, written_data_ptr, written_data_size, written_data_offset + written_data_size);
			stream.dispose();

			auto expected_file_layout = tc::ByteData(std::max<size_t>(original_size, written_data_offset + written_data_size));

			memcpy(expected_file_layout.data(), original_ptr, original_size);
			memcpy(expected_file_layout.data() + written_data_offset, written_data_ptr, written_data_size);

			helper_ValidateFileContents(kAsciiFilePath, expected_file_layout.data(), expected_file_layout.size());

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Write_OverwriteSomeDataFromMiddle()
{
	std::cout << "[tc::io::FileStream] test_Write_OverwriteSomeDataFromMiddle : " << std::flush;
	try
	{
		auto padding = tc::io::PaddingSource(0xee, kRandomString.size());
		auto padding_data = padding.pullData(0, padding.length());
		helper_CreateFileForReading(kAsciiFilePath, padding_data.data(), padding_data.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Write);

			const byte_t* original_ptr = padding_data.data();
			size_t original_size = padding_data.size();

			const byte_t* written_data_ptr = (const byte_t*)kRandomString.c_str();
			size_t written_data_size = 90;
			size_t written_data_offset = 50;

			StreamTestUtil::write_TestHelper(stream, written_data_offset, tc::io::SeekOrigin::Begin, written_data_ptr, written_data_size, written_data_offset + written_data_size);
			stream.dispose();

			auto expected_file_layout = tc::ByteData(std::max<size_t>(original_size, written_data_offset + written_data_size));

			memcpy(expected_file_layout.data(), original_ptr, original_size);
			memcpy(expected_file_layout.data() + written_data_offset, written_data_ptr, written_data_size);

			helper_ValidateFileContents(kAsciiFilePath, expected_file_layout.data(), expected_file_layout.size());

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Write_ExtendStreamSizeThruWritingDataFromZero()
{
	std::cout << "[tc::io::FileStream] test_Write_ExtendStreamSizeThruWritingDataFromZero : " << std::flush;
	try
	{
		auto padding = tc::io::PaddingSource(0xee, 0x20);
		auto padding_data = padding.pullData(0, padding.length());
		helper_CreateFileForReading(kAsciiFilePath, padding_data.data(), padding_data.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Write);

			const byte_t* original_ptr = padding_data.data();
			size_t original_size = padding_data.size();

			const byte_t* written_data_ptr = (const byte_t*)kRandomString.c_str();
			size_t written_data_size = 90;
			size_t written_data_offset = 0;

			StreamTestUtil::write_TestHelper(stream, written_data_offset, tc::io::SeekOrigin::Begin, written_data_ptr, written_data_size, written_data_offset + written_data_size, written_data_offset + written_data_size);
			stream.dispose();

			auto expected_file_layout = tc::ByteData(std::max<size_t>(original_size, written_data_offset + written_data_size));

			memcpy(expected_file_layout.data(), original_ptr, original_size);
			memcpy(expected_file_layout.data() + written_data_offset, written_data_ptr, written_data_size);

			helper_ValidateFileContents(kAsciiFilePath, expected_file_layout.data(), expected_file_layout.size());

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Write_ExtendStreamSizeThruWritingDataFromMiddle()
{
	std::cout << "[tc::io::FileStream] test_Write_ExtendStreamSizeThruWritingDataFromMiddle : " << std::flush;
	try
	{
		auto padding = tc::io::PaddingSource(0xee, 0x20);
		auto padding_data = padding.pullData(0, padding.length());
		helper_CreateFileForReading(kAsciiFilePath, padding_data.data(), padding_data.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Write);

			const byte_t* original_ptr = padding_data.data();
			size_t original_size = padding_data.size();

			const byte_t* written_data_ptr = (const byte_t*)kRandomString.c_str();
			size_t written_data_size = 90;
			size_t written_data_offset = 0x10;

			StreamTestUtil::write_TestHelper(stream, written_data_offset, tc::io::SeekOrigin::Begin, written_data_ptr, written_data_size, written_data_offset + written_data_size, written_data_offset + written_data_size);
			stream.dispose();

			auto expected_file_layout = tc::ByteData(std::max<size_t>(original_size, written_data_offset + written_data_size));

			memcpy(expected_file_layout.data(), original_ptr, original_size);
			memcpy(expected_file_layout.data() + written_data_offset, written_data_ptr, written_data_size);

			helper_ValidateFileContents(kAsciiFilePath, expected_file_layout.data(), expected_file_layout.size());

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Write_BeyondEnd()
{
	std::cout << "[tc::io::FileStream] test_Write_BeyondEnd : " << std::flush;
	try
	{
		auto padding = tc::io::PaddingSource(0xee, 0x20);
		auto padding_data = padding.pullData(0, padding.length());
		helper_CreateFileForReading(kAsciiFilePath, padding_data.data(), padding_data.size());

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::OpenOrCreate, tc::io::FileAccess::Write);

			const byte_t* original_ptr = padding_data.data();
			size_t original_size = padding_data.size();

			const byte_t* written_data_ptr = (const byte_t*)kRandomString.c_str();
			size_t written_data_size = 90;
			size_t written_data_offset = original_size + 4;

			StreamTestUtil::write_TestHelper(stream, written_data_offset, tc::io::SeekOrigin::Begin, written_data_ptr, written_data_size, written_data_offset + written_data_size, written_data_offset + written_data_size);
			stream.dispose();

			auto expected_file_layout = tc::ByteData(std::max<size_t>(original_size, written_data_offset + written_data_size));
			memcpy(expected_file_layout.data(), original_ptr, original_size);
			memcpy(expected_file_layout.data() + written_data_offset, written_data_ptr, written_data_size);

			helper_ValidateFileContents(kAsciiFilePath, expected_file_layout.data(), expected_file_layout.size());

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Write_CanWriteFalse()
{
	std::cout << "[tc::io::FileStream] test_Write_CanWriteFalse : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, nullptr, 0);

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Read);

			StreamTestUtil::write_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, (const byte_t*)kRandomString.c_str(), kRandomString.size(), kRandomString.size());

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::NotSupportedException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_FileStream_TestClass::test_Write_NullSrcPointer()
{
	std::cout << "[tc::io::FileStream] test_Write_NullSrcPointer : " << std::flush;
	try
	{
		helper_CreateFileForReading(kAsciiFilePath, nullptr, 0);

		try 
		{
			auto stream = tc::io::FileStream(kAsciiFilePath, tc::io::FileMode::Open, tc::io::FileAccess::Write);

			StreamTestUtil::write_TestHelper(stream, 0, tc::io::SeekOrigin::Begin, nullptr, kRandomString.size(), kRandomString.size());

			std::cout << "FAIL" << std::endl;
		}
		catch (const tc::ArgumentNullException& e)
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (Wrong Exception)(" << e.error() << ")" << std::endl;
		}

		helper_DeleteFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

// helper code

void io_FileStream_TestClass::helper_CreateFileForReading(const std::string& path, const uint8_t* data, size_t data_len)
{
	auto test_file = std::ofstream(path, std::ios::binary);
	if (data != nullptr && data_len != 0)
	{
		test_file.write((char*)data, data_len);
	}
	test_file.close();
}

void io_FileStream_TestClass::helper_ValidateFileContents(const std::string& path, const uint8_t* data, size_t data_len)
{
	auto test_file = std::ifstream(path, std::ios::binary);

	if (test_file.fail())
	{
		throw tc::Exception("helper_ValidateFileContents : Failed to open file");
	}

	if (data == nullptr || data_len == 0)
	{
		return;
	}

	auto datablob = tc::ByteData(data_len);
	test_file.read((char*)datablob.data(), data_len);

	if (test_file.fail())
	{
		throw tc::Exception("helper_ValidateFileContents : Failed to read file");
	}

	test_file.close();

	if (memcmp(datablob.data(), data, data_len) != 0)
	{
		throw tc::Exception("helper_ValidateFileContents : Invalid file contents");
	}
} 

void io_FileStream_TestClass::helper_DeleteFile(const std::string& path)
{
	tc::io::LocalFileSystem s;
	s.removeFile(path);
}

void io_FileStream_TestClass::helper_CreateDirectory(const std::string& path)
{
	tc::io::LocalFileSystem s;
	s.createDirectory(path);
}

void io_FileStream_TestClass::helper_DeleteDirectory(const std::string& path)
{
	tc::io::LocalFileSystem s;
	s.removeDirectory(path);
}