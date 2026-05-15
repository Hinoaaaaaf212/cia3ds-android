	/**
	 * @file StreamSink.h
	 * @brief Declaration of tc::io::StreamSink
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2020/04/10
	 **/
#pragma once
#include <tc/io/ISink.h>
#include <tc/io/IStream.h>

#include <tc/ArgumentNullException.h>
#include <tc/ObjectDisposedException.h>
#include <tc/NotSupportedException.h>
#include <tc/io/IOException.h>

namespace tc { namespace io {

	/**
	 * @class StreamSink
	 * @brief An IStream wrapped in an ISink.
	 **/
class StreamSink : public tc::io::ISink
{
public:
		/**
		 * @brief Default constructor
		 * @post This will create an unusable StreamSink, it will have to be assigned from a valid StreamSink object to be usable.
		 **/ 
	StreamSink();

		/** 
		 * @brief Create StreamSink
		 * 
		 * @param[in] stream The base IStream object which this sink will derive from.
		 * 
		 * @pre The base stream must support writing.
		 * 
		 * @throw tc::ArgumentNullException @p stream is a @p nullptr.
		 * @throw tc::NotSupportedException @p stream does not support writing.
		 **/
	StreamSink(const std::shared_ptr<tc::io::IStream>& stream);

		/**
		 * @brief Gets the length of the sink.
		 **/
	int64_t length();

		/**
		 * @brief Sets the length of the sink.
		 * 
		 * @param[in] length The desired length of the sink in bytes.
		 * 
		 * @throw tc::ObjectDisposedException The base stream was not initialized.
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
		 * @throw tc::ObjectDisposedException The base stream was not initialized.
		 * @throw tc::io::IOException Data was not written to base stream successfully.
		 **/
	size_t pushData(const tc::ByteData& data, int64_t offset);
private:
	static const std::string kClassName;

	std::shared_ptr<tc::io::IStream> mBaseStream;
};

}} // namespace tc::io