	/**
	 * @file TranscodeUtil.h
	 * @brief Declaration of tc::string::TranscodeUtil
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2020/03/22
	 **/
#pragma once
#include <string>

#include <tc/ArgumentException.h>

namespace tc { namespace string {

	/**
	 * @class TranscodeUtil
	 * @brief Collection of functions to transcode between UTF-8/UTF-16/UTF-32
	 **/
class TranscodeUtil
{
public:
		/**
		 * @brief Transcode a UTF-8 string to UTF-32.
		 * @param[in] src Source UTF-8 string.
		 * @param[out] dst Destination UTF-32 string.
		 * 
		 * @throw tc::ArgumentException When src is an invalid string.
		 **/
	static void UTF8ToUTF32(const std::string& src, std::u32string& dst);

		/**
		 * @brief Transcode a UTF-16 string to UTF-32.
		 * @param[in] src Source UTF-16 string.
		 * @param[out] dst Destination UTF-32 string.
		 *
		 * @throw tc::ArgumentException When src is an invalid string.
		 **/
	static void UTF16ToUTF32(const std::u16string& src, std::u32string& dst);

		/**
		 * @brief Transcode a UTF-32 string to UTF-8.
		 * @param[in] src Source UTF-32 string.
		 * @param[out] dst Destination UTF-8 string.
		 * 
		 * @throw tc::ArgumentException When src is an invalid string.
		 **/
	static void UTF32ToUTF8(const std::u32string& src, std::string& dst);

		/**
		 * @brief Transcode a UTF-32 string to UTF-16.
		 * @param[in] src Source UTF-32 string.
		 * @param[out] dst Destination UTF-16 string.
		 * 
		 * @throw tc::ArgumentException When src is an invalid string.
		 **/
	static void UTF32ToUTF16(const std::u32string& src, std::u16string& dst);

		/**
		 * @brief Transcode a UTF-8 string to UTF-16.
		 * @param[in] src Source UTF-8 string.
		 * @param[out] dst Destination UTF-16 string.
		 * 
		 * @throw tc::ArgumentException When src is an invalid string.
		 **/
	static void UTF8ToUTF16(const std::string& src, std::u16string& dst);

		/**
		 * @brief Transcode a UTF-16 string to UTF-8.
		 * @param[in] src Source UTF-16 string.
		 * @param[out] dst Destination UTF-8 string.
		 * 
		 * @throw tc::ArgumentException When src is an invalid string.
		 **/
	static void UTF16ToUTF8(const std::u16string& src, std::string& dst);
};

}} // namespace tc::string