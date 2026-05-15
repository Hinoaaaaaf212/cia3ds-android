#include <tc/io/VirtualFileSystem.h>

const std::string tc::io::VirtualFileSystem::kClassName = "tc::io::VirtualFileSystem";

tc::io::VirtualFileSystem::VirtualFileSystem() :
	mCurDir(nullptr),
	mFsSnapshot(),
	mPathResolver()
{
}

tc::io::VirtualFileSystem::VirtualFileSystem(const FileSystemSnapshot& fs_snapshot, const std::shared_ptr<tc::io::IPortablePathResolver>& path_resolver) :
	VirtualFileSystem()
{
	mFsSnapshot = fs_snapshot;
	mPathResolver = path_resolver;

	// Use default path resolver if none was provided
	if (mPathResolver == nullptr)
	{
		mPathResolver = std::shared_ptr<tc::io::BasicPathResolver>(new tc::io::BasicPathResolver());
	}
	
	// get root directory
	tc::io::Path canonical_root_path = mPathResolver->resolveCanonicalPath(tc::io::Path("/"));

	auto root_itr = mFsSnapshot.dir_entry_path_map.find(canonical_root_path);
	// if the path was not found in the map, throw exception
	if (root_itr == mFsSnapshot.dir_entry_path_map.end())
	{
		throw tc::InvalidOperationException(kClassName, "Failed to located root directory");
	}
	// if the dir_entry index isn't valid, throw exception
	if (root_itr->second >= mFsSnapshot.dir_entries.size())
	{
		throw tc::InvalidOperationException(kClassName, "Failed to located root directory");
	}

	mCurDir = &mFsSnapshot.dir_entries.at(root_itr->second);
}

tc::ResourceStatus tc::io::VirtualFileSystem::state()
{
	return mCurDir == nullptr? tc::ResourceStatus(1 << tc::RESFLAG_NOINIT) : tc::ResourceStatus(1 << tc::RESFLAG_READY);
}

void tc::io::VirtualFileSystem::dispose()
{
	mCurDir = nullptr;
	mFsSnapshot.dir_entries.clear();
	mFsSnapshot.file_entries.clear();
	mFsSnapshot.dir_entry_path_map.clear();
	mFsSnapshot.file_entry_path_map.clear();
	mPathResolver.reset();
}

void tc::io::VirtualFileSystem::createFile(const tc::io::Path& path)
{
	if (mCurDir == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::createFile()", "VirtualFileSystem not initialized");
	}

	throw tc::NotImplementedException(kClassName+"::createFile()", "createFile is not supported for VirtualFileSystem");
}

void tc::io::VirtualFileSystem::removeFile(const tc::io::Path& path)
{
	if (mCurDir == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::removeFile()", "VirtualFileSystem not initialized");
	}

	throw tc::NotImplementedException(kClassName+"::removeFile()", "removeFile is not supported for VirtualFileSystem");
}

void tc::io::VirtualFileSystem::openFile(const tc::io::Path& path, tc::io::FileMode mode, tc::io::FileAccess access, std::shared_ptr<tc::io::IStream>& stream)
{
	if (mCurDir == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::openFile()", "VirtualFileSystem not initialized");
	}

	tc::io::Path resolved_path = mPathResolver->resolveCanonicalPath(path);

	if (mode != tc::io::FileMode::Open)
	{
		throw tc::NotSupportedException(kClassName+"::openFile()", "This file-system is read-only, only FileMode::Open is supported.");
	}
	if (access != tc::io::FileAccess::Read)
	{
		throw tc::NotSupportedException(kClassName+"::openFile()", "This file-system is read-only, only FileAccess::Read is supported.");
	}

	auto file_itr = mFsSnapshot.file_entry_path_map.find(resolved_path);
	// if resolved_path does not exist in the map, throw exception
	if (file_itr == mFsSnapshot.file_entry_path_map.end())
	{
		throw tc::io::FileNotFoundException(kClassName+"::openFile()", "File does not exist.");
	}
	// if the file_entry index isn't valid or leads to a null IStream pointer, throw exception
	if (file_itr->second >= mFsSnapshot.file_entries.size() || mFsSnapshot.file_entries.at(file_itr->second).stream == nullptr)
	{
		throw tc::io::FileNotFoundException(kClassName+"::openFile()", "File does not exist.");
	}
	// if the stream has invalid properties, throw exception
	if ( !(mFsSnapshot.file_entries.at(file_itr->second).stream->canRead() == true && mFsSnapshot.file_entries.at(file_itr->second).stream->canWrite() == false) )
	{
		throw tc::io::FileNotFoundException(kClassName+"::openFile()", "File does not exist.");
	}

	stream = mFsSnapshot.file_entries.at(file_itr->second).stream;
}

