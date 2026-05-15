	/**
	 * @file string.h
	 * @brief Declaration of tc::bn::string
	 * @author Jack (jakcron)
	 * @version 0.3
	 * @date 2022/02/05
	 */
#pragma once
#include <tc/types.h>

namespace tc { namespace bn {

	/**
	 * @class string
	 * @brief This class represents a literal char array.
	 *
	 * @tparam ENCODED_SIZE Literal size of the string structure. sizeof() will return this size.
	 * @tparam LOGICAL_SIZE Logical maximum size of the string, LOGICAL_SIZE <= ENCODED_SIZE. 
	 *
	 * @details The intended for use is for defining structures read from files. 
	 * 
	 * Consider this structure read from a file with some comments from the spec:
	 * @code
	 *	struct MyStruct
	 *	{
	 *		uint32_t version;
	 *		char product_sku[16]; // ASCII but not NULL terminated, so 16 usable characters
	 *		char product_title[32]; // ASCII but must be NULL terminated, so only 31 usable chars
	 *		uint32_t product_version;
	 *	};
	 * @endcode
	 * The member @c product_sku according to specification can have all 16 chars populated, and if all 16 chars 
	 * are populated it isn't guarenteed to be null terminated. The member @c product_title according to specification 
	 * must be null terminated, reserving one byte for the null byte. This isn't the same behaviour as @c product_sku, 
	 * requiring different string logic specific to this one struct. But these constraints can be enforced into the 
	 * defintion of MyStruct using @ref tc::bn::string.
	 *
	 * Consider this revised struct using tc::bn::string :
	 * @code
	 *	struct MyStruct
	 *	{
	 *		uint32_t version;
	 *		tc::bn::string<16> product_sku; // this has a ENCODED_SIZE & LOGICAL_SIZE of 16 bytes which mean's the size on disk is 16 bytes and there are 16 usable characters
	 *		tc::bn::string<32,31> product_title; // this has a ENCODED_SIZE of 32 bytes & LOGICAL_SIZE of 31 bytes which mean's the size on disk is 32 bytes and there are 31 usable characters
	 *		uint32_t product_version;
	 *	};
	 * @endcode
	 * In the above struct the correct size of @c product_sku and @c product_title are preserved while also enforcing the logical size of the string.
	 *
	 * To get the maximum length a string can be for a given tc::bn::string, use @ref max_size():
	 * @code
	 * MyStruct st;
	 * size_t product_sku_max_size = st.product_sku.max_size(); // 16
	 * size_t product_title_max_size = st.product_title.max_size(); // 31
	 * @endcode
	 *
	 * To get the current string length for a given tc::bn::string, use @ref size():
	 * @code
	 * MyStruct st;
	 * size_t product_sku_str_size = st.product_sku.size();
	 * size_t product_title_str_size = st.product_title.size();
	 * @endcode
	 *
	 * To decode the data in a tc::bn::string to a std::string, use @ref decode():
	 * @code
	 * MyStruct st;
	 * std::string product_sku = st.product_sku.decode();
	 * std::string product_title = st.product_title.decode();
	 * @endcode
	 *
	 * To encode a std::string into a tc::bn::string, use @ref encode():
	 * @code
	 * MyStruct st;
	 * st.product_sku.encode("SKU-1234-X");
	 * st.product_title.encode("MyProductTitle");
	 * @endcode
	 */
template <size_t ENCODED_SIZE, size_t LOGICAL_SIZE = ENCODED_SIZE>
class string
{
public:
	static_assert(ENCODED_SIZE >= LOGICAL_SIZE, "literal string had a logical size greater than the encoded size.");

		/// Access specific element
	const char& operator[](size_t index) const { return mRawString[index]; }

		/// Access specific element
	char& operator[](size_t index) { return mRawString[index]; }

		/// Direct access to the underlying array
	const char* data() const { return mRawString.data(); }

		/// Direct access to the underlying array
	char* data() { return mRawString.data(); }

		/// Returns maximum possible length for this string
	size_t max_size() const { return LOGICAL_SIZE; }
		
		/// Returns length of string
	size_t size() const
	{
		size_t chr_count = 0;

		for (; chr_count < LOGICAL_SIZE; chr_count++)
		{
			if (mRawString[chr_count] == 0) break;
		}

		return chr_count;
	}
	
		/// Returns a std::string created from the underlying char array
	std::string decode() const { return std::string(this->data(), this->size()); }

		/// Encode the underlying char array from a std::string
	void encode(const std::string& source_str)
	{
		size_t chr_count = 0;

		// copy chars from source_str
		for (; chr_count < LOGICAL_SIZE; chr_count++)
		{
			// skip if chr count exceeds the size of the string, or the string char is null byte
			if (chr_count >= source_str.size()) break;
			if (source_str[chr_count] == 0) break;

			mRawString[chr_count] = source_str[chr_count];
		}

		// clear remaining chars
		for (; chr_count < LOGICAL_SIZE; chr_count++)
		{
			mRawString[chr_count] = 0;
		}
	}

private:
	std::array<char, ENCODED_SIZE> mRawString;
};

}} // namespace tc::bn