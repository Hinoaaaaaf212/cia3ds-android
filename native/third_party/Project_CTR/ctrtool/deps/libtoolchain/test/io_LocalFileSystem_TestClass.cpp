#include <iostream>
#include <sstream>

#include "io_LocalFileSystem_TestClass.h"

#include <tc.h>

std::string io_LocalFileSystem_TestClass::kDirPath = "./testdir";
std::string io_LocalFileSystem_TestClass::kUtf8DirPath = "./ЀЁЂЃЄЅテスト/";
std::string io_LocalFileSystem_TestClass::kAsciiFilePath = "LocalFileTest.bin";
std::string io_LocalFileSystem_TestClass::kUtf8TestPath = "ЀЁЂЃЄЅ-מבחן-тест-テスト.bin";
std::string io_LocalFileSystem_TestClass::kNotExistFilePath = "ThisDoesNotExist.bin";

void io_LocalFileSystem_TestClass::runAllTests(void)
{
	std::cout << "[tc::io::LocalFileSystem] START" << std::endl;
	test_CreateFile_NotExist();
	test_CreateFile_DoesExist();
	test_CreateFile_UnicodePath();
	test_RemoveFile_DoesExist();
	test_RemoveFile_NotExist();
	test_RemoveFile_UnicodePath();
	
	test_CreateDirectory_NotExist();
	test_CreateDirectory_DoesExist();
	test_CreateDirectory_UnicodePath();
	test_RemoveDirectory_DoesExist();
	test_RemoveDirectory_NotExist();
	test_RemoveDirectory_UnicodePath();
	test_RemoveDirectory_HasChildren();
	test_RemoveDirectory_NotDirectoryActuallyFile();
	test_GetDirectoryListing_DoesExist();
	test_GetDirectoryListing_NotExist();
	test_GetDirectoryListing_UnicodePath();
	test_ChangeWorkingDirectory_DoesExist();
	test_ChangeWorkingDirectory_NotExist();
	test_ChangeWorkingDirectory_UnicodePath();
	std::cout << "[tc::io::LocalFileSystem] END" << std::endl;
}

