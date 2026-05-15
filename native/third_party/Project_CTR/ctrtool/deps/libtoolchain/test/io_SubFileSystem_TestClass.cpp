#include <tc/Exception.h>
#include <iostream>

#include "io_SubFileSystem_TestClass.h"
#include "FileSystemTestUtil.h"
#include "StreamTestUtil.h"

void io_SubFileSystem_TestClass::runAllTests(void)
{
	std::cout << "[tc::io::SubFileSystem] START" << std::endl;
	testBaseFileSystemRetainsWorkingDirectory();
	testGetSetWorkingDirectory();
	testCreateFile();
	testOpenFile();
	testRemoveFile();
	testCreateDirectory();
	testRemoveDirectory();
	testGetDirectoryListing();
	testNavigateUpSubFileSystemEscape();
	testOpenFileOutsideSubFileSystem();
	std::cout << "[tc::io::SubFileSystem] END" << std::endl;
}

void io_SubFileSystem_TestClass::testBaseFileSystemRetainsWorkingDirectory()
{
	std::cout << "[tc::io::SubFileSystem] testBaseFileSystemRetainsWorkingDirectory : " << std::flush;
	try
	{
		class DummyFileSystem : public FileSystemTestUtil::DummyFileSystemBase
		{
		public:
			DummyFileSystem()
			{
			}
		};
		
		// define sub filesystem base path
		tc::io::Path subfilesystem_base_path = tc::io::Path("/home/jakcron/source/LibToolChain/testdir");

		// define base filesystem
		DummyFileSystem filesystem = DummyFileSystem();

		// test sub filesystem creation & test base working directory is maintained after SubFileSystem constructor
		try
		{
			// save a copy of the base filesystem working directory
			tc::io::Path base_initial_working_dir_path;
			filesystem.getWorkingDirectory(base_initial_working_dir_path);

			// create sub filesystem
			tc::io::SubFileSystem sub_filesystem(std::make_shared<DummyFileSystem>(filesystem), subfilesystem_base_path);

			// check the sub filesystem preserved the base filesystem working directory after the constructor
			tc::io::Path base_current_working_dir_path;
			filesystem.getWorkingDirectory(base_current_working_dir_path);
			if (base_initial_working_dir_path != base_current_working_dir_path)
			{
				throw tc::Exception("SubFileSystem constructor did not preserve the base file system working directory.");
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

void io_SubFileSystem_TestClass::testGetSetWorkingDirectory()
{
	std::cout << "[tc::io::SubFileSystem] testGetSetWorkingDirectory : " << std::flush;
	try
	{
		class DummyFileSystem : public FileSystemTestUtil::DummyFileSystemBase
		{
		public:
			DummyFileSystem()
			{
			}
		};
		
		// define sub filesystem base path
		tc::io::Path subfilesystem_base_path = tc::io::Path("/home/jakcron/source/LibToolChain/testdir");

		// define base filesystem
		DummyFileSystem filesystem = DummyFileSystem();

		// test sub filesystem creation & test base working directory is maintained after SubFileSystem constructor
		try
		{
			// save a copy of the base filesystem working directory
			tc::io::Path base_initial_working_dir_path;
			filesystem.getWorkingDirectory(base_initial_working_dir_path);

			// create sub filesystem
			tc::io::SubFileSystem sub_filesystem(std::make_shared<DummyFileSystem>(filesystem), subfilesystem_base_path);

			// check the sub filesystem preserved the base filesystem working directory after get/set the working directory

			// test 1a) is the initial working directory for sub filesystem root?
			{
				tc::io::Path sub_current_working_dir_path;
				sub_filesystem.getWorkingDirectory(sub_current_working_dir_path);
				if (sub_current_working_dir_path != tc::io::Path("/"))
				{
					throw tc::Exception("SubFileSystem initial working directory was not root.");
				}
			}
			

			// test 1b) is the base filesystem working directory unchanged after using SubFileSystem::getWorkingDirectory()?
			{
				tc::io::Path base_current_working_dir_path;
				filesystem.getWorkingDirectory(base_current_working_dir_path);
				if (base_initial_working_dir_path != base_current_working_dir_path)
				{
					throw tc::Exception("SubFileSystem getWorkingDirectory did not preserve the base file system working directory.");
				}
			}

			// test 2a) can the sub filesystem change its working directory?
			tc::io::Path sub_test_path = tc::io::Path("/a/path/to/change/to");
			{
				sub_filesystem.setWorkingDirectory(sub_test_path);

				tc::io::Path sub_current_working_dir_path;
				sub_filesystem.getWorkingDirectory(sub_current_working_dir_path);

				if (sub_current_working_dir_path != sub_test_path)
				{
					throw tc::Exception("SubFileSystem setWorkingDirectory() failed to set working directory as getWorkingDirectory() returned unexpected path.");
				}
			}

			// test 2b) is the base filesystem working directory unchanged after using SubFileSystem::setWorkingDirectory()?
			{
				tc::io::Path base_current_working_dir_path;
				filesystem.getWorkingDirectory(base_current_working_dir_path);
				if (base_initial_working_dir_path != base_current_working_dir_path)
				{
					throw tc::Exception("SubFileSystem getWorkingDirectory did not preserve the base file system working directory.");
				}
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

void io_SubFileSystem_TestClass::testCreateFile()
{
	std::cout << "[tc::io::SubFileSystem] testCreateFile : " << std::flush;
	try
	{
		class DummyFileSystem : public FileSystemTestUtil::DummyFileSystemBase
		{
		public:
			DummyFileSystem(const tc::io::Path& expected_subfs_base) :
				mExpectedSubfsBasePath(expected_subfs_base)
			{
				getWorkingDirectory(mInitialWorkingDirectoryPath);
			}

			void createFile(const tc::io::Path& path)
			{
				// validate base working directory was preserved
				tc::io::Path cur_dir;
				getWorkingDirectory(cur_dir);

				if (cur_dir != mInitialWorkingDirectoryPath)
				{
					throw tc::Exception("DummyFileSystem", "Working directory was not preserved by SubFileSystem.");
				}

				// check input was correct
				if (path != mExpectedSubfsBasePath + tc::io::Path("a_dir/testfile"))
				{
					throw tc::Exception("DummyFileSystem", "file had incorrect path");
				}
			}
		private:
			tc::io::Path mInitialWorkingDirectoryPath;
			tc::io::Path mExpectedSubfsBasePath;
		};

		// define sub filesystem base path
		tc::io::Path subfilesystem_base_path = tc::io::Path("/home/jakcron/source/LibToolChain/testdir");

		// define base filesystem
		DummyFileSystem filesystem = DummyFileSystem(subfilesystem_base_path);

		// test sub filesystem creation & test translation of input to base filesystem
		try
		{
			// create sub filesystem
			tc::io::SubFileSystem sub_filesystem(std::make_shared<DummyFileSystem>(filesystem), subfilesystem_base_path);

			// attempt to create file
			sub_filesystem.createFile(tc::io::Path("/a_dir/testfile"));
			
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

void io_SubFileSystem_TestClass::testOpenFile()
{
	std::cout << "[tc::io::SubFileSystem] testOpenFile : " << std::flush;
	try
	{
		class DummyFileSystem : public FileSystemTestUtil::DummyFileSystemBase
		{
		public:
			DummyFileSystem(const tc::io::Path& expected_subfs_base) :
				mExpectedSubfsBasePath(expected_subfs_base)
			{
				getWorkingDirectory(mInitialWorkingDirectoryPath);
			}

			void openFile(const tc::io::Path& path, tc::io::FileMode mode, tc::io::FileAccess access, std::shared_ptr<tc::io::IStream>& stream)
			{
				// validate base working directory was preserved
				tc::io::Path cur_dir;
				getWorkingDirectory(cur_dir);

				if (cur_dir != mInitialWorkingDirectoryPath)
				{
					throw tc::Exception("DummyFileSystem", "Working directory was not preserved by SubFileSystem.");
				}
				
				// check input was correct
				if (mode != tc::io::FileMode::Open || access != tc::io::FileAccess::Read)
				{
					throw tc::Exception("DummyFileSystem", "file had incorrect access permissions");
				}
				if (path != mExpectedSubfsBasePath + tc::io::Path("a_dir/testfile"))
				{
					throw tc::Exception("DummyFileSystem", "file had incorrect path");
				}

				// popualate file stream pointer
				stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xdeadbeef));
			}
		private:
			tc::io::Path mInitialWorkingDirectoryPath;
			tc::io::Path mExpectedSubfsBasePath;
		};

		// define sub filesystem base path
		tc::io::Path subfilesystem_base_path = tc::io::Path("/home/jakcron/source/LibToolChain/testdir");

		// define base filesystem
		DummyFileSystem filesystem = DummyFileSystem(subfilesystem_base_path);

		// test sub filesystem creation & test translation of input/output to/from base filesystem
		try
		{
			// create sub filesystem
			tc::io::SubFileSystem sub_filesystem(std::make_shared<DummyFileSystem>(filesystem), subfilesystem_base_path);

			// attempt to open file
			std::shared_ptr<tc::io::IStream> file;
			sub_filesystem.openFile(tc::io::Path("/a_dir/testfile"), tc::io::FileMode::Open, tc::io::FileAccess::Read, file);

			// check file was opened and correct
			if (file == nullptr)
			{
				throw tc::Exception("openFile() did not populate stream pointer");
			}
			if (file->length() != 0xdeadbeef)
			{
				throw tc::Exception("openFile() did not populate stream pointer correctly");
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

void io_SubFileSystem_TestClass::testRemoveFile()
{
	std::cout << "[tc::io::SubFileSystem] testRemoveFile : " << std::flush;
	try
	{
		class DummyFileSystem : public FileSystemTestUtil::DummyFileSystemBase
		{
		public:
			DummyFileSystem(const tc::io::Path& expected_subfs_base) :
				mExpectedSubfsBasePath(expected_subfs_base)
			{
				getWorkingDirectory(mInitialWorkingDirectoryPath);
			}

			void removeFile(const tc::io::Path& path)
			{
				// validate base working directory was preserved
				tc::io::Path cur_dir;
				getWorkingDirectory(cur_dir);

				if (cur_dir != mInitialWorkingDirectoryPath)
				{
					throw tc::Exception("DummyFileSystem", "Working directory was not preserved by SubFileSystem.");
				}

				// check input was correct
				if (path != mExpectedSubfsBasePath + tc::io::Path("a_dir/testfile"))
				{
					throw tc::Exception("DummyFileSystem", "file had incorrect path");
				}
			}
		private:
			tc::io::Path mInitialWorkingDirectoryPath;
			tc::io::Path mExpectedSubfsBasePath;
		};
	
		// define sub filesystem base path
		tc::io::Path subfilesystem_base_path = tc::io::Path("/home/jakcron/source/LibToolChain/testdir");

		// define base filesystem
		DummyFileSystem filesystem = DummyFileSystem(subfilesystem_base_path);

		// test sub filesystem creation & test translation of input to base filesystem
		try
		{
			// create sub filesystem
			tc::io::SubFileSystem sub_filesystem(std::make_shared<DummyFileSystem>(filesystem), subfilesystem_base_path);

			// attempt to delete file
			sub_filesystem.removeFile(tc::io::Path("/a_dir/testfile"));

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


void io_SubFileSystem_TestClass::testCreateDirectory()
{
	std::cout << "[tc::io::SubFileSystem] testCreateDirectory : " << std::flush;
	try
	{
		class DummyFileSystem : public FileSystemTestUtil::DummyFileSystemBase
		{
		public:
			DummyFileSystem(const tc::io::Path& expected_subfs_base) :
				mExpectedSubfsBasePath(expected_subfs_base)
			{
				getWorkingDirectory(mInitialWorkingDirectoryPath);
			}

			void createDirectory(const tc::io::Path& path)
			{
				// validate base working directory was preserved
				tc::io::Path cur_dir;
				getWorkingDirectory(cur_dir);

				if (cur_dir != mInitialWorkingDirectoryPath)
				{
					throw tc::Exception("DummyFileSystem", "Working directory was not preserved by SubFileSystem.");
				}

				// check input was correct
				if (path != mExpectedSubfsBasePath + tc::io::Path("a_dir/testdir/hey"))
				{
					throw tc::Exception("DummyFileSystem", "dir had incorrect path");
				}
			}
		private:
			tc::io::Path mInitialWorkingDirectoryPath;
			tc::io::Path mExpectedSubfsBasePath;
		};
	
		// define sub filesystem base path
		tc::io::Path subfilesystem_base_path = tc::io::Path("/home/jakcron/source/LibToolChain/testdir");

		// define base filesystem
		DummyFileSystem filesystem = DummyFileSystem(subfilesystem_base_path);

		// test sub filesystem creation & test translation of input to base filesystem
		try
		{
			// create sub filesystem
			tc::io::SubFileSystem sub_filesystem(std::make_shared<DummyFileSystem>(filesystem), subfilesystem_base_path);

			// attempt to create directory
			sub_filesystem.createDirectory(tc::io::Path("/a_dir/testdir/hey"));

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

void io_SubFileSystem_TestClass::testRemoveDirectory()
{
	std::cout << "[tc::io::SubFileSystem] testRemoveDirectory : " << std::flush;
	try
	{
		class DummyFileSystem : public FileSystemTestUtil::DummyFileSystemBase
		{
		public:
			DummyFileSystem(const tc::io::Path& expected_subfs_base) :
				mExpectedSubfsBasePath(expected_subfs_base)
			{
				getWorkingDirectory(mInitialWorkingDirectoryPath);
			}

			void removeDirectory(const tc::io::Path& path)
			{
				// validate base working directory was preserved
				tc::io::Path cur_dir;
				getWorkingDirectory(cur_dir);

				if (cur_dir != mInitialWorkingDirectoryPath)
				{
					throw tc::Exception("DummyFileSystem", "Working directory was not preserved by SubFileSystem.");
				}

				// check input was correct
				if (path != mExpectedSubfsBasePath + tc::io::Path("a_dir/testdir/hey"))
				{
					throw tc::Exception("DummyFileSystem", "dir had incorrect path");
				}
			}
		private:
			tc::io::Path mInitialWorkingDirectoryPath;
			tc::io::Path mExpectedSubfsBasePath;
		};
	
		// define sub filesystem base path
		tc::io::Path subfilesystem_base_path = tc::io::Path("/home/jakcron/source/LibToolChain/testdir");

		// define base filesystem
		DummyFileSystem filesystem = DummyFileSystem(subfilesystem_base_path);

		// test sub filesystem creation & test translation of input to base filesystem
		try
		{
			// create sub filesystem
			tc::io::SubFileSystem sub_filesystem(std::make_shared<DummyFileSystem>(filesystem), subfilesystem_base_path);

			// attempt to remove directory
			sub_filesystem.removeDirectory(tc::io::Path("/a_dir/testdir/hey"));

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

void io_SubFileSystem_TestClass::testGetDirectoryListing()
{
	std::cout << "[tc::io::SubFileSystem] testGetDirectoryListing : " << std::flush;
	try
	{
		class DummyFileSystem : public FileSystemTestUtil::DummyFileSystemBase
		{
		public:
			DummyFileSystem(const tc::io::Path& expected_subfs_base) :
				mExpectedSubfsBasePath(expected_subfs_base)
			{
				getWorkingDirectory(mInitialWorkingDirectoryPath);
			}

			void getDirectoryListing(const tc::io::Path& path, tc::io::sDirectoryListing& dir_info)
			{
				// validate base working directory was preserved
				tc::io::Path cur_dir;
				getWorkingDirectory(cur_dir);

				if (cur_dir != mInitialWorkingDirectoryPath)
				{
					throw tc::Exception("DummyFileSystem", "Working directory was not preserved by SubFileSystem.");
				}

				// check input was correct
				if (path != mExpectedSubfsBasePath + tc::io::Path("a_dir/testdir/hey"))
				{
					throw tc::Exception("DummyFileSystem", "dir had incorrect path");
				}

				dir_info.abs_path = path;
				dir_info.dir_list = std::vector<std::string>({ "dir0", "dir1", "dir2" });
				dir_info.file_list = std::vector<std::string>({ "file0", "file1" });
			}
		private:
			tc::io::Path mInitialWorkingDirectoryPath;
			tc::io::Path mExpectedSubfsBasePath;
		};
	
		// define sub filesystem base path
		tc::io::Path subfilesystem_base_path = tc::io::Path("/home/jakcron/source/LibToolChain/testdir");

		// define base filesystem
		DummyFileSystem filesystem = DummyFileSystem(subfilesystem_base_path);

		// test sub filesystem creation & test translation of input to base filesystem
		try
		{
			// create sub filesystem
			tc::io::SubFileSystem sub_filesystem(std::make_shared<DummyFileSystem>(filesystem), subfilesystem_base_path);

			// save sub filesystem dir info
			tc::io::sDirectoryListing sb_dir_info;
			sub_filesystem.getDirectoryListing(tc::io::Path("/a_dir/testdir/hey"), sb_dir_info);

			// save real dir info
			tc::io::sDirectoryListing real_dir_info;
			filesystem.getDirectoryListing(subfilesystem_base_path + tc::io::Path("a_dir/testdir/hey"), real_dir_info);

			if (sb_dir_info.file_list != real_dir_info.file_list)
			{
				throw tc::Exception("DummyFileSystem", "File list was not as expected");
			}

			if (sb_dir_info.dir_list != real_dir_info.dir_list)
			{
				throw tc::Exception("DummyFileSystem", "Directory list was not as expected");
			}

			tc::io::Path fixed_sub_filesystem_path;
			for (tc::io::Path::const_iterator itr = sb_dir_info.abs_path.begin(); itr != sb_dir_info.abs_path.end(); itr++)
			{
				if (*itr == "" && itr == sb_dir_info.abs_path.begin())
				{
					continue;
				}

				fixed_sub_filesystem_path.push_back(*itr);
			}

			if ((subfilesystem_base_path + fixed_sub_filesystem_path) != real_dir_info.abs_path)
			{
				throw tc::Exception("DummyFileSystem", "Directory absolute path was not as expected");
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


void io_SubFileSystem_TestClass::testNavigateUpSubFileSystemEscape()
{
	std::cout << "[tc::io::SubFileSystem] testNavigateUpSubFileSystemEscape : " << std::flush;
	try
	{
		class DummyFileSystem : public FileSystemTestUtil::DummyFileSystemBase
		{
		public:
			DummyFileSystem() :
				mLastUsedPath(new tc::io::Path())
			{
			}

			void getDirectoryListing(const tc::io::Path& path, tc::io::sDirectoryListing& dir_info)
			{			
				dir_info.abs_path = path;
				*mLastUsedPath = path;
			}

			const tc::io::Path& getLastUsedPath()
			{
				return *mLastUsedPath;
			}
		private:
			std::shared_ptr<tc::io::Path> mLastUsedPath;
		};

		DummyFileSystem filesystem;

		// save the current directory
		tc::io::Path dummyio_curdir = tc::io::Path("/home/jakcron/source/LibToolChain");

		// define directory names
		tc::io::Path testdir_path = tc::io::Path("testdir");
		tc::io::Path sub_filesystem_relative_root = testdir_path + tc::io::Path("subfilesystem");

		// test navigating outside of sub filesystem with ".." navigation
		try
		{
			// get sub filesystem
			tc::io::SubFileSystem sub_filesystem(std::make_shared<DummyFileSystem>(filesystem), dummyio_curdir + sub_filesystem_relative_root);

			// get info about current directory
			tc::io::sDirectoryListing dir_info;
			sub_filesystem.getDirectoryListing(tc::io::Path("./../../../../../../../../../../../../../..///./././"), dir_info);
			
			if (dir_info.abs_path != tc::io::Path("/"))
			{
				throw tc::Exception("SubFileSystem directory path not as expected");
			}

			if (filesystem.getLastUsedPath() != dummyio_curdir + sub_filesystem_relative_root)
			{
				throw tc::Exception("Real directory path not as expected");
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

void io_SubFileSystem_TestClass::testOpenFileOutsideSubFileSystem()
{
	std::cout << "[tc::io::SubFileSystem] testOpenFileOutsideSubFileSystem : " << std::flush;
	try
	{
		class DummyFileSystem : public FileSystemTestUtil::DummyFileSystemBase
		{
		public:
			DummyFileSystem()
			{
			}

			void openFile(const tc::io::Path& path, tc::io::FileMode mode, tc::io::FileAccess access, std::shared_ptr<tc::io::IStream>& stream)
			{
				tc::io::Path mCurDir;
				getWorkingDirectory(mCurDir);
				if (mode != tc::io::FileMode::Open || access != tc::io::FileAccess::Read)
				{
					throw tc::Exception("DummyFileSystem", "file had incorrect access mode");
				}
				if (path == tc::io::Path("/home/jakcron/source/LibToolChain/testdir/inaccessible_file0"))
				{
					throw tc::Exception("DummyFileSystem", "escaped sub filesystem");
				}
				if (path != tc::io::Path("/home/jakcron/source/LibToolChain/testdir/subfilesystem/inaccessible_file0"))
				{
					throw tc::Exception("DummyFileSystem", "sub filesystem path was not as expected");
				}
			}
		};

		DummyFileSystem filesystem;

		// save the current directory
		tc::io::Path dummyio_curdir = tc::io::Path("/home/jakcron/source/LibToolChain");

		// define directory names
		tc::io::Path testdir_path = tc::io::Path("testdir");
		tc::io::Path sub_filesystem_relative_root = testdir_path + tc::io::Path("subfilesystem");

		// test accessing file outside of sub filesystem
		try {
			// create sub filesystem
			tc::io::SubFileSystem sub_filesystem(std::make_shared<DummyFileSystem>(filesystem), dummyio_curdir + sub_filesystem_relative_root);
			  
			// try to open the file just outside the sub filesystem
			sub_filesystem.setWorkingDirectory(tc::io::Path("/"));
			std::shared_ptr<tc::io::IStream> inaccessible_file;
			sub_filesystem.openFile(tc::io::Path("../inaccessible_file0"), tc::io::FileMode::Open, tc::io::FileAccess::Read, inaccessible_file);

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