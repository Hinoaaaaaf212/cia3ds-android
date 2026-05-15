	/**
	 * @file ByteData.h
	 * @brief Declaration of tc::ByteData
	 * @author Jack (jakcron)
	 * @version 0.5
	 * @date 2020/10/31
	 **/
#pragma once
#include <tc/types.h>

#include <tc/OutOfMemoryException.h>

namespace tc {

	/**
	 * @class ByteData
	 * @brief A container of linear memory, used to hold raw data.
	 **/
class ByteData
{
public:
		/// Create empty ByteData
	ByteData();

		/// Copy constructor
	ByteData(const ByteData& other);

		/// Move constructor
	ByteData(ByteData&& other);

		/// Create from byte_t initalizer list
	ByteData(std::initializer_list<byte_t> l);

		/**
		 * @brief Create linear memory block
		 * 
		 * @param[in] size Size in bytes of the memory block.
		 * @param[in] clear_memory Clear memory after allocation. Default is true.
		 * 
		 * @throw tc::OutOfMemoryException Insuffient memory available.
		 **/
	ByteData(size_t size, bool clear_memory = true);

		/**
		 * @brief Create ByteData from existing memory.
		 * 
		 * @param[in] data Pointer to memory to copy.
		 * @param[in] size Size of memory to copy.
		 * 
		 * @throw tc::OutOfMemoryException Insuffient memory available.
		 **/
	ByteData(const byte_t* data, size_t size);
		
		/**
		 * @brief Copy assignment operator (deep copy)
		 **/
	ByteData& operator=(const ByteData& other);

		/**
		 * @brief Move assignment
		 **/
	ByteData& operator=(ByteData&& other);

		/**
		 * @brief Element access operator
		 **/
	byte_t& operator[](size_t index);

		/**
		 * @brief Const Element access operator
		 **/
	byte_t operator[](size_t index) const;

		/**
		 * @brief Equality operator
		 */
	bool operator==(const ByteData& other) const;

		/**
		 * @brief Inequality operator
		 */
	bool operator!=(const ByteData& other) const;

		/**
		 * @brief Get data pointer
		 * 
		 * @return nullptr if @ref size() == 0
		 **/
	byte_t* data() const;

		/**
		 * @brief Get data size
		 **/
	size_t size() const;
private:
	static const std::string kClassName;

	size_t mSize;
	std::unique_ptr<byte_t> mPtr;
}; 

} // namespace tc