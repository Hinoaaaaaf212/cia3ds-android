/**
 * @file PathUtil.h
 * @brief Declaration of tc::io::PathUtil
 * @author Jack (jakcron)
 * @version 0.2
 * @date 2020/03/22
 */
#pragma once
#include <list>
#include <tc/types.h>
#include <tc/io/Path.h>

namespace tc { namespace io {

	/**
	 * @class PathUtil
	 * @brief Collection of utilities related to tc::io::Path
	 **/
class PathUtil
{
public:
		/**
		 * @brief Format a Path as a Windows style UTF-16 string
		 * @param[in] path Source Path
		 * @param[out] out Destination UTF-16 string
		 * @note See @ref tc::io::Path
		 **/
	static void pathToWindowsUTF16(const tc::io::Path& path, std::u16string& out);

		/**
		 * @brief Format a Path as a Unix/Linux style UTF-8 string
		 * @param[in] path Source Path
		 * @param[out] out Destination UTF-8 string
		 * @note See @ref tc::io::Path
		 **/
	static void pathToUnixUTF8(const tc::io::Path& path, std::string& out);
};

}} // namespace tc::io