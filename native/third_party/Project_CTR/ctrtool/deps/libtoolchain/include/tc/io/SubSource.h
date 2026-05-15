	/**
	 * @file SubSource.h
	 * @brief Declaration of tc::io::SubSource
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/02/08
	 **/
#pragma once
#include <tc/io/ISource.h>

#include <tc/ArgumentNullException.h>
#include <tc/ArgumentOutOfRangeException.h>

namespace tc { namespace io {

	/**
	 * @class SubSource
	 * @brief A ISource that exposes a subset of a base ISource.
	 **/
class SubSource : public tc::io::ISource
{
public:
		/**
		 * @brief Default constructor
		 * @post This will create an unusable SubSource, it will have to be assigned from a valid SubSource object to be usable.
		 **/ 
	SubSource();

		/** 
		 * @brief Create SubSource
		 * 
		 * @param[in] source The base ISource object which this sub source will derive from.
		 * @param[in] offset The zero-based byte offset in source at which to begin the sub source.
		 * @param[in] length Length of the sub source.
		 * 
		 * @pre The sub source must be a subset of the base source.
		 * 
		 * @throw tc::ArgumentNullException @p source is a @p nullptr.
		 * @throw tc::ArgumentOutOfRangeException @p offset or @p length is negative or otherwise invalid given the length of the base source.
		 **/
	SubSource(const std::shared_ptr<tc::io::ISource>& source, int64_t offset, int64_t length);

		/// Get length of source
	int64_t length();

		/**
		 * @brief Pull data from source
		 * 
		 * @param[in] offset Zero-based offset in source to pull data.
		 * @param[in] count The maximum number of bytes to be pull from the source.
		 *
		 * @return ByteData containing data pulled from source
		 **/
	tc::ByteData pullData(int64_t offset, size_t count);
private:
	static const std::string kClassName;

	std::shared_ptr<tc::io::ISource> mBaseSource;
	int64_t mBaseSourceOffset;

	int64_t mSubSourceLength;
};

}} // namespace tc::io