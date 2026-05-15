#pragma once

#include <tc/io.h>
#include <tc/NotImplementedException.h>

class FileSystemTestUtil
{
public:
	class DummyFileSystemBase : public tc::io::IFileSystem
	{
	public:
		DummyFileSystemBase()
		{
			init();
		}

		void init()
		{
			dispose();
			mCurDir = std::make_shared<tc::io::Path>(tc::io::Path("/some/initial/path/"));
			mState.set(tc::RESFLAG_READY);
		}

		virtual tc::ResourceStatus state()
		{
			return mState;
		}

		virtual void dispose()
		{
			mState.reset();
			mCurDir.reset();
		}

		virtual void createFile(const tc::io::Path& path)
		{
			throw tc::NotImplementedException(kClassName, "createFile() not implemented");
		}

		virtual void removeFile(const tc::io::Path& path)
		{
			throw tc::NotImplementedException(kClassName, "removeFile() not implemented");
		}

		virtual void openFile(const tc::io::Path& path, tc::io::FileMode mode, tc::io::FileAccess access, std::shared_ptr<tc::io::IStream>& stream)
		{
			throw tc::NotImplementedException(kClassName, "openFile() not implemented");
		}

		virtual void createDirectory(const tc::io::Path& path)
		{
			throw tc::NotImplementedException(kClassName, "createDirectory() not implemented");
		}

		virtual void removeDirectory(const tc::io::Path& path)
		{
			throw tc::NotImplementedException(kClassName, "removeDirectory() not implemented");
		}

		virtual void getWorkingDirectory(tc::io::Path& path)
		{
			path = *mCurDir;
		}

		virtual void setWorkingDirectory(const tc::io::Path& path)
		{
			*mCurDir = path;
		}

		virtual void getDirectoryListing(const tc::io::Path& path, tc::io::sDirectoryListing& info)
		{
			throw tc::NotImplementedException(kClassName, "getDirectoryListing() not implemented");
		}
	private:
		static const std::string kClassName;
		tc::ResourceStatus mState;
		std::shared_ptr<tc::io::Path> mCurDir;
	};
};