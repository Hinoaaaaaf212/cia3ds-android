#pragma once
#include "ITestClass.h"

class io_FileStream_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_DefaultConstructor();

	void test_Constructor_CreateNew_Read_FileExists();
	void test_Constructor_CreateNew_Read_FileNotExist();
	void test_Constructor_CreateNew_Write_FileExists();
	void test_Constructor_CreateNew_Write_FileNotExist();
	void test_Constructor_CreateNew_ReadWrite_FileExists();
	void test_Constructor_CreateNew_ReadWrite_FileNotExist();

	void test_Constructor_Create_Read_FileExists();
	void test_Constructor_Create_Read_FileNotExist();
	void test_Constructor_Create_Write_FileExists();
	void test_Constructor_Create_Write_FileNotExist();
	void test_Constructor_Create_ReadWrite_FileExists();
	void test_Constructor_Create_ReadWrite_FileNotExist();

	void test_Constructor_Open_Read_FileExists();
	void test_Constructor_Open_Read_FileNotExist();
	void test_Constructor_Open_Write_FileExists();
	void test_Constructor_Open_Write_FileNotExist();
	void test_Constructor_Open_ReadWrite_FileExists();
	void test_Constructor_Open_ReadWrite_FileNotExist();

	void test_Constructor_OpenOrCreate_Read_FileExists();
	void test_Constructor_OpenOrCreate_Read_FileNotExist();
	void test_Constructor_OpenOrCreate_Write_FileExists();
	void test_Constructor_OpenOrCreate_Write_FileNotExist();
	void test_Constructor_OpenOrCreate_ReadWrite_FileExists();
	void test_Constructor_OpenOrCreate_ReadWrite_FileNotExist();

	void test_Constructor_Truncate_Read_FileExists();
	void test_Constructor_Truncate_Read_FileNotExist();
	void test_Constructor_Truncate_Write_FileExists();
	void test_Constructor_Truncate_Write_FileNotExist();
	void test_Constructor_Truncate_ReadWrite_FileExists();
	void test_Constructor_Truncate_ReadWrite_FileNotExist();

	void test_Constructor_Append_Read_FileExists();
	void test_Constructor_Append_Read_FileNotExist();
	void test_Constructor_Append_Write_FileExists();
	void test_Constructor_Append_Write_FileNotExist();
	void test_Constructor_Append_ReadWrite_FileExists();
	void test_Constructor_Append_ReadWrite_FileNotExist();

	void test_Constructor_IllegalMode();
	void test_Constructor_IllegalAccess();

	void test_Constructor_DirectoryPath();
	void test_Constructor_CreateThenReopenFileWithUnicodePath();

	void test_Seek_EmptyFile();
	void test_Seek_CreatedFile();
	void test_Seek_AppendMode();
	void test_Seek_PositionBeforeFileBegin();
	void test_Seek_PositionAfterFileEnd();

	void test_Read_NoData();
	void test_Read_SomeDataFromZero();
	void test_Read_SomeDataFromMiddle();
	void test_Read_AllData();
	void test_Read_TooMuchData();
	void test_Read_BeyondEnd();
	void test_Read_CanReadFalse();
	void test_Read_NullDstPointer();

	void test_Write_NoData();
	void test_Write_OverwriteSomeDataFromZero();
	void test_Write_OverwriteSomeDataFromMiddle();
	void test_Write_ExtendStreamSizeThruWritingDataFromZero();
	void test_Write_ExtendStreamSizeThruWritingDataFromMiddle();
	void test_Write_BeyondEnd();
	void test_Write_CanWriteFalse();
	void test_Write_NullSrcPointer();

	void helper_CreateFileForReading(const std::string& path, const uint8_t* data, size_t data_len);
	void helper_ValidateFileContents(const std::string& path, const uint8_t* data, size_t data_len); 
	void helper_DeleteFile(const std::string& path);
	void helper_CreateDirectory(const std::string& path);
	void helper_DeleteDirectory(const std::string& path);

	static std::string kAsciiFilePath;
	static std::string kUtf8TestPath;
	static std::string kNotExistFilePath;
	static std::string kTestPhrase;
	static std::string kRandomString;
};