void io_LocalFileSystem_TestClass::test_CreateFile_NotExist()
{
	std::cout << "[tc::io::LocalFileSystem] test_CreateFile_NotExist : " << std::flush;
	try 
	{
		tc::io::LocalFileSystem local_storage;

		local_storage.createFile(kAsciiFilePath);

		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e)
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_CreateFile_DoesExist()
{
	std::cout << "[tc::io::LocalFileSystem] test_CreateFile_DoesExist : " << std::flush;
	try 
	{
		tc::io::LocalFileSystem local_storage;

		local_storage.createFile(kAsciiFilePath);

		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e)
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_CreateFile_UnicodePath()
{
	std::cout << "[tc::io::LocalFileSystem] test_CreateFile_UnicodePath : " << std::flush;
	try 
	{
		tc::io::LocalFileSystem local_storage;

		local_storage.createFile(kUtf8TestPath);

		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e)
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_RemoveFile_DoesExist()
{
	std::cout << "[tc::io::LocalFileSystem] test_RemoveFile_DoesExist : " << std::flush;
	try 
	{
		tc::io::LocalFileSystem local_storage;

		local_storage.removeFile(kAsciiFilePath);

		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e)
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_RemoveFile_NotExist()
{
	std::cout << "[tc::io::LocalFileSystem] test_RemoveFile_NotExist : " << std::flush;
	try 
	{
		tc::io::LocalFileSystem local_storage;

		local_storage.removeFile(kNotExistFilePath);
		std::cout << "FAIL (Did not throw exception when stream was not present on FS)" << std::endl;
	}
	catch (const tc::Exception& e) 
	{
		std::cout << "PASS (" << e.error() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_RemoveFile_UnicodePath()
{
	std::cout << "[tc::io::LocalFileSystem] test_RemoveFile_UnicodePath : " << std::flush;
	try 
	{
		tc::io::LocalFileSystem local_storage;

		local_storage.removeFile(kUtf8TestPath);

		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e)
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_CreateDirectory_NotExist()
{
	std::cout << "[tc::io::LocalFileSystem] test_CreateDirectory_NotExist : " << std::flush;
	try 
	{
		tc::io::LocalFileSystem local_storage;

		local_storage.createDirectory(kDirPath);
		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e) 
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_CreateDirectory_DoesExist()
{
	std::cout << "[tc::io::LocalFileSystem] test_CreateDirectory_DoesExist : " << std::flush;
	try 
	{
		tc::io::LocalFileSystem local_storage;

		local_storage.createDirectory(kDirPath);
		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e) 
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_CreateDirectory_UnicodePath()
{
	std::cout << "[tc::io::LocalFileSystem] test_CreateDirectory_UnicodePath : " << std::flush;
	try 
	{
		tc::io::LocalFileSystem local_storage;

		local_storage.createDirectory(kUtf8DirPath);
		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e) 
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_RemoveDirectory_DoesExist()
{
	std::cout << "[tc::io::LocalFileSystem] test_RemoveDirectory_DoesExist : " << std::flush;
	try
	{
		tc::io::LocalFileSystem local_storage;

		local_storage.removeDirectory(kDirPath);
		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e)
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_RemoveDirectory_NotExist()
{
	std::cout << "[tc::io::LocalFileSystem] test_RemoveDirectory_NotExist : " << std::flush;
	try 
	{
		tc::io::LocalFileSystem local_storage;

		local_storage.removeDirectory(kDirPath);
		std::cout << "FAIL (did not throw exception on expected error)" << std::endl;
	}
	catch (const tc::Exception& e) 
	{
		std::cout << "PASS (" << e.error() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_RemoveDirectory_UnicodePath()
{
	std::cout << "[tc::io::LocalFileSystem] test_RemoveDirectory_UnicodePath : " << std::flush;
	try 
	{
		tc::io::LocalFileSystem local_storage;

		local_storage.removeDirectory(kUtf8DirPath);
		std::cout << "PASS" << std::endl;
	}
	catch (const tc::Exception& e) 
	{
		std::cout << "FAIL (" << e.error() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_RemoveDirectory_HasChildren()
{
	std::cout << "[tc::io::LocalFileSystem] test_RemoveDirectory_HasChildren : " << std::flush;
	try
	{
		tc::io::LocalFileSystem local_storage;
	
		tc::io::Path dir_child_path = tc::io::Path(kDirPath) + tc::io::Path(kUtf8DirPath);
		tc::io::Path stream_child_path = tc::io::Path(kDirPath) + tc::io::Path(kAsciiFilePath);

		try 
		{
			local_storage.createDirectory(kDirPath);
			local_storage.createDirectory(dir_child_path);
			local_storage.createFile(stream_child_path);
			local_storage.removeDirectory(kDirPath);
			std::cout << "FAIL (did not throw exception on expected error)" << std::endl;
		}
		catch (const tc::Exception& e) 
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}

		local_storage.removeDirectory(dir_child_path);
		local_storage.removeFile(stream_child_path);
		local_storage.removeDirectory(kDirPath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_RemoveDirectory_NotDirectoryActuallyFile()
{
	std::cout << "[tc::io::LocalFileSystem] test_RemoveDirectory_NotDirectoryActuallyFile : " << std::flush;
	try
	{
		tc::io::LocalFileSystem local_storage;
		try 
		{
			local_storage.createFile(kAsciiFilePath);
			local_storage.removeDirectory(kAsciiFilePath);
			std::cout << "FAIL (did not throw exception on expected error)" << std::endl;
		}
		catch (const tc::Exception& e) 
		{
			std::cout << "PASS (" << e.error() << ")" << std::endl;
		}

		local_storage.removeFile(kAsciiFilePath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_GetDirectoryListing_DoesExist()
{
	std::cout << "[tc::io::LocalFileSystem] test_GetDirectoryListing_DoesExist : " << std::flush;
	try
	{
		tc::io::LocalFileSystem local_storage;

		std::vector<std::string> file_list;
		std::vector<std::string> dir_list;
		
		file_list.push_back("streamA.bin");
		file_list.push_back("streamB.bin");
		file_list.push_back("streamC.bin");
		file_list.push_back("streamD.bin");

		dir_list.push_back("dir000");
		dir_list.push_back("dir001");
		dir_list.push_back("dir002");
		dir_list.push_back("dir003");

		local_storage.createDirectory(kDirPath);

		for (size_t i = 0; i < file_list.size(); i++)
		{
			local_storage.createFile(tc::io::Path(kDirPath) + tc::io::Path(file_list[i]));
		}

		for (size_t i = 0; i < dir_list.size(); i++)
		{
			local_storage.createDirectory(tc::io::Path(kDirPath) + tc::io::Path(dir_list[i]));
		}

		try
		{
			tc::io::sDirectoryListing info;

			local_storage.getDirectoryListing(kDirPath, info);
			
			// + 2 for "." & ".."
			if (info.dir_list.size() != (dir_list.size() + 2))
			{
				throw tc::Exception("Unexpected directory count");
			}

			if (info.file_list.size() != file_list.size())
			{
				throw tc::Exception("Unexpected stream count");
			}

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		for (size_t i = 0; i < file_list.size(); i++)
		{
			local_storage.removeFile(tc::io::Path(kDirPath) + tc::io::Path(file_list[i]));
		}

		for (size_t i = 0; i < dir_list.size(); i++)
		{
			local_storage.removeDirectory(tc::io::Path(kDirPath) + tc::io::Path(dir_list[i]));
		}

		local_storage.removeDirectory(tc::io::Path(kDirPath));
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_GetDirectoryListing_NotExist()
{
	std::cout << "[tc::io::LocalFileSystem] test_GetDirectoryListing_NotExist : " << std::flush;
	try 
	{
		tc::io::LocalFileSystem local_storage;

		tc::io::sDirectoryListing info;
		local_storage.getDirectoryListing(kNotExistFilePath, info);
		std::cout << "FAIL (did not throw exception on expected error)" << std::endl;
	}
	catch (const tc::Exception& e) 
	{
		std::cout << "PASS (" << e.error() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_GetDirectoryListing_UnicodePath()
{
	std::cout << "[tc::io::LocalFileSystem] test_GetDirectoryListing_UnicodePath : " << std::flush;
	try
	{
		tc::io::LocalFileSystem local_storage;

		std::vector<std::string> file_list;
		std::vector<std::string> dir_list;
		
		file_list.push_back("streamA.bin");
		file_list.push_back("streamB.bin");
		file_list.push_back("streamC.bin");
		file_list.push_back("streamD.bin");

		dir_list.push_back("dir000");
		dir_list.push_back("dir001");
		dir_list.push_back("dir002");
		dir_list.push_back("dir003");

		local_storage.createDirectory(kUtf8DirPath);

		for (size_t i = 0; i < file_list.size(); i++)
		{
			local_storage.createFile(tc::io::Path(kUtf8DirPath) + tc::io::Path(file_list[i]));
		}

		for (size_t i = 0; i < dir_list.size(); i++)
		{
			local_storage.createDirectory(tc::io::Path(kUtf8DirPath) + tc::io::Path(dir_list[i]));
		}

		try
		{
			tc::io::sDirectoryListing info;

			local_storage.getDirectoryListing(kUtf8DirPath, info);
			
			// + 2 for "." & ".."
			if (info.dir_list.size() != (dir_list.size() + 2))
			{
				throw tc::Exception("Unexpected directory count");
			}

			if (info.file_list.size() != file_list.size())
			{
				throw tc::Exception("Unexpected stream count");
			}

			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e)
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		for (size_t i = 0; i < file_list.size(); i++)
		{
			local_storage.removeFile(tc::io::Path(kUtf8DirPath) + tc::io::Path(file_list[i]));
		}

		for (size_t i = 0; i < dir_list.size(); i++)
		{
			local_storage.removeDirectory(tc::io::Path(kUtf8DirPath) + tc::io::Path(dir_list[i]));
		}

		local_storage.removeDirectory(tc::io::Path(kUtf8DirPath));
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_ChangeWorkingDirectory_DoesExist()
{
	std::cout << "[tc::io::LocalFileSystem] test_ChangeWorkingDirectory_DoesExist : " << std::flush;
	try
	{
		tc::io::LocalFileSystem local_storage;
		
		tc::io::Path old_dir;
		local_storage.getWorkingDirectory(old_dir);

		try 
		{
			local_storage.createDirectory(kDirPath);
			local_storage.setWorkingDirectory(kDirPath);
			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e) 
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		local_storage.setWorkingDirectory(old_dir);
		local_storage.removeDirectory(tc::io::Path(kDirPath));
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_ChangeWorkingDirectory_NotExist()
{
	std::cout << "[tc::io::LocalFileSystem] test_ChangeWorkingDirectory_NotExist : " << std::flush;
	try
	{
		tc::io::LocalFileSystem local_storage;
		
		local_storage.setWorkingDirectory(kNotExistFilePath);
		std::cout << "FAIL (did not throw exception on expected error)" << std::endl;
	}
	catch (const tc::Exception& e)
	{
		std::cout << "PASS (" << e.error() << ")" << std::endl;
	}
}

void io_LocalFileSystem_TestClass::test_ChangeWorkingDirectory_UnicodePath()
{
	std::cout << "[tc::io::LocalFileSystem] test_ChangeWorkingDirectory_UnicodePath : " << std::flush;
	try
	{
		tc::io::LocalFileSystem local_storage;
		
		tc::io::Path old_dir;
		local_storage.getWorkingDirectory(old_dir);

		try 
		{
			local_storage.createDirectory(kUtf8DirPath);
			local_storage.setWorkingDirectory(kUtf8DirPath);
			std::cout << "PASS" << std::endl;
		}
		catch (const tc::Exception& e) 
		{
			std::cout << "FAIL (" << e.error() << ")" << std::endl;
		}

		local_storage.setWorkingDirectory(old_dir);
		local_storage.removeDirectory(kUtf8DirPath);
	}
	catch (const std::exception& e)
	{
		std::cout << "UNHANDLED EXCEPTION (" << e.what() << ")" << std::endl;
	}
}