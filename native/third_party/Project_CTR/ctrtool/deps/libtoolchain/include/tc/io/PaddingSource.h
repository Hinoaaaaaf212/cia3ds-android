	/**
	 * @file PaddingSource.h
	 * @brief Declaration of tc::io::PaddingSource
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/02/07
	 **/
#pragma once
#include <tc/io/ISource.h>

#include <tc/ArgumentOutOfRangeException.h>

namespace tc { namespace io {

	/**
	 * @class PaddingSource
	 * @brief A source that provides dummy/filler data.
	 **/
class PaddingSource : public tc::io::ISource
{
public:
		/**
		 * @brief Default constructor
		 * @post This will create a PaddingSource with length() == 0.
		 **/ 
	PaddingSource();

		/**
		 * @brief Create PaddingSource
		 * 
		 * @param[in] padding_byte Byte to fill data pulled using @ref pullData.
		 * @param[in] length Length of source.
		 * 
		 * @throw tc::ArgumentOutOfRangeException @p length is negative.
		 **/ 
	PaddingSource(byte_t padding_byte, int64_t length);

		/// Get length of source
	int64_t length();

		/**
		 * @brief Pull data from source
		 * 
		 * @param[in] offset Zero-based offset in source to pull data.
		 * @param[in] count The maximum number of bytes to be pull from the source.
		 *
		 * @return ByteData containing data pulled from source
		 * 
		 * @throw tc::OutOfMemoryException The @ref tc::ByteData object could not be created due to insuffient memory.
		 **/
	tc::ByteData pullData(int64_t offset, size_t count);
private:
	static const std::string kClassName;

	int64_t mSourceLength;
	byte_t mPaddingByte;
};

}} // namespace tc::io