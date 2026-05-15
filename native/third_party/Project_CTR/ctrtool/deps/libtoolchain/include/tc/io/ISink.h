	/**
	 * @file ISink.h
	 * @brief Declaration of tc::io::ISink
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2020/04/10
	 **/
#pragma once
#include <tc/types.h>
#include <tc/ResourceStatus.h>
#include <tc/ByteData.h>

namespace tc { namespace io {

	/**
	 * @class ISink
	 * @brief An interface defining a byte data sink.
	 **/
class ISink
{
public:
		/// Destructor
	virtual ~ISink() = default;

		/// Gets the length of the sink.
	virtual int64_t length() = 0;

		/**
		 * @brief Sets the length of the sink.
		 * 
		 * @param[in] length The desired length of the sink in bytes.
		 **/
	virtual void setLength(int64_t length) = 0;

		/**
		 * @brief Push data to the sink.
		 * 
		 * @param[in] data Data to be pushed to the sink.
		 * @param[in] offset Zero-based offset in sink to push data.
		 * 
		 * @return Number of bytes pushed to sink.
		 **/
	virtual size_t pushData(const tc::ByteData& data, int64_t offset) = 0;
};

}} // namespace tc::io