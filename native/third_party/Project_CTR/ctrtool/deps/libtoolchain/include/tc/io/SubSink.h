	/**
	 * @file SubSink.h
	 * @brief Declaration of tc::io::SubSink
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2020/04/10
	 **/
#pragma once
#include <tc/io/ISink.h>

#include <tc/ArgumentNullException.h>
#include <tc/ArgumentOutOfRangeException.h>
#include <tc/OutOfMemoryException.h>
#include <tc/ObjectDisposedException.h>
#include <tc/NotSupportedException.h>
#include <tc/NotImplementedException.h>

namespace tc { namespace io {

	/**
	 * @class SubSink
	 * @brief A ISink that exposes a subset of a base ISink.
	 **/
class SubSink : public tc::io::ISink
{
public:
		/**
		 * @brief Default constructor
		 * @post This will create an unusable SubSink, it will have to be assigned from a valid SubSink object to be usable.
		 **/ 
	SubSink();

		/** 
		 * @brief Create SubSink
		 * 
		 * @param[in] sink The base ISink object which this sub sink will derive from.
		 * @param[in] offset The zero-based byte offset in sink at which to begin the sub sink.
		 * @param[in] length Length of the sub sink.
		 * 
		 * @pre The sub sink must be a subset of the base sink.
		 * 
		 * @throw tc::ArgumentNullException @p sink is a @p nullptr.
		 * @throw tc::ArgumentOutOfRangeException @p offset or @p length is negative or otherwise invalid given the length of the base sink.
		 **/
	SubSink(const std::shared_ptr<tc::io::ISink>& sink, int64_t offset, int64_t length);

		/// Gets the length of the sink.
	int64_t length();

		/**
		 * @brief Sets the length of the sink. This is not supported for SubSink. 
		 * @throw tc::NotImplemented setLength is not implemented for SubSink.
		 **/
	void setLength(int64_t length);

		/**
		 * @brief Push data to the sink.
		 * 
		 * @param[in] data Data to be pushed to the sink.
		 * @param[in] offset Zero-based offset in sink to push data.
		 * 
		 * @return Number of bytes pushed to sink.
		 * 
		 * @throw tc::ObjectDisposedException The base sink was not initialized.
		 * @throw tc::ArgumentOutOfRangeException @p data was too large to be pushed to the sink.
		 **/
	size_t pushData(const tc::ByteData& data, int64_t offset);
private:
	static const std::string kClassName;

	std::shared_ptr<tc::io::ISink> mBaseSink;
	int64_t mBaseSinkOffset;

	int64_t mSubSinkLength;
};

}} // namespace tc::io