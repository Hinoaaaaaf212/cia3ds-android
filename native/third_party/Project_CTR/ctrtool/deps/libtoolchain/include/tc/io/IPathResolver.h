	/**
	 * @file IPathResolver.h
	 * @brief Declaration of tc::io::IPathResolver
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2022/02/22
	 **/
#pragma once
#include <tc/io/Path.h>

namespace tc { namespace io {

	    /**
		 * @class IPathResolver
		 * @brief This is an interface for a class that resolves relative paths to canonical paths.
		 */
	class IPathResolver
	{
	public:
		virtual ~IPathResolver() = default;
		
			/**
			 * @brief Resolve path to its canonical path
			 * 
			 * @param path Input path.
			 * @param canonical_path Output path to write resolved canonical path.
			 */
		virtual void resolveCanonicalPath(const tc::io::Path& path, tc::io::Path& canonical_path) const = 0;

			/**
			 * @brief Resolve path to its canonical path
			 * 
			 * @param path Input path.
			 * 
			 * @return Resolved canonical path.
			 */
		virtual tc::io::Path resolveCanonicalPath(const tc::io::Path& path) const = 0;
	};

}} // namespace tc::io