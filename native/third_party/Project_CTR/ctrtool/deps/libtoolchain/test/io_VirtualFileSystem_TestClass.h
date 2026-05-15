#pragma once
#include "ITestClass.h"

class io_VirtualFileSystem_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void test_CreateUninitializedFs_DefaultConstructor();
	void test_BadFsSnapshot_CreateConstructor();
	void test_CreateFs_CreateConstructor();
	void test_ThrowsOnBadPermissions_OpenFile();
	void test_ThrowsOnBadFileEntry_OpenFile();
	void test_ThrowsOnBadFileEntry_GetDirectoryListing();
	void test_ThrowsOnBadFileEntry_SetWorkingDirectory();
	void test_WorksForAllValidPaths_OpenFile();
	void test_WorksForAllValidPaths_GetDirectoryListing();
	void test_WorksForAllValidPaths_SetWorkingDirectory();
	void test_WorksForAllValidPaths_GetWorkingDirectory();
	void test_DisposeWillChangeStateToUninitialized();
};