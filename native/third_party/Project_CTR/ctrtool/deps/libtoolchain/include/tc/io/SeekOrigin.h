	/**
	 * @file SeekOrigin.h
	 * @brief Declaration of tc::io::SeekOrigin
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/01/22
	 **/
#pragma once

namespace tc { namespace io {

	/**
	 * @enum SeekOrigin
	 * @brief Specifies the position in a stream to use for seeking.
	 **/
enum class SeekOrigin
{
	Begin = 0, /**< Specifies the beginning of a stream. */
	Current = 1, /**< Specifies the current position within a stream. */
	End = 2 /**< Specifies the end of a stream. */
};

}} // namespace tc::io