void tc::io::VirtualFileSystem::createDirectory(const tc::io::Path& path)
{
	if (mCurDir == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::createDirectory()", "VirtualFileSystem not initialized");
	}

	throw tc::NotImplementedException(kClassName+"::createDirectory()", "createDirectory is not supported for VirtualFileSystem");
}

void tc::io::VirtualFileSystem::removeDirectory(const tc::io::Path& path)
{
	if (mCurDir == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::removeDirectory()", "VirtualFileSystem not initialized");
	}

	throw tc::NotImplementedException(kClassName+"::removeDirectory()", "removeDirectory is not supported for VirtualFileSystem");
}

void tc::io::VirtualFileSystem::getWorkingDirectory(tc::io::Path& path)
{
	if (mCurDir == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::getWorkingDirectory()", "VirtualFileSystem not initialized");
	}

	path = mCurDir->dir_listing.abs_path;
}

void tc::io::VirtualFileSystem::setWorkingDirectory(const tc::io::Path& path)
{
	if (mCurDir == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::setWorkingDirectory()", "VirtualFileSystem not initialized");
	}

	tc::io::Path resolved_path = mPathResolver->resolveCanonicalPath(path);

	auto dir_itr = mFsSnapshot.dir_entry_path_map.find(resolved_path);
	// if the path was not found in the map, throw exception
	if (dir_itr == mFsSnapshot.dir_entry_path_map.end())
	{
		throw tc::io::DirectoryNotFoundException(kClassName+"::setWorkingDirectory()", "Directory does not exist.");
	}
	// if the dir_entry index isn't valid, throw exception
	if (dir_itr->second >= mFsSnapshot.dir_entries.size())
	{
		throw tc::io::DirectoryNotFoundException(kClassName+"::setWorkingDirectory()", "Directory does not exist.");
	}

	mCurDir = &mFsSnapshot.dir_entries.at(dir_itr->second);
	mPathResolver->setCurrentDirectory(mCurDir->dir_listing.abs_path);
}

void tc::io::VirtualFileSystem::getDirectoryListing(const tc::io::Path& path, tc::io::sDirectoryListing& info)
{
	if (mCurDir == nullptr)
	{
		throw tc::ObjectDisposedException(kClassName+"::getDirectoryListing()", "VirtualFileSystem not initialized");
	}

	tc::io::Path resolved_path = mPathResolver->resolveCanonicalPath(path);

	auto dir_itr = mFsSnapshot.dir_entry_path_map.find(resolved_path);
	// if the path was not found in the map, throw exception
	if (dir_itr == mFsSnapshot.dir_entry_path_map.end())
	{
		throw tc::io::DirectoryNotFoundException(kClassName+"::getDirectoryListing()", "Directory does not exist.");
	}
	// if the dir_entry index isn't valid, throw exception
	if (dir_itr->second >= mFsSnapshot.dir_entries.size())
	{
		throw tc::io::DirectoryNotFoundException(kClassName+"::getDirectoryListing()", "Directory does not exist.");
	}

	info = mFsSnapshot.dir_entries.at(dir_itr->second).dir_listing;
}