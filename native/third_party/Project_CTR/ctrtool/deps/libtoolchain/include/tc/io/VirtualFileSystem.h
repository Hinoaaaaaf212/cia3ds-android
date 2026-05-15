	/**
	 * @file VirtualFileSystem.h
	 * @brief Declaration of tc::io::VirtualFileSystem
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2022/02/08
	 **/
#pragma once
#include <tc/io/IFileSystem.h>
#include <tc/io/BasicPathResolver.h>

#include <tc/ObjectDisposedException.h>
#include <tc/NotImplementedException.h>
#include <tc/NotSupportedException.h>
#include <tc/io/DirectoryNotFoundException.h>
#include <tc/io/FileNotFoundException.h>

namespace tc { namespace io {

	/**
	 * @class VirtualFileSystem
	 * @brief A virtual read-only file-system created using a file-system snapshot.
	 * 
	 * @details
	 * The intended use-case for VirtualFileSystem is for when the user must read/parse the file-system structures directly instead of using the OS to mount it.
	 * The user generates a file-system snapshot from the file-system structures (see @ref VirtualFileSystem::FileSystemSnapshot) and supplies that to VirtualFileSystem to create a IFileSystem shim.
	 *
	 * User supplies:
	 * * a @ref VirtualFileSystem::FileSystemSnapshot struct which contains vectors of directory and file entries, including mapping between absolute tc::io::Path and a dir/file entry.
	 * * optionally an implementation of @ref tc::io::IPortablePathResolver to determine the absolute path from a relative path and the current directory. Providing a custom IPortablePathResolver implementation is only required when special logic (like case insensitivity) is required to resolve the correct absolute path.
	 **/	
class VirtualFileSystem : public tc::io::IFileSystem
{
public:
		/**
		 * @struct FileSystemSnapshot
		 * @brief A struct which contains vectors of directory and file entries, including mapping between absolute tc::io::Path and a dir/file entry.
		 *
		 * @details
		 * FileSystemSnapshot is snapshot of a FileSystem directory tree. It only has to support read-only access to directory information and file streams.
		 *
		 * VirtualFileSystem considers a directory to exist if the following conditions are satisfied:
		 * * The absolute tc::io::Path to the directory exists in @b dir_entry_path_map
		 * * The index from looking up the absolute path in @b dir_entry_path_map is a valid index for @b dir_entries
		 *
		 * VirtualFileSystem considers a file to exist if the following conditions are satisfied:
		 * * The absolute tc::io::Path to the file exists in @b file_entry_path_map
		 * * The index from looking up the absolute path in @b file_entry_path_map is a valid index for @b file_entries
		 * * The FileEntry in @b file_entries has a @b stream member that is not @b nullptr
		 * * The IStream has properties: @b .canRead() == @b true, @b .canWrite() == @b false
		 */
	struct FileSystemSnapshot
	{
		FileSystemSnapshot() :
			dir_entries(),
			file_entries(),
			dir_entry_path_map(),
			file_entry_path_map()
		{
		}

			/**
			 * @struct DirEntry
			 * @brief This struct contains data for the directory entry.
			 * @details
			 * This currently contains only a @ref tc::io::sDirectoryListing.
			 *
			 * DirEntries are used to confirm that a directory exists, and also to provide the sDirectoryListing for getDirectoryListing()
			 */
		struct DirEntry
		{
			tc::io::sDirectoryListing dir_listing;
		};

			/**
			 * @struct FileEntry
			 * @brief This struct contains data for the file entry.
			 * @details
			 * This currently contains only a @ref tc::io::IStream pointer.
			 *
			 * FileEntry objects are used to confirm that a files exists, and also to provide the IStream for openFile() 
			 * IStream objects should have .canRead() == true AND .canWrite() == false, canSeek() is preferred to be true, but it isn't required.
			 */
		struct FileEntry
		{
			std::shared_ptr<tc::io::IStream> stream;
		};

			/// Vector of directory entries
		std::vector<DirEntry> dir_entries;
			/// Vector of file entries
		std::vector<FileEntry> file_entries;
			/// Mapping of absolute path to index of DirEntry in dir_entries
		std::map<tc::io::Path, size_t> dir_entry_path_map;
			/// Mapping of absolute path to index of FileEntry in file_entries
		std::map<tc::io::Path, size_t> file_entry_path_map;
	};

