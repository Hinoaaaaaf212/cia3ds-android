#pragma once
#include "ITestClass.h"

class io_LocalFileSystem_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_CreateFile_NotExist();
	void test_CreateFile_DoesExist();
	void test_CreateFile_UnicodePath();
	void test_RemoveFile_DoesExist();
	void test_RemoveFile_NotExist();
	void test_RemoveFile_UnicodePath();
	void test_CreateDirectory_NotExist();
	void test_CreateDirectory_DoesExist();
	void test_CreateDirectory_UnicodePath();
	void test_RemoveDirectory_DoesExist();
	void test_RemoveDirectory_NotExist();
	void test_RemoveDirectory_UnicodePath();
	void test_RemoveDirectory_HasChildren();
	void test_RemoveDirectory_NotDirectoryActuallyFile();
	void test_GetDirectoryListing_DoesExist();
	void test_GetDirectoryListing_NotExist();
	void test_GetDirectoryListing_UnicodePath();
	void test_ChangeWorkingDirectory_DoesExist();
	void test_ChangeWorkingDirectory_NotExist();
	void test_ChangeWorkingDirectory_UnicodePath();

	static std::string kDirPath;
	static std::string kUtf8DirPath;
	static std::string kAsciiFilePath;
	static std::string kUtf8TestPath;
	static std::string kNotExistFilePath;
};