	/**
	 * @file IOUtil.h
	 * @brief Declaration of tc::io::IOUtil
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/04/10
	 **/
#pragma once
#include <tc/types.h>

namespace tc { namespace io {

	/**
	 * @class IOUtil
	 * @brief Utility functions for IO based classes.
	 **/
class IOUtil
{
public:
		/**
		 * @brief Convert size_t to int64_t safely.
		 **/
	static int64_t castSizeToInt64(size_t size);

		/**
		 * @brief Convert int64_t to size_t safely.
		 **/
	static size_t castInt64ToSize(int64_t length);

		/**
		 * @brief Get size of available data for an IO class performing a read or write operation.
		 * 
		 * @param[in] data_length Total length of data.
		 * @param[in] data_offset Byte offset in data to begin operation from.
		 * 
		 * @return Largest possible operable data size.
		 **/
	static size_t getAvailableSize(int64_t data_length, int64_t data_offset);

		/**
		 * @brief Get size of writeable data for an IO class, given the data length, desired read offset and count.
		 * 
		 * @param[in] data_length Total length of data.
		 * @param[in] data_offset Byte offset in data to begin reading from.
		 * @param[in] requested_read_count Number of bytes to read.
		 * 
		 * @return Largest possible readable count.
		 **/
	static size_t getReadableCount(int64_t data_length, int64_t data_offset, size_t requested_read_count);

		/**
		 * @brief Get size of writeable data for an IO class, given the data length, desired write offset and count.
		 * 
		 * @param[in] data_length Total length of data.
		 * @param[in] data_offset Byte offset in data to begin writing from.
		 * @param[in] requested_write_count Number of bytes to write.
		 * 
		 * @return Largest possible writeable count.
		 **/
	static size_t getWritableCount(int64_t data_length, int64_t data_offset, size_t requested_write_count);
};

}} // namespace tc::io