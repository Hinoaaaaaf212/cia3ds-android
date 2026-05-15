	/**
	 * @file BasicPathResolver.h
	 * @brief Declaration of tc::io::BasicPathResolver
	 * @author Jack (jakcron)
	 * @version 0.3
	 * @date 2022/02/25
	 **/
#pragma once
#include <tc/io/IPortablePathResolver.h>

#include <tc/ArgumentOutOfRangeException.h>

namespace tc { namespace io {

	    /**
		 * @class BasicPathResolver
		 * @brief This implementation of IPortablePathResolver resolves a path and current directory to canonical path, resolving only '`.`', '`..`' and empty path elements.
		 * @details 
		 * This does not consider the local file-system/environment, so links or '`~`' will not be resolved properly. It is intended for processing archived/portable filesystems.
		 * 
		 * This supports custom root labels. A root label being the first element of absolute/canonical paths.
		 * * For POSIX this is an empty string, e.g. the empty string prior to / in "/some/path/to/a/file"
		 * * For Windows systems this is the drive letter, e.g. "C:" in "C:\some\path\to\a\file"
		 * 
		 * BasicPathResolver maintains a list of root labels to determine if a path being resolved is absolute or relative, including:
		 * * Implicit Root Label : This is the first element in the current working directory, will change if the current working directory changes.
		 * * Explicit Root Labels : This is a list of root labels supplied separately via @ref setExplicitRootLabels().
		 */
	class BasicPathResolver : public tc::io::IPortablePathResolver
	{
	public:
			/**
			 * @brief Default Constructor
			 * 
			 * @post The current directory will be "/" and the list of explicit root labels will be {}.
			 */
		BasicPathResolver();

			/**
			 * @brief Create BasicPathResolver
			 * 
			 * @param[in] current_directory_path Canonical path for the current directory.
			 * 
			 * @post The current directory will be @p current_directory_path and the list of explicit root labels will be {}.
			 */
		BasicPathResolver(const tc::io::Path& current_directory_path);

			/**
			 * @brief Create BasicPathResolver
			 * 
			 * @param[in] current_directory_path Canonical path for the current directory.
			 * @param[in] root_labels Vector of valid root path names for this path resolver.
			 * 
			 * @post The current directory will be @p current_directory_path and the list of explicit root labels will be @p root_labels .
			 */
		BasicPathResolver(const tc::io::Path& current_directory_path, const std::vector<std::string>& root_labels);

			/**
			 * @brief Set the current directory path
			 * 
			 * @param path Canonical current directory path.
			 * 
			 * @throws tc::ArgumentOutOfRangeException @p path was an empty path.
			 */
		void setCurrentDirectory(const tc::io::Path& path);

			/**
			 * @brief Get the current directory path
			 * 
			 * @return tc::io::Path Canonical current directory path.
			 */
		const tc::io::Path& getCurrentDirectory() const;

			/**
			 * @brief Set explicit root labels
			 * @details
			 * This is only required where multiple root labels need to be registered.
			 * 
			 * @param root_labels Vector of root labels
			 */
		void setExplicitRootLabels(const std::vector<std::string>& root_labels);

			/// Get explicit root labels
		const std::vector<std::string>& getExplicitRootLabels() const;

			/**
			 * @brief Resolve path to its canonical path
			 * 
			 * @param path Input path.
			 * @param canonical_path Output path to write resolved canonical path.
			 */
		void resolveCanonicalPath(const tc::io::Path& path, tc::io::Path& canonical_path) const;

			/**
			 * @brief Resolve path to its canonical path
			 * 
			 * @param path Input path.
			 * 
			 * @return Resolved canonical path.
			 */
		tc::io::Path resolveCanonicalPath(const tc::io::Path& path) const;
	private:
		static const std::string kClassName;

		tc::io::Path mCurrentDirPath;
		std::vector<std::string> mExplicitRootLabels;
	};

}} // namespace tc::io