		/**
		 * @brief Default constructor
		 * @post This will create an unusable VirtualFileSystem, it will have to be assigned from a valid VirtualFileSystem object to be usable.
		 **/
	VirtualFileSystem();

		/** 
		 * @brief Create VirtualFileSystem
		 * 
		 * @param[in] fs_snapshot The FileSystemSnapshot object which this VirtualFileSystem will use to process file-system requests.
		 * @param[in] path_resolver Pointer to @ref tc::io::IPortablePathResolver object that resolves relative paths to absolute paths. If @p nullptr, @ref tc::io::BasicPathResolver will be used.
		 * 
		 * @throw tc::InvalidOperationException @p fs_snapshot Did not contain a root directory entry.
		 **/
	VirtualFileSystem(const FileSystemSnapshot& fs_snapshot, const std::shared_ptr<tc::io::IPortablePathResolver>& path_resolver = nullptr);

	tc::ResourceStatus state();

		/// This will release the underlying FileSystemSnapshot and PathResolver
	void dispose();

		/** 
		 * @brief Create a new file
		 * @details This method is not implemented for VirtualFileSystem.
		 * 
		 * @param[in] path A relative or absolute path to file.
		 * 
		 * @throw tc::NotImplementedException This method is not implemented for VirtualFileSystem.
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 **/
	void createFile(const tc::io::Path& path);

		/** 
		 * @brief Remove a file
		 * @details This method is not implemented for VirtualFileSystem.
		 * 
		 * @param[in] path A relative or absolute path to file.
		 * 
		 * @throw tc::NotImplementedException This method is not implemented for VirtualFileSystem.
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 **/
	void removeFile(const tc::io::Path& path);

		/** 
		 * @brief Open a file
		 * 
		 * @param[in] path A relative or absolute path to file.
		 * @param[in] mode One of the enumeration values that determines how to open or create the file. This must be @ref tc::io::FileMode::Open for VirtualFileSystem.
		 * @param[in] access One of the enumeration values that determines how the file can be accessed by the @ref IStream object. This must be @ref tc::io::FileAccess::Read for VirtualFileSystem.
		 * @param[out] stream Pointer to IStream object to be instantiated.
		 * 
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 * @throw tc::NotSupportedException Unsupported access/mode ( @p mode was not @ref tc::io::FileMode::Open, or @p access was not @ref tc::io::FileAccess::Read).
		 * @throw tc::io::FileNotFoundException File was not found.
		 **/
	void openFile(const tc::io::Path& path, tc::io::FileMode mode, tc::io::FileAccess access, std::shared_ptr<tc::io::IStream>& stream);
	
		/** 
		 * @brief Create a new directory
		 * @details This method is not implemented for VirtualFileSystem.
		 * 
		 * @param[in] path A relative or absolute path to directory.
		 * 
		 * @throw tc::NotImplementedException This method is not implemented for VirtualFileSystem.
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 **/
	void createDirectory(const tc::io::Path& path);

		/** 
		 * @brief Remove a directory
		 * @details This method is not implemented for VirtualFileSystem.
		 * 
		 * @param[in] path A relative or absolute path to directory.
		 * 
		 * @throw tc::NotImplementedException This method is not implemented for VirtualFileSystem.
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 **/
	void removeDirectory(const tc::io::Path& path);

		/** 
		 * @brief Get the full path of the working directory
		 * @param[out] path Path object to populate.
		 * 
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 **/
	void getWorkingDirectory(tc::io::Path& path);

		/** 
		 * @brief Change the working directory
		 * @param[in] path A relative or absolute path to directory.
		 * 
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 * @throw tc::io::DirectoryNotFoundException Directory was not found.
		 **/
	void setWorkingDirectory(const tc::io::Path& path);

		/** 
		 * @brief Get directory listing a directory
		 * @param[in] path A relative or absolute path to directory.
		 * @param[out] info The sDirectoryListing object to populate.
		 * 
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 * @throw tc::io::DirectoryNotFoundException Directory was not found.
		 **/
	void getDirectoryListing(const tc::io::Path& path, tc::io::sDirectoryListing& info);
private:
	static const std::string kClassName;
	
	FileSystemSnapshot::DirEntry* mCurDir;
	FileSystemSnapshot mFsSnapshot;
	std::shared_ptr<tc::io::IPortablePathResolver> mPathResolver;
};

}} // namespace tc::io