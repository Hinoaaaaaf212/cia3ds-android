#include <tc/Exception.h>
#include <fmt/core.h>

#include <tc/io/VirtualFileSystem.h>
#include <tc/io/PathUtil.h>

#include "io_VirtualFileSystem_TestClass.h"
#include "StreamTestUtil.h"

void io_VirtualFileSystem_TestClass::runAllTests(void)
{
	fmt::print("[tc::io::VirtualFileSystem] START\n");
	test_CreateUninitializedFs_DefaultConstructor();
	test_BadFsSnapshot_CreateConstructor();
	test_CreateFs_CreateConstructor();
	test_ThrowsOnBadPermissions_OpenFile();
	test_ThrowsOnBadFileEntry_OpenFile();
	test_ThrowsOnBadFileEntry_GetDirectoryListing();
	test_ThrowsOnBadFileEntry_SetWorkingDirectory();
	test_WorksForAllValidPaths_OpenFile();
	test_WorksForAllValidPaths_GetDirectoryListing();
	test_WorksForAllValidPaths_SetWorkingDirectory();
	test_WorksForAllValidPaths_GetWorkingDirectory();
	test_DisposeWillChangeStateToUninitialized();
	fmt::print("[tc::io::VirtualFileSystem] END\n");
}

void io_VirtualFileSystem_TestClass::test_CreateUninitializedFs_DefaultConstructor()
{
	fmt::print("[tc::io::VirtualFileSystem] test_CreateUninitializedFs_DefaultConstructor : ");
	try
	{
		try 
		{
			// create filesystem
			auto filesystem = tc::io::VirtualFileSystem();

			// Test .state() returns correct
			uint64_t state_ulong = filesystem.state().to_ulong();
			uint64_t expected_state_ulong = (1 << tc::RESFLAG_NOINIT);
			if (state_ulong != expected_state_ulong)
			{
				throw tc::Exception(fmt::format(".state().to_ulong() returned 0x{:x} (expected 0x{:x})", state_ulong, expected_state_ulong));
			}

			bool state_test_ready = filesystem.state().test(tc::RESFLAG_READY);
			bool expected_state_test_ready = false;
			if (state_test_ready != expected_state_test_ready)
			{
				throw tc::Exception(fmt::format(".state().test(tc::RESFLAG_READY) returned {} (expected {})", state_test_ready, expected_state_test_ready));
			}

			bool state_test_error = filesystem.state().test(tc::RESFLAG_ERROR);
			bool expected_state_test_error = false;
			if (state_test_error != expected_state_test_error)
			{
				throw tc::Exception(fmt::format(".state().test(tc::RESFLAG_ERROR) returned {} (expected {})", state_test_error, expected_state_test_error));
			}

			bool state_test_noinit = filesystem.state().test(tc::RESFLAG_NOINIT);
			bool expected_state_test_noinit = true;
			if (state_test_noinit != expected_state_test_noinit)
			{
				throw tc::Exception(fmt::format(".state().test(tc::RESFLAG_NOINIT) returned {} (expected {})", state_test_noinit, expected_state_test_noinit));
			}

			// Test using methods that should throw ObjectDisposedException, as this is not initialized
			try
			{
				filesystem.createFile(tc::io::Path());
				throw tc::Exception(".createFile() did not throw tc::ObjectDisposedException for uninitialized VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				filesystem.removeFile(tc::io::Path());
				throw tc::Exception(".removeFile() did not throw tc::ObjectDisposedException for uninitialized VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				tc::io::Path file_path;
				std::shared_ptr<tc::io::IStream> file_stream;
				filesystem.openFile(file_path, tc::io::FileMode::Open, tc::io::FileAccess::Read, file_stream);
				throw tc::Exception(".openFile() did not throw tc::ObjectDisposedException for uninitialized VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				filesystem.createDirectory(tc::io::Path());
				throw tc::Exception(".createDirectory() did not throw tc::ObjectDisposedException for uninitialized VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				filesystem.removeDirectory(tc::io::Path());
				throw tc::Exception(".removeDirectory() did not throw tc::ObjectDisposedException for uninitialized VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				tc::io::Path path;
				filesystem.getWorkingDirectory(path);
				throw tc::Exception(".getWorkingDirectory() did not throw tc::ObjectDisposedException for uninitialized VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				filesystem.setWorkingDirectory(tc::io::Path());
				throw tc::Exception(".setWorkingDirectory() did not throw tc::ObjectDisposedException for uninitialized VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				tc::io::sDirectoryListing info;

				filesystem.getDirectoryListing(tc::io::Path(), info);
				throw tc::Exception(".setWorkingDirectory() did not throw tc::ObjectDisposedException for uninitialized VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void io_VirtualFileSystem_TestClass::test_BadFsSnapshot_CreateConstructor()
{
	fmt::print("[tc::io::VirtualFileSystem] test_BadFsSnapshot_CreateConstructor : ");
	try
	{
		try 
		{
			try
			{
				tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;

				// create filesystem
				auto filesystem = tc::io::VirtualFileSystem(snapshot);

				throw tc::Exception("Create Constructor did not throw tc::InvalidOperationException for an unpopulated fs snapshot.");
			}
			catch (tc::InvalidOperationException&) {
				// do nothing
			}

			try
			{
				tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;

				// create snapshot data
				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
				dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
				dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
				dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
				dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
				dirA_entry.dir_listing.dir_list = {};
				dirA_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
				dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
				dirB_entry.dir_listing.dir_list = {};
				dirB_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
				dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
				dirC_entry.dir_listing.dir_list = {};
				dirC_entry.dir_listing.file_list = {};
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
				fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
				fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
				fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

				// add data to snapshot
				/* // omit including dir_entry
				snapshot.dir_entries.push_back(dirRoot_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));
				*/
				snapshot.dir_entries.push_back(dirA_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirB_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirC_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.file_entries.push_back(fileA_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileB_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileC_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));

				// create filesystem
				auto filesystem = tc::io::VirtualFileSystem(snapshot);

				throw tc::Exception("Create Constructor did not throw tc::InvalidOperationException for a fs snapshot that did not have a root directory.");
			}
			catch (tc::InvalidOperationException&) {
				// do nothing
			}

			try
			{
				tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;

				// create snapshot data
				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
				dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
				dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
				dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
				dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
				dirA_entry.dir_listing.dir_list = {};
				dirA_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
				dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
				dirB_entry.dir_listing.dir_list = {};
				dirB_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
				dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
				dirC_entry.dir_listing.dir_list = {};
				dirC_entry.dir_listing.file_list = {};
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
				fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
				fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
				fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

				// add data to snapshot
				snapshot.dir_entries.push_back(dirRoot_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, -1)); // invalid index for root entry
				
				snapshot.dir_entries.push_back(dirA_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirB_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirC_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.file_entries.push_back(fileA_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileB_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileC_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));

				// create filesystem
				auto filesystem = tc::io::VirtualFileSystem(snapshot);

				throw tc::Exception("Create Constructor did not throw tc::InvalidOperationException for a fs snapshot that had an invalid root directory index.");
			}
			catch (tc::InvalidOperationException&) {
				// do nothing
			}

			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void io_VirtualFileSystem_TestClass::test_CreateFs_CreateConstructor()
{
	fmt::print("[tc::io::VirtualFileSystem] test_CreateFs_CreateConstructor : ");
	try
	{
		try 
		{
			tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;

			// create snapshot data
			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
			dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
			dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
			dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
			dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
			dirA_entry.dir_listing.dir_list = {};
			dirA_entry.dir_listing.file_list = {};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
			dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
			dirB_entry.dir_listing.dir_list = {};
			dirB_entry.dir_listing.file_list = {};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
			dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
			dirC_entry.dir_listing.dir_list = {};
			dirC_entry.dir_listing.file_list = {};
			
			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
			fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
			
			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
			fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
			fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

			// add data to snapshot
			snapshot.dir_entries.push_back(dirRoot_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirA_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirB_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirC_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.file_entries.push_back(fileA_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileB_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileC_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));

			// create filesystem
			auto filesystem = tc::io::VirtualFileSystem(snapshot);

			// Test .state() returns correct
			uint64_t state_ulong = filesystem.state().to_ulong();
			uint64_t expected_state_ulong = (1 << tc::RESFLAG_READY);
			if (state_ulong != expected_state_ulong)
			{
				throw tc::Exception(fmt::format(".state().to_ulong() returned 0x{:x} (expected 0x{:x})", state_ulong, expected_state_ulong));
			}

			bool state_test_ready = filesystem.state().test(tc::RESFLAG_READY);
			bool expected_state_test_ready = true;
			if (state_test_ready != expected_state_test_ready)
			{
				throw tc::Exception(fmt::format(".state().test(tc::RESFLAG_READY) returned {} (expected {})", state_test_ready, expected_state_test_ready));
			}

			bool state_test_error = filesystem.state().test(tc::RESFLAG_ERROR);
			bool expected_state_test_error = false;
			if (state_test_error != expected_state_test_error)
			{
				throw tc::Exception(fmt::format(".state().test(tc::RESFLAG_ERROR) returned {} (expected {})", state_test_error, expected_state_test_error));
			}

			bool state_test_noinit = filesystem.state().test(tc::RESFLAG_NOINIT);
			bool expected_state_test_noinit = false;
			if (state_test_noinit != expected_state_test_noinit)
			{
				throw tc::Exception(fmt::format(".state().test(tc::RESFLAG_NOINIT) returned {} (expected {})", state_test_noinit, expected_state_test_noinit));
			}

			// Test using methods that should throw NotImplementedException, since this is initialized
			try
			{
				filesystem.createFile(tc::io::Path());
				throw tc::Exception(".createFile() did not throw tc::NotImplementedException for initialized VirtualFileSystem");
			}
			catch (tc::NotImplementedException&) {
				// do nothing
			}

			try
			{
				filesystem.removeFile(tc::io::Path());
				throw tc::Exception(".removeFile() did not throw tc::NotImplementedException for initialized VirtualFileSystem");
			}
			catch (tc::NotImplementedException&) {
				// do nothing
			}

			try
			{
				filesystem.createDirectory(tc::io::Path());
				throw tc::Exception(".createDirectory() did not throw tc::NotImplementedException for initialized VirtualFileSystem");
			}
			catch (tc::NotImplementedException&) {
				// do nothing
			}

			try
			{
				filesystem.removeDirectory(tc::io::Path());
				throw tc::Exception(".removeDirectory() did not throw tc::NotImplementedException for initialized VirtualFileSystem");
			}
			catch (tc::NotImplementedException&) {
				// do nothing
			}

			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void io_VirtualFileSystem_TestClass::test_ThrowsOnBadPermissions_OpenFile()
{
	fmt::print("[tc::io::VirtualFileSystem] test_ThrowsOnBadPermissions_OpenFile : ");
	try
	{
		try 
		{
			tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;

			// create snapshot data
			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
			dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
			dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
			dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
			dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
			dirA_entry.dir_listing.dir_list = {};
			dirA_entry.dir_listing.file_list = {};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
			dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
			dirB_entry.dir_listing.dir_list = {};
			dirB_entry.dir_listing.file_list = {};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
			dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
			dirC_entry.dir_listing.dir_list = {};
			dirC_entry.dir_listing.file_list = {};
			
			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
			fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
			
			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
			fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
			fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

			// add data to snapshot
			snapshot.dir_entries.push_back(dirRoot_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirA_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirB_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirC_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.file_entries.push_back(fileA_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileB_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileC_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));

			// create filesystem
			auto filesystem = tc::io::VirtualFileSystem(snapshot);

			// test openFile throws exceptions for bad permissions

			/* tc::io::FileMode // only open is allowed
			CreateNew = 1,
			Create = 2,
			Open = 3, 
			OpenOrCreate = 4,
			Truncate = 5,
			Append = 6
			*/

			/* tc::io::FileAccess  // only read is allowed
			Read = 1,
			Write = 2,
			ReadWrite = Read|Write, // (1|2 == 3)
			*/

			// CreateNew,Read
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::CreateNew, tc::io::FileAccess::Read, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "CreateNew", "Read"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// CreateNew,Write
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::CreateNew, tc::io::FileAccess::Write, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "CreateNew", "Write"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// CreateNew,ReadWrite
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::CreateNew, tc::io::FileAccess::ReadWrite, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "CreateNew", "ReadWrite"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// Create,Read
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::Create, tc::io::FileAccess::Read, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "Create", "Read"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// Create,Write
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::Create, tc::io::FileAccess::Write, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "Create", "Write"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// Create,ReadWrite
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::Create, tc::io::FileAccess::ReadWrite, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "Create", "ReadWrite"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// Open,Read (this is the only supported combination)
			/*
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::Open, tc::io::FileAccess::Read, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "Open", "Read"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}
			*/

			// Open,Write
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::Open, tc::io::FileAccess::Write, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "Open", "Write"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// Open,ReadWrite
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::Open, tc::io::FileAccess::ReadWrite, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "Open", "ReadWrite"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// OpenOrCreate,Read
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::OpenOrCreate, tc::io::FileAccess::Read, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "OpenOrCreate", "Read"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// OpenOrCreate,Write
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::OpenOrCreate, tc::io::FileAccess::Write, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "OpenOrCreate", "Write"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// OpenOrCreate,ReadWrite
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::OpenOrCreate, tc::io::FileAccess::ReadWrite, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "OpenOrCreate", "ReadWrite"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// Truncate,Read
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::Truncate, tc::io::FileAccess::Read, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "Truncate", "Read"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// Truncate,Write
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::Truncate, tc::io::FileAccess::Write, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "Truncate", "Write"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// Truncate,ReadWrite
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::Truncate, tc::io::FileAccess::ReadWrite, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "Truncate", "ReadWrite"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// Append,Read
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::Append, tc::io::FileAccess::Read, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "Append", "Read"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// Append,Write
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::Append, tc::io::FileAccess::Write, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "Append", "Write"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			// Append,ReadWrite
			try 
			{
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;

			
				filesystem.openFile(path, tc::io::FileMode::Append, tc::io::FileAccess::ReadWrite, stream);
				
				throw tc::Exception(fmt::format(".openFile() did not throw tc::NotSupportedException where unsupported mode ({:s}) and access ({:s}) were used.", "Append", "ReadWrite"));
			}
			catch (const tc::NotSupportedException&)
			{
				// do nothing
			}

			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void io_VirtualFileSystem_TestClass::test_ThrowsOnBadFileEntry_OpenFile()
{
	fmt::print("[tc::io::VirtualFileSystem] test_ThrowsOnBadFileEntry_OpenFile : ");
	try
	{
		try 
		{
			try 
			{
				tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
				// create snapshot data
			
				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
				dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
				dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
				dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
				dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
				dirA_entry.dir_listing.dir_list = {};
				dirA_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
				dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
				dirB_entry.dir_listing.dir_list = {};
				dirB_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
				dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
				dirC_entry.dir_listing.dir_list = {};
				dirC_entry.dir_listing.file_list = {};
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
				fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
				fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
				fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

				// add data to snapshot
				snapshot.dir_entries.push_back(dirRoot_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirA_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirB_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirC_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.file_entries.push_back(fileA_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

				// error: file entry not created
				//snapshot.file_entries.push_back(fileB_entry);
				//snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileC_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));
			
				// create filesystem
				auto filesystem = tc::io::VirtualFileSystem(snapshot);

				// open file
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;
				filesystem.openFile(path, tc::io::FileMode::Open, tc::io::FileAccess::Read, stream);
				
				throw tc::Exception(".openFile() did not throw tc::io::FileNotFoundException where file entry did not exist");
			}
			catch (tc::io::FileNotFoundException&)
			{
				// do nothing
			}

			try 
			{
				tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
				// create snapshot data
			
				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
				dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
				dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
				dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
				dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
				dirA_entry.dir_listing.dir_list = {};
				dirA_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
				dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
				dirB_entry.dir_listing.dir_list = {};
				dirB_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
				dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
				dirC_entry.dir_listing.dir_list = {};
				dirC_entry.dir_listing.file_list = {};
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
				fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
				fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
				fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

				// add data to snapshot
				snapshot.dir_entries.push_back(dirRoot_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirA_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirB_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirC_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.file_entries.push_back(fileA_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

				// error: file entry not created but not mapped to a path
				snapshot.file_entries.push_back(fileB_entry);
				//snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileC_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));
			
				// create filesystem
				auto filesystem = tc::io::VirtualFileSystem(snapshot);

				// open file
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;
				filesystem.openFile(path, tc::io::FileMode::Open, tc::io::FileAccess::Read, stream);
				
				throw tc::Exception(".openFile() did not throw tc::io::FileNotFoundException where file entry did exist, but not mapped to a path");
			}
			catch (tc::io::FileNotFoundException&)
			{
				// do nothing
			}
			
			try 
			{
				tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
				// create snapshot data
			
				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
				dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
				dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
				dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
				dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
				dirA_entry.dir_listing.dir_list = {};
				dirA_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
				dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
				dirB_entry.dir_listing.dir_list = {};
				dirB_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
				dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
				dirC_entry.dir_listing.dir_list = {};
				dirC_entry.dir_listing.file_list = {};
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
				fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
				fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
				fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

				// add data to snapshot
				snapshot.dir_entries.push_back(dirRoot_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirA_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirB_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirC_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.file_entries.push_back(fileA_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

				// error: file entry created, but not mapped correctly (index == -1)
				snapshot.file_entries.push_back(fileB_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), -1));

				snapshot.file_entries.push_back(fileC_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));
			
				// create filesystem
				auto filesystem = tc::io::VirtualFileSystem(snapshot);

				// open file
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;
				filesystem.openFile(path, tc::io::FileMode::Open, tc::io::FileAccess::Read, stream);
				
				throw tc::Exception(".openFile() did not throw tc::io::FileNotFoundException where file entry did exist, but had a bad mapping");
			}
			catch (tc::io::FileNotFoundException&)
			{
				// do nothing
			}

			try 
			{
				tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
				// create snapshot data
			
				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
				dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
				dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
				dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
				dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
				dirA_entry.dir_listing.dir_list = {};
				dirA_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
				dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
				dirB_entry.dir_listing.dir_list = {};
				dirB_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
				dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
				dirC_entry.dir_listing.dir_list = {};
				dirC_entry.dir_listing.file_list = {};
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
				fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
				fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
				fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

				// add data to snapshot
				snapshot.dir_entries.push_back(dirRoot_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirA_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirB_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirC_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.file_entries.push_back(fileA_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

				// error: file entry created and mapped, but stream is null
				fileB_entry.stream.reset(); // <- null the pointer here
				snapshot.file_entries.push_back(fileB_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileC_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));
			
				// create filesystem
				auto filesystem = tc::io::VirtualFileSystem(snapshot);

				// open file
				tc::io::Path path = tc::io::Path("/fileB");
				std::shared_ptr<tc::io::IStream> stream;
				filesystem.openFile(path, tc::io::FileMode::Open, tc::io::FileAccess::Read, stream);
				
				throw tc::Exception(".openFile() did not throw tc::io::FileNotFoundException where file entry did exist, but stream was null");
			}
			catch (tc::io::FileNotFoundException&)
			{
				// do nothing
			}

			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void io_VirtualFileSystem_TestClass::test_ThrowsOnBadFileEntry_GetDirectoryListing()
{
	fmt::print("[tc::io::VirtualFileSystem] test_ThrowsOnBadFileEntry_GetDirectoryListing : ");
	try
	{
		try 
		{
			try 
			{
				tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
				// create snapshot data
			
				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
				dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
				dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
				dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
				dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
				dirA_entry.dir_listing.dir_list = {};
				dirA_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
				dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
				dirB_entry.dir_listing.dir_list = {};
				dirB_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
				dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
				dirC_entry.dir_listing.dir_list = {};
				dirC_entry.dir_listing.file_list = {};
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
				fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
				fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
				fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

				// add data to snapshot
				snapshot.dir_entries.push_back(dirRoot_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirA_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirB_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				// error: dir entry not created
				//snapshot.dir_entries.push_back(dirC_entry);
				//snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.file_entries.push_back(fileA_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileB_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileC_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));
			
				// create filesystem
				auto filesystem = tc::io::VirtualFileSystem(snapshot);

				// get directory listing
				tc::io::Path path = tc::io::Path("/dirC");
				tc::io::sDirectoryListing info;
				filesystem.getDirectoryListing(path, info);
				
				throw tc::Exception(".getDirectoryListing() did not throw tc::io::DirectoryNotFoundException where dir entry did not exist");
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				// do nothing
			}

			try 
			{
				tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
				// create snapshot data
			
				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
				dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
				dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
				dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
				dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
				dirA_entry.dir_listing.dir_list = {};
				dirA_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
				dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
				dirB_entry.dir_listing.dir_list = {};
				dirB_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
				dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
				dirC_entry.dir_listing.dir_list = {};
				dirC_entry.dir_listing.file_list = {};
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
				fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
				fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
				fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

				// add data to snapshot
				snapshot.dir_entries.push_back(dirRoot_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirA_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirB_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				// error: dir entry created, but not mapped to a path
				snapshot.dir_entries.push_back(dirC_entry);
				//snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.file_entries.push_back(fileA_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileB_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileC_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));
			
				// create filesystem
				auto filesystem = tc::io::VirtualFileSystem(snapshot);

				// get directory listing
				tc::io::Path path = tc::io::Path("/dirC");
				tc::io::sDirectoryListing info;
				filesystem.getDirectoryListing(path, info);
				
				throw tc::Exception(".getDirectoryListing() did not throw tc::io::DirectoryNotFoundException where dir entry did exist, but not mapped to a path");
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				// do nothing
			}
			
			try 
			{
				tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
				// create snapshot data
			
				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
				dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
				dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
				dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
				dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
				dirA_entry.dir_listing.dir_list = {};
				dirA_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
				dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
				dirB_entry.dir_listing.dir_list = {};
				dirB_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
				dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
				dirC_entry.dir_listing.dir_list = {};
				dirC_entry.dir_listing.file_list = {};
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
				fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
				fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
				fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

				// add data to snapshot
				snapshot.dir_entries.push_back(dirRoot_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirA_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirB_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				// error: dir entry created, but not mapped correctly (index == -1)
				snapshot.dir_entries.push_back(dirC_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, -1));

				snapshot.file_entries.push_back(fileA_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileB_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileC_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));
			
				// create filesystem
				auto filesystem = tc::io::VirtualFileSystem(snapshot);

				// get directory listing
				tc::io::Path path = tc::io::Path("/dirC");
				tc::io::sDirectoryListing info;
				filesystem.getDirectoryListing(path, info);
				
				throw tc::Exception(".getDirectoryListing() did not throw tc::io::DirectoryNotFoundException where dir entry did exist, but not mapped correctly.");
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				// do nothing
			}
			
			
			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void io_VirtualFileSystem_TestClass::test_ThrowsOnBadFileEntry_SetWorkingDirectory()
{
	fmt::print("[tc::io::VirtualFileSystem] test_ThrowsOnBadFileEntry_SetWorkingDirectory : ");
	try
	{
		try 
		{
			try 
			{
				tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
				// create snapshot data
			
				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
				dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
				dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
				dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
				dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
				dirA_entry.dir_listing.dir_list = {};
				dirA_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
				dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
				dirB_entry.dir_listing.dir_list = {};
				dirB_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
				dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
				dirC_entry.dir_listing.dir_list = {};
				dirC_entry.dir_listing.file_list = {};
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
				fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
				fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
				fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

				// add data to snapshot
				snapshot.dir_entries.push_back(dirRoot_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirA_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirB_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				// error: dir entry not created
				//snapshot.dir_entries.push_back(dirC_entry);
				//snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.file_entries.push_back(fileA_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileB_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileC_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));
			
				// create filesystem
				auto filesystem = tc::io::VirtualFileSystem(snapshot);

				// set working directory
				tc::io::Path path = tc::io::Path("/dirC");
				filesystem.setWorkingDirectory(path);
				
				throw tc::Exception(".setWorkingDirectory() did not throw tc::io::DirectoryNotFoundException where dir entry did not exist");
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				// do nothing
			}

			try 
			{
				tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
				// create snapshot data
			
				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
				dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
				dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
				dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
				dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
				dirA_entry.dir_listing.dir_list = {};
				dirA_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
				dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
				dirB_entry.dir_listing.dir_list = {};
				dirB_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
				dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
				dirC_entry.dir_listing.dir_list = {};
				dirC_entry.dir_listing.file_list = {};
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
				fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
				fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
				fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

				// add data to snapshot
				snapshot.dir_entries.push_back(dirRoot_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirA_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirB_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				// error: dir entry created, but not mapped to a path
				snapshot.dir_entries.push_back(dirC_entry);
				//snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.file_entries.push_back(fileA_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileB_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileC_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));
			
				// create filesystem
				auto filesystem = tc::io::VirtualFileSystem(snapshot);

				// set working directory
				tc::io::Path path = tc::io::Path("/dirC");
				filesystem.setWorkingDirectory(path);
				
				throw tc::Exception(".setWorkingDirectory() did not throw tc::io::DirectoryNotFoundException where dir entry did exist, but not mapped to a path");
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				// do nothing
			}
			
			try 
			{
				tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
				// create snapshot data
			
				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
				dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
				dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
				dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
				dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
				dirA_entry.dir_listing.dir_list = {};
				dirA_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
				dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
				dirB_entry.dir_listing.dir_list = {};
				dirB_entry.dir_listing.file_list = {};

				tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
				dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
				dirC_entry.dir_listing.dir_list = {};
				dirC_entry.dir_listing.file_list = {};
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
				fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
				
				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
				fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

				tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
				fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

				// add data to snapshot
				snapshot.dir_entries.push_back(dirRoot_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirA_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				snapshot.dir_entries.push_back(dirB_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

				// error: dir entry created, but not mapped correctly (index == -1)
				snapshot.dir_entries.push_back(dirC_entry);
				snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, -1));

				snapshot.file_entries.push_back(fileA_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileB_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

				snapshot.file_entries.push_back(fileC_entry);
				snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));
			
				// create filesystem
				auto filesystem = tc::io::VirtualFileSystem(snapshot);

				// set working directory
				tc::io::Path path = tc::io::Path("/dirC");
				filesystem.setWorkingDirectory(path);
				
				throw tc::Exception(".setWorkingDirectory() did not throw tc::io::DirectoryNotFoundException where dir entry did exist, but not mapped correctly.");
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				// do nothing
			}
			
			
			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void io_VirtualFileSystem_TestClass::test_WorksForAllValidPaths_OpenFile()
{
	fmt::print("[tc::io::VirtualFileSystem] test_WorksForAllValidPaths_OpenFile : ");
	try
	{
		try 
		{
			tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
			// create snapshot data
		
			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
			dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
			dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
			dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
			dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
			dirA_entry.dir_listing.dir_list = {};
			dirA_entry.dir_listing.file_list = {};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
			dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
			dirB_entry.dir_listing.dir_list = {};
			dirB_entry.dir_listing.file_list = {"fileD"};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
			dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
			dirC_entry.dir_listing.dir_list = {};
			dirC_entry.dir_listing.file_list = {};
			
			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
			fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
			
			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
			fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
			fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileD_entry;
			fileD_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xD, true, false, true, false, false));

			// add data to snapshot
			snapshot.dir_entries.push_back(dirRoot_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirA_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirB_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirC_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.file_entries.push_back(fileA_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileB_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileC_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileD_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path + tc::io::Path("fileD"), snapshot.file_entries.size()-1));
		
			// create filesystem
			auto filesystem = tc::io::VirtualFileSystem(snapshot);


			// test that openFile works for all valid paths
			std::string literal_path;
			
			literal_path = "/fileA";
			try 
			{
				// open file
				std::shared_ptr<tc::io::IStream> stream;
				filesystem.openFile(tc::io::Path(literal_path), tc::io::FileMode::Open, tc::io::FileAccess::Read, stream);

				if (stream == nullptr)
				{
					throw tc::Exception(fmt::format(".openFile() returned a null stream for file \"{:s}\".", literal_path));
				}

				if (stream->canRead() == false)
				{
					throw tc::Exception(fmt::format(".openFile() returned a stream where canRead() was false for file \"{:s}\".", literal_path));
				}

				if (stream->canWrite() == true)
				{
					throw tc::Exception(fmt::format(".openFile() returned a stream where canWrite() was true for file \"{:s}\".", literal_path));
				}

				int64_t expected_length = 0xA;
				int64_t actual_length = stream->length();
				if (actual_length != expected_length)
				{
					throw tc::Exception(fmt::format(".openFile() returned a stream where length was 0x{:x} (expected 0x{:x}) for file \"{:s}\".", actual_length, expected_length, literal_path));
				}
			}
			catch (tc::io::FileNotFoundException&)
			{
				throw tc::Exception(fmt::format(".openFile() threw tc::io::FileNotFoundException where file (\"{:s}\") did exist.", literal_path));
			}

			literal_path = "/fileB";
			try 
			{
				// open file
				std::shared_ptr<tc::io::IStream> stream;
				filesystem.openFile(tc::io::Path(literal_path), tc::io::FileMode::Open, tc::io::FileAccess::Read, stream);

				if (stream == nullptr)
				{
					throw tc::Exception(fmt::format(".openFile() returned a null stream for file \"{:s}\".", literal_path));
				}

				if (stream->canRead() == false)
				{
					throw tc::Exception(fmt::format(".openFile() returned a stream where canRead() was false for file \"{:s}\".", literal_path));
				}

				if (stream->canWrite() == true)
				{
					throw tc::Exception(fmt::format(".openFile() returned a stream where canWrite() was true for file \"{:s}\".", literal_path));
				}

				int64_t expected_length = 0xB;
				int64_t actual_length = stream->length();
				if (actual_length != expected_length)
				{
					throw tc::Exception(fmt::format(".openFile() returned a stream where length was 0x{:x} (expected 0x{:x}) for file \"{:s}\".", actual_length, expected_length, literal_path));
				}
			}
			catch (tc::io::FileNotFoundException&)
			{
				throw tc::Exception(fmt::format(".openFile() threw tc::io::FileNotFoundException where file (\"{:s}\") did exist.", literal_path));
			}

			literal_path = "/fileC";
			try 
			{
				// open file
				std::shared_ptr<tc::io::IStream> stream;
				filesystem.openFile(tc::io::Path(literal_path), tc::io::FileMode::Open, tc::io::FileAccess::Read, stream);

				if (stream == nullptr)
				{
					throw tc::Exception(fmt::format(".openFile() returned a null stream for file \"{:s}\".", literal_path));
				}

				if (stream->canRead() == false)
				{
					throw tc::Exception(fmt::format(".openFile() returned a stream where canRead() was false for file \"{:s}\".", literal_path));
				}

				if (stream->canWrite() == true)
				{
					throw tc::Exception(fmt::format(".openFile() returned a stream where canWrite() was true for file \"{:s}\".", literal_path));
				}

				int64_t expected_length = 0xC;
				int64_t actual_length = stream->length();
				if (actual_length != expected_length)
				{
					throw tc::Exception(fmt::format(".openFile() returned a stream where length was 0x{:x} (expected 0x{:x}) for file \"{:s}\".", actual_length, expected_length, literal_path));
				}
			}
			catch (tc::io::FileNotFoundException&)
			{
				throw tc::Exception(fmt::format(".openFile() threw tc::io::FileNotFoundException where file (\"{:s}\") did exist.", literal_path));
			}

			literal_path = "/dirB/fileD";
			try 
			{
				// open file
				std::shared_ptr<tc::io::IStream> stream;
				filesystem.openFile(tc::io::Path(literal_path), tc::io::FileMode::Open, tc::io::FileAccess::Read, stream);

				if (stream == nullptr)
				{
					throw tc::Exception(fmt::format(".openFile() returned a null stream for file \"{:s}\".", literal_path));
				}

				if (stream->canRead() == false)
				{
					throw tc::Exception(fmt::format(".openFile() returned a stream where canRead() was false for file \"{:s}\".", literal_path));
				}

				if (stream->canWrite() == true)
				{
					throw tc::Exception(fmt::format(".openFile() returned a stream where canWrite() was true for file \"{:s}\".", literal_path));
				}

				int64_t expected_length = 0xD;
				int64_t actual_length = stream->length();
				if (actual_length != expected_length)
				{
					throw tc::Exception(fmt::format(".openFile() returned a stream where length was 0x{:x} (expected 0x{:x}) for file \"{:s}\".", actual_length, expected_length, literal_path));
				}
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				throw tc::Exception(fmt::format(".openFile() threw tc::io::DirectoryNotFoundException where file (\"{:s}\") did exist.", literal_path));
			}
			
			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void io_VirtualFileSystem_TestClass::test_WorksForAllValidPaths_GetDirectoryListing()
{
	fmt::print("[tc::io::VirtualFileSystem] test_WorksForAllValidPaths_GetDirectoryListing : ");
	try
	{
		try 
		{
			tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
			// create snapshot data
		
			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
			dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
			dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
			dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
			dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
			dirA_entry.dir_listing.dir_list = {};
			dirA_entry.dir_listing.file_list = {};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
			dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
			dirB_entry.dir_listing.dir_list = {};
			dirB_entry.dir_listing.file_list = {"fileD"};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
			dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
			dirC_entry.dir_listing.dir_list = {};
			dirC_entry.dir_listing.file_list = {};
			
			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
			fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
			
			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
			fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
			fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileD_entry;
			fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xD, true, false, true, false, false));

			// add data to snapshot
			snapshot.dir_entries.push_back(dirRoot_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirA_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirB_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirC_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.file_entries.push_back(fileA_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileB_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileC_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileD_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path + tc::io::Path("fileD"), snapshot.file_entries.size()-1));
		
			// create filesystem
			auto filesystem = tc::io::VirtualFileSystem(snapshot);


			// test that getDirectoryListing works for all valid paths
			std::string literal_path;
			
			literal_path = "/";
			try 
			{
				// get directory listing
				tc::io::sDirectoryListing info;
				filesystem.getDirectoryListing(tc::io::Path(literal_path), info);

				if (info.abs_path != dirRoot_entry.dir_listing.abs_path)
				{
					throw tc::Exception(".getDirectoryListing() did not return the correct DirectoryListing for directory \"{:s}\" (wrong absolute path).", literal_path);
				}

				if (info.dir_list != dirRoot_entry.dir_listing.dir_list)
				{
					throw tc::Exception(".getDirectoryListing() did not return the correct DirectoryListing for directory \"{:s}\" (wrong dir list).", literal_path);
				}

				if (info.file_list != dirRoot_entry.dir_listing.file_list)
				{
					throw tc::Exception(".getDirectoryListing() did not return the correct DirectoryListing for directory \"{:s}\" (wrong dir list).", literal_path);
				}
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				throw tc::Exception(fmt::format(".getDirectoryListing() threw tc::io::DirectoryNotFoundException where directory (\"{:s}\") did exist.", literal_path));
			}

			literal_path = "/dirA/";
			try 
			{
				// get directory listing
				tc::io::sDirectoryListing info;
				filesystem.getDirectoryListing(tc::io::Path(literal_path), info);

				if (info.abs_path != dirA_entry.dir_listing.abs_path)
				{
					throw tc::Exception(fmt::format(".getDirectoryListing() did not return the correct DirectoryListing for directory \"{:s}\" (wrong absolute path).", literal_path));
				}

				if (info.dir_list != dirA_entry.dir_listing.dir_list)
				{
					throw tc::Exception(fmt::format(".getDirectoryListing() did not return the correct DirectoryListing for directory \"{:s}\" (wrong dir list).", literal_path));
				}

				if (info.file_list != dirA_entry.dir_listing.file_list)
				{
					throw tc::Exception(fmt::format(".getDirectoryListing() did not return the correct DirectoryListing for directory \"{:s}\" (wrong dir list).", literal_path));
				}
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				throw tc::Exception(fmt::format(".getDirectoryListing() threw tc::io::DirectoryNotFoundException where directory (\"{:s}\") did exist.", literal_path));
			}

			literal_path = "/dirB/";
			try 
			{
				// get directory listing
				tc::io::sDirectoryListing info;
				filesystem.getDirectoryListing(tc::io::Path(literal_path), info);

				if (info.abs_path != dirB_entry.dir_listing.abs_path)
				{
					throw tc::Exception(fmt::format(".getDirectoryListing() did not return the correct DirectoryListing for directory \"{:s}\" (wrong absolute path).", literal_path));
				}

				if (info.dir_list != dirB_entry.dir_listing.dir_list)
				{
					throw tc::Exception(fmt::format(".getDirectoryListing() did not return the correct DirectoryListing for directory \"{:s}\" (wrong dir list).", literal_path));
				}

				if (info.file_list != dirB_entry.dir_listing.file_list)
				{
					throw tc::Exception(fmt::format(".getDirectoryListing() did not return the correct DirectoryListing for directory \"{:s}\" (wrong dir list).", literal_path));
				}
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				throw tc::Exception(fmt::format(".getDirectoryListing() threw tc::io::DirectoryNotFoundException where directory (\"{:s}\") did exist.", literal_path));
			}

			literal_path = "/dirC/";
			try 
			{
				// get directory listing
				tc::io::sDirectoryListing info;
				filesystem.getDirectoryListing(tc::io::Path(literal_path), info);

				if (info.abs_path != dirC_entry.dir_listing.abs_path)
				{
					throw tc::Exception(fmt::format(".getDirectoryListing() did not return the correct DirectoryListing for directory \"{:s}\" (wrong absolute path).", literal_path));
				}

				if (info.dir_list != dirC_entry.dir_listing.dir_list)
				{
					throw tc::Exception(fmt::format(".getDirectoryListing() did not return the correct DirectoryListing for directory \"{:s}\" (wrong dir list).", literal_path));
				}

				if (info.file_list != dirC_entry.dir_listing.file_list)
				{
					throw tc::Exception(fmt::format(".getDirectoryListing() did not return the correct DirectoryListing for directory \"{:s}\" (wrong dir list).", literal_path));
				}
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				throw tc::Exception(fmt::format(".getDirectoryListing() threw tc::io::DirectoryNotFoundException where directory (\"{:s}\") did exist.", literal_path));
			}
			
			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void io_VirtualFileSystem_TestClass::test_WorksForAllValidPaths_SetWorkingDirectory()
{
	fmt::print("[tc::io::VirtualFileSystem] test_WorksForAllValidPaths_SetWorkingDirectory : ");
	try
	{
		try 
		{
			tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
			// create snapshot data
		
			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
			dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
			dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
			dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
			dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
			dirA_entry.dir_listing.dir_list = {};
			dirA_entry.dir_listing.file_list = {};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
			dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
			dirB_entry.dir_listing.dir_list = {};
			dirB_entry.dir_listing.file_list = {};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
			dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
			dirC_entry.dir_listing.dir_list = {};
			dirC_entry.dir_listing.file_list = {};
			
			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
			fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
			
			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
			fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
			fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

			// add data to snapshot
			snapshot.dir_entries.push_back(dirRoot_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirA_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirB_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirC_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.file_entries.push_back(fileA_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileB_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileC_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));
		
			// create filesystem
			auto filesystem = tc::io::VirtualFileSystem(snapshot);


			// test that setWorkingDirectory works for all valid paths
			std::string literal_path;
			
			literal_path = "/";
			try 
			{
				// set working directory
				filesystem.setWorkingDirectory(tc::io::Path(literal_path));
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				throw tc::Exception(fmt::format(".setWorkingDirectory() threw tc::io::DirectoryNotFoundException where directory (\"{:s}\") did exist.", literal_path));
			}

			literal_path = "/dirA/";
			try 
			{
				// set working directory
				filesystem.setWorkingDirectory(tc::io::Path(literal_path));
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				throw tc::Exception(fmt::format(".setWorkingDirectory() threw tc::io::DirectoryNotFoundException where directory (\"{:s}\") did exist.", literal_path));
			}

			literal_path = "/dirB/";
			try 
			{
				// set working directory
				filesystem.setWorkingDirectory(tc::io::Path(literal_path));
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				throw tc::Exception(fmt::format(".setWorkingDirectory() threw tc::io::DirectoryNotFoundException where directory (\"{:s}\") did exist.", literal_path));
			}

			literal_path = "/dirC/";
			try 
			{
				// set working directory
				filesystem.setWorkingDirectory(tc::io::Path(literal_path));
			}
			catch (tc::io::DirectoryNotFoundException&)
			{
				throw tc::Exception(fmt::format(".setWorkingDirectory() threw tc::io::DirectoryNotFoundException where directory (\"{:s}\") did exist.", literal_path));
			}
			
			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void io_VirtualFileSystem_TestClass::test_WorksForAllValidPaths_GetWorkingDirectory()
{
	fmt::print("[tc::io::VirtualFileSystem] test_WorksForAllValidPaths_GetWorkingDirectory : ");
	try
	{
		try 
		{
			tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
			// create snapshot data
		
			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
			dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
			dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
			dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
			dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
			dirA_entry.dir_listing.dir_list = {};
			dirA_entry.dir_listing.file_list = {};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
			dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
			dirB_entry.dir_listing.dir_list = {};
			dirB_entry.dir_listing.file_list = {};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
			dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
			dirC_entry.dir_listing.dir_list = {};
			dirC_entry.dir_listing.file_list = {};
			
			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
			fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
			
			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
			fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
			fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

			// add data to snapshot
			snapshot.dir_entries.push_back(dirRoot_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirA_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirB_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirC_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.file_entries.push_back(fileA_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileB_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileC_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));
		
			// create filesystem
			auto filesystem = tc::io::VirtualFileSystem(snapshot);
		

			// test that working directory at point of filesystem creation is root
			{
				tc::io::Path expected_working_directory = tc::io::Path("/");
				tc::io::Path working_directory;

				filesystem.getWorkingDirectory(working_directory);

				if (expected_working_directory != working_directory)
				{
					std::string expected_working_directory_string;
					std::string working_directory_string;
					tc::io::PathUtil::pathToUnixUTF8(expected_working_directory, expected_working_directory_string);
					tc::io::PathUtil::pathToUnixUTF8(working_directory, working_directory_string);
					throw tc::Exception(fmt::format("getWorkingDirectory() returned a path that was not root after the filesystem was created. (returned: \"{:s}\", expected: \"{:s}\")", working_directory_string, expected_working_directory_string));
				}
			}


			// test that setWorkingDirectory works for all valid paths
			std::string literal_path;
			
			literal_path = "/"; 
			{
				tc::io::Path expected_working_directory = tc::io::Path(literal_path);
				tc::io::Path working_directory;

				filesystem.setWorkingDirectory(expected_working_directory);
				filesystem.getWorkingDirectory(working_directory);

				if (expected_working_directory != working_directory)
				{
					std::string expected_working_directory_string;
					std::string working_directory_string;
					tc::io::PathUtil::pathToUnixUTF8(expected_working_directory, expected_working_directory_string);
					tc::io::PathUtil::pathToUnixUTF8(working_directory, working_directory_string);
					throw tc::Exception(fmt::format("getWorkingDirectory() did not return the expected path. (returned: \"{:s}\", expected: \"{:s}\")", working_directory_string, expected_working_directory_string));
				}
			}
			

			literal_path = "/dirA/";
			{
				tc::io::Path expected_working_directory = tc::io::Path(literal_path);
				tc::io::Path working_directory;

				filesystem.setWorkingDirectory(expected_working_directory);
				filesystem.getWorkingDirectory(working_directory);

				if (expected_working_directory != working_directory)
				{
					std::string expected_working_directory_string;
					std::string working_directory_string;
					tc::io::PathUtil::pathToUnixUTF8(expected_working_directory, expected_working_directory_string);
					tc::io::PathUtil::pathToUnixUTF8(working_directory, working_directory_string);
					throw tc::Exception(fmt::format("getWorkingDirectory() did not return the expected path. (returned: \"{:s}\", expected: \"{:s}\")", working_directory_string, expected_working_directory_string));
				}
			}

			literal_path = "/dirB/";
			{
				tc::io::Path expected_working_directory = tc::io::Path(literal_path);
				tc::io::Path working_directory;

				filesystem.setWorkingDirectory(expected_working_directory);
				filesystem.getWorkingDirectory(working_directory);

				if (expected_working_directory != working_directory)
				{
					std::string expected_working_directory_string;
					std::string working_directory_string;
					tc::io::PathUtil::pathToUnixUTF8(expected_working_directory, expected_working_directory_string);
					tc::io::PathUtil::pathToUnixUTF8(working_directory, working_directory_string);
					throw tc::Exception(fmt::format("getWorkingDirectory() did not return the expected path. (returned: \"{:s}\", expected: \"{:s}\")", working_directory_string, expected_working_directory_string));
				}
			}

			literal_path = "/dirC/";
			{
				tc::io::Path expected_working_directory = tc::io::Path(literal_path);
				tc::io::Path working_directory;

				filesystem.setWorkingDirectory(expected_working_directory);
				filesystem.getWorkingDirectory(working_directory);

				if (expected_working_directory != working_directory)
				{
					std::string expected_working_directory_string;
					std::string working_directory_string;
					tc::io::PathUtil::pathToUnixUTF8(expected_working_directory, expected_working_directory_string);
					tc::io::PathUtil::pathToUnixUTF8(working_directory, working_directory_string);
					throw tc::Exception(fmt::format("getWorkingDirectory() did not return the expected path. (returned: \"{:s}\", expected: \"{:s}\")", working_directory_string, expected_working_directory_string));
				}
			}
			
			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void io_VirtualFileSystem_TestClass::test_DisposeWillChangeStateToUninitialized()
{
	fmt::print("[tc::io::VirtualFileSystem] test_DisposeWillChangeStateToUninitialized : ");
	try
	{
		try 
		{
			tc::io::VirtualFileSystem::FileSystemSnapshot snapshot;
				
			// create snapshot data
		
			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirRoot_entry;
			dirRoot_entry.dir_listing.abs_path = tc::io::Path("/");
			dirRoot_entry.dir_listing.dir_list = {"dirA", "dirB", "dirC"};
			dirRoot_entry.dir_listing.file_list = {"fileA", "fileB", "fileC"};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirA_entry;
			dirA_entry.dir_listing.abs_path = tc::io::Path("/dirA/");
			dirA_entry.dir_listing.dir_list = {};
			dirA_entry.dir_listing.file_list = {};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirB_entry;
			dirB_entry.dir_listing.abs_path = tc::io::Path("/dirB/");
			dirB_entry.dir_listing.dir_list = {};
			dirB_entry.dir_listing.file_list = {};

			tc::io::VirtualFileSystem::FileSystemSnapshot::DirEntry dirC_entry;
			dirC_entry.dir_listing.abs_path = tc::io::Path("/dirC/");
			dirC_entry.dir_listing.dir_list = {};
			dirC_entry.dir_listing.file_list = {};
			
			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileA_entry;
			fileA_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xA, true, false, true, false, false));
			
			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileB_entry;
			fileB_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xB, true, false, true, false, false));

			tc::io::VirtualFileSystem::FileSystemSnapshot::FileEntry fileC_entry;
			fileC_entry.stream = std::make_shared<StreamTestUtil::DummyStreamBase>(StreamTestUtil::DummyStreamBase(0xC, true, false, true, false, false));

			// add data to snapshot
			snapshot.dir_entries.push_back(dirRoot_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirA_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirA_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirB_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirB_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.dir_entries.push_back(dirC_entry);
			snapshot.dir_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirC_entry.dir_listing.abs_path, snapshot.dir_entries.size()-1));

			snapshot.file_entries.push_back(fileA_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileA"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileB_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileB"), snapshot.file_entries.size()-1));

			snapshot.file_entries.push_back(fileC_entry);
			snapshot.file_entry_path_map.insert(std::pair<tc::io::Path, size_t>(dirRoot_entry.dir_listing.abs_path + tc::io::Path("fileC"), snapshot.file_entries.size()-1));
		
			// create filesystem
			auto filesystem = tc::io::VirtualFileSystem(snapshot);
		
			filesystem.dispose();

			// Test .state() returns correct
			uint64_t state_ulong = filesystem.state().to_ulong();
			uint64_t expected_state_ulong = (1 << tc::RESFLAG_NOINIT);
			if (state_ulong != expected_state_ulong)
			{
				throw tc::Exception(fmt::format(".state().to_ulong() returned 0x{:x} (expected 0x{:x})", state_ulong, expected_state_ulong));
			}

			bool state_test_ready = filesystem.state().test(tc::RESFLAG_READY);
			bool expected_state_test_ready = false;
			if (state_test_ready != expected_state_test_ready)
			{
				throw tc::Exception(fmt::format(".state().test(tc::RESFLAG_READY) returned {} (expected {})", state_test_ready, expected_state_test_ready));
			}

			bool state_test_error = filesystem.state().test(tc::RESFLAG_ERROR);
			bool expected_state_test_error = false;
			if (state_test_error != expected_state_test_error)
			{
				throw tc::Exception(fmt::format(".state().test(tc::RESFLAG_ERROR) returned {} (expected {})", state_test_error, expected_state_test_error));
			}

			bool state_test_noinit = filesystem.state().test(tc::RESFLAG_NOINIT);
			bool expected_state_test_noinit = true;
			if (state_test_noinit != expected_state_test_noinit)
			{
				throw tc::Exception(fmt::format(".state().test(tc::RESFLAG_NOINIT) returned {} (expected {})", state_test_noinit, expected_state_test_noinit));
			}

			// Test using methods that should throw ObjectDisposedException, as this is not initialized
			try
			{
				filesystem.createFile(tc::io::Path());
				throw tc::Exception(".createFile() did not throw tc::ObjectDisposedException for disposed VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				filesystem.removeFile(tc::io::Path());
				throw tc::Exception(".removeFile() did not throw tc::ObjectDisposedException for disposed VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				tc::io::Path file_path;
				std::shared_ptr<tc::io::IStream> file_stream;
				filesystem.openFile(file_path, tc::io::FileMode::Open, tc::io::FileAccess::Read, file_stream);
				throw tc::Exception(".openFile() did not throw tc::ObjectDisposedException for disposed VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				filesystem.createDirectory(tc::io::Path());
				throw tc::Exception(".createDirectory() did not throw tc::ObjectDisposedException for disposed VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				filesystem.removeDirectory(tc::io::Path());
				throw tc::Exception(".removeDirectory() did not throw tc::ObjectDisposedException for disposed VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				tc::io::Path path;
				filesystem.getWorkingDirectory(path);
				throw tc::Exception(".getWorkingDirectory() did not throw tc::ObjectDisposedException for disposed VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				filesystem.setWorkingDirectory(tc::io::Path());
				throw tc::Exception(".setWorkingDirectory() did not throw tc::ObjectDisposedException for disposed VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}

			try
			{
				tc::io::sDirectoryListing info;

				filesystem.getDirectoryListing(tc::io::Path(), info);
				throw tc::Exception(".setWorkingDirectory() did not throw tc::ObjectDisposedException for disposed VirtualFileSystem");
			}
			catch (tc::ObjectDisposedException&) {
				// do nothing
			}
			
			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}