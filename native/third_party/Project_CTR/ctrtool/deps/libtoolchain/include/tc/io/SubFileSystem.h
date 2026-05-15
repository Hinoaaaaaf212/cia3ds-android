	/**
	 * @file SubFileSystem.h
	 * @brief Declaration of tc::io::SubFileSystem
	 * @author Jack (jakcron)
	 * @version 0.5
	 * @date 2022/01/23
	 **/
#pragma once
#include <tc/io/IFileSystem.h>
#include <tc/io/BasicPathResolver.h>

#include <tc/ArgumentNullException.h>
#include <tc/InvalidOperationException.h>
#include <tc/ObjectDisposedException.h>
#include <tc/UnauthorisedAccessException.h>

namespace tc { namespace io {

	/**
	 * @class SubFileSystem
	 * @brief A wrapper around an existing IFileSystem object that exposes a subset of the base IFileSystem directory tree.
	 **/
class SubFileSystem : public tc::io::IFileSystem
{
public:

		/**
		 * @brief Default constructor
		 * @post This will create an unusable SubFileSystem, it will have to be assigned from a valid SubFileSystem object to be usable.
		 **/
	SubFileSystem();

		/** 
		 * @brief Create SubFileSystem
		 * 
		 * @param[in] file_system The base IFileSystem object which this sub file-system will derive from.
		 * @param[in] base_path The path to the subdirectory used as the substream root directory.
		 * 
		 * @throw tc::ArgumentNullException @p file_system is @a nullptr.
		 * @throw tc::InvalidOperationException @p file_system was not in a ready state.
		 *
		 * @note This will temporarily change and then restore the base file-system current working directory.
		 **/
	SubFileSystem(const std::shared_ptr<tc::io::IFileSystem>& file_system, const tc::io::Path& base_path);

	tc::ResourceStatus state();
	void dispose();

		/** 
		 * @brief Create a new file
		 * 
		 * @param[in] path A relative or absolute path to file.
		 * 
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 **/
	void createFile(const tc::io::Path& path);

		/** 
		 * @brief Remove a file
		 * 
		 * @param[in] path A relative or absolute path to file.
		 * 
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 **/
	void removeFile(const tc::io::Path& path);

		/** 
		 * @brief Open a file
		 * 
		 * @param[in] path A relative or absolute path to file.
		 * @param[in] mode One of the enumeration values that determines how to open or create the file.
		 * @param[in] access One of the enumeration values that determines how the file can be accessed by the @ref IStream object. This also determines the values returned by the @ref IStream::canRead and @ref IStream::canWrite methods of the IStream object. @ref IStream::canSeek is true if path specifies a disk file.
		 * @param[out] stream Pointer to IStream object to be instantiated
		 * 
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 **/
	void openFile(const tc::io::Path& path, tc::io::FileMode mode, tc::io::FileAccess access, std::shared_ptr<tc::io::IStream>& stream);
	
		/** 
		 * @brief Create a new directory
		 * 
		 * @param[in] path A relative or absolute path to directory.
		 * 
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 **/
	void createDirectory(const tc::io::Path& path);

		/** 
		 * @brief Remove a directory
		 * @param[in] path A relative or absolute path to directory.
		 * 
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 **/
	void removeDirectory(const tc::io::Path& path);

		/** 
		 * @brief Get the full path of the working directory
		 * @param[out] path Path object to populate
		 * 
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 *
		 * @note This will temporarily change and then restore the base file-system current working directory.
		 **/
	void getWorkingDirectory(tc::io::Path& path);

		/** 
		 * @brief Change the working directory
		 * @param[in] path A relative or absolute path to directory.
		 * 
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 * @throw tc::UnauthorisedAccessException Sub file-system escape detected.
		 **/
	void setWorkingDirectory(const tc::io::Path& path);

		/** 
		 * @brief Get directory listing a directory
		 * @param[in] path A relative or absolute path to directory.
		 * @param[out] info The sDirectoryListing object to populate
		 * 
		 * @throw tc::ObjectDisposedException Methods were called after the file-system was closed.
		 * @throw tc::UnauthorisedAccessException Sub file-system escape detected.
		 **/
	void getDirectoryListing(const tc::io::Path& path, tc::io::sDirectoryListing& info);
private:
	static const std::string kClassName;
	
	std::shared_ptr<tc::io::IFileSystem> mBaseFileSystem;

	tc::io::BasicPathResolver mBasePathResolver;
	tc::io::BasicPathResolver mSubPathResolver;

	void subPathToRealPath(const tc::io::Path& sub_path, tc::io::Path& real_path);
	void realPathToSubPath(const tc::io::Path& real_path, tc::io::Path& sub_path);
};

}} // namespace tc::io