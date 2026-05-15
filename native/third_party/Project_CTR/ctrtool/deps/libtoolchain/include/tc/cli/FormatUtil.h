	/**
	 * @file FormatUtil.h
	 * @brief Declaration of tc::cli::FormatUtil
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2020/12/31
	 **/
#pragma once
#include <tc/types.h>
#include <tc/ByteData.h>

namespace tc { namespace cli {

	/**
	 * @class FormatUtil
	 * @brief A collection of utilities to format binary data as strings and vice-versa.
	 **/
class FormatUtil
{
public:
		/**
		 * @brief Convert a hexadecimal string to bytes.
		 * 
		 * @param[in] str Hexadecimal string to convert.
		 * 
		 * @return Converted string as bytes.
		 * 
		 * @post ByteData returned will be empty if the string has encoding errors.
		 **/
	static tc::ByteData hexStringToBytes(const std::string& str);

		/**
		 * @brief Format raw bytes as a hexadecimal string.
		 * 
		 * @param[in] data Pointer to bytes to format.
		 * @param[in] size Size of data to format.
		 * @param[in] is_upper_case Format bytes in upper case. If false the bytes will be formatted in lower case.
		 * @param[in] delimiter String to separate formated bytes with.
		 * 
		 * @return Formatted string
		 **/
	static std::string formatBytesAsString(const byte_t* data, size_t size, bool is_upper_case, const std::string& delimiter);

		/**
		 * @brief Format tc::ByteData as a hexadecimal string.
		 * 
		 * @param[in] data Reference to tc::ByteData object to format.
		 * @param[in] is_upper_case Format bytes in upper case. If false the bytes will be formatted in lower case.
		 * @param[in] delimiter String to separate formated bytes with.
		 * 
		 * @return Formatted string
		 **/
	static std::string formatBytesAsString(const tc::ByteData& data, bool is_upper_case, const std::string& delimiter);

		/**
		 * @brief Format raw bytes as a hexadecimal string. Introducing a new-line to keep each row within a certain size.
		 * 
		 * @param[in] data Pointer to bytes to format.
		 * @param[in] size Size of data to format.
		 * @param[in] is_upper_case Format bytes in upper case. If false the bytes will be formatted in lower case.
		 * @param[in] delimiter String to separate formated bytes with.
		 * @param[in] row_len Maximum length each row can be before a newline is introduced.
		 * @param[in] indent_len Length of spaces each new line should be indented.
		 * @param[in] print_first_indent Print the indent for the first line, default is true.
		 * 
		 * @return Formatted string
		 **/
	static std::string formatBytesAsStringWithLineLimit(const byte_t* data, size_t size, bool is_upper_case, const std::string& delimiter, size_t row_len, size_t indent_len, bool print_first_indent = true);

		/**
		 * @brief Format a list of strings as comma delimited. Introducing a new-line to keep each row within a certain size.
		 * 
		 * @param[in] str_list List of strings to print.
		 * @param[in] row_len Maximum length each row can be before a newline is introduced.
		 * @param[in] indent_len Length of spaces each new line should be indented.
		 * @param[in] print_first_indent Print the indent for the first line, default is true.
		 * 
		 * @return Formatted string
		 **/
	static std::string formatListWithLineLimit(const std::vector<std::string>& str_list, size_t row_len, size_t indent_len, bool print_first_indent = true);

		/**
		 * @brief Format raw bytes in the style of HxD editor hex view. 
		 * 
		 * @param[in] data Pointer to bytes to format.
		 * @param[in] size Size of data to format.
		 * @param[in] bytes_per_row Number of bytes to print on each row.
		 * @param[in] byte_group_size Size of each byte group.
		 * 
		 * @return Formatted string
		 **/
	static std::string formatBytesAsHxdHexString(const byte_t* data, size_t size, size_t bytes_per_row, size_t byte_group_size);

		/**
		 * @brief Format raw bytes in the style of HxD editor hex view. 
		 * 
		 * @param[in] data Pointer to bytes to format.
		 * @param[in] size Size of data to format.
		 * 
		 * @return Formatted string
		 **/
	static std::string formatBytesAsHxdHexString(const byte_t* data, size_t size);
};

}} // namespace tc::cli