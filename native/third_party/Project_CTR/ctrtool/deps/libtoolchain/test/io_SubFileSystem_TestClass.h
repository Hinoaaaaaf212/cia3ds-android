#pragma once
#include "ITestClass.h"

class io_SubFileSystem_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void testBaseFileSystemRetainsWorkingDirectory();
	void testGetSetWorkingDirectory();
	void testCreateFile();
	void testOpenFile();
	void testRemoveFile();
	void testCreateDirectory();
	void testRemoveDirectory();
	void testGetDirectoryListing();
	void testNavigateUpSubFileSystemEscape();
	void testOpenFileOutsideSubFileSystem();
};