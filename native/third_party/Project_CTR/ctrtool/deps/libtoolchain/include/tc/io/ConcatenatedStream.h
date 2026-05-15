	/**
	 * @file ConcatenatedStream.h
	 * @brief Declaration of tc::io::ConcatenatedStream
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2022/02/28
	 **/
#pragma once
#include <tc/io/IStream.h>
#include <tc/Optional.h>

#include <tc/ArgumentOutOfRangeException.h>
#include <tc/NotSupportedException.h>
#include <tc/NotImplementedException.h>
#include <tc/ObjectDisposedException.h>
#include <tc/io/IOException.h>

namespace tc { namespace io {

	/**
	 * @class ConcatenatedStream
	 * @brief A stream that concatenates multiple streams into a single stream.
	 **/
class ConcatenatedStream : public tc::io::IStream
{
public:
		/**
		 * @brief Default Constructor
		 * @post This will create an unusable ConcatenatedStream, it will have to be assigned from a valid ConcatenatedStream object to be usable.
		 **/
	ConcatenatedStream();

		/**
		 * @brief Move constructor
		 **/
	ConcatenatedStream(ConcatenatedStream&& other);

		/** 
		 * @brief Create ConcatenatedStream
		 * 
		 * @param[in] stream_list The list IStream objects to concatenate in this stream.
		 * 
		 * @pre All base streams must support seeking for @ref seek() to work
		 * @pre All base streams must support reading for @ref read() to work.
		 * @pre All base streams must support writing for @ref write() to work.
		 * 
		 * @throw tc::NotSupportedException List of streams did not all support either read or write.
		 * @throw tc::NotSupportedException List of streams combined to a stream with no length.
		 **/
	ConcatenatedStream(const std::vector<std::shared_ptr<tc::io::IStream>>& stream_list);

		/// Destructor
	~ConcatenatedStream();

		/**
		 * @brief Move assignment
		 **/
	ConcatenatedStream& operator=(ConcatenatedStream&& other);

		/**
		 * @brief Indicates whether the current stream supports reading.
		 **/ 
	bool canRead() const;

		/**
		 * @brief Indicates whether the current stream supports writing.
		 **/
	bool canWrite() const;

		/**
		 * @brief Indicates whether the current stream supports seeking.
		 **/
	bool canSeek() const;

		/**
		 * @brief Gets the length in bytes of the stream.
		 **/
	int64_t length();

		/** 
		 * @brief Gets the position within the current stream. 
		 * 
		 * @return This is returns the current position within the stream.
		 **/
	int64_t position();

		/**
		 * @brief Reads a sequence of bytes from the current stream and advances the position within the stream by the number of bytes read.
		 * 
		 * @param[out] ptr Pointer to an array of bytes. When this method returns, @p ptr contains the specified byte array with the values between 0 and (@p count - 1) replaced by the bytes read from the current source.
		 * @param[in] count The maximum number of bytes to be read from the current stream.
		 * 
		 * @return The total number of bytes read into @p ptr. This can be less than the number of bytes requested if that many bytes are not currently available, or zero (0) if the end of the stream has been reached.
		 * 
		 * @pre A stream must support reading for @ref read to work. 
		 * @note Use @ref canRead to determine if this stream supports reading.
		 * @note Exceptions thrown by the base stream are not altered/intercepted, refer to that module's documentation for those exceptions.
		 * 
		 * @throw tc::io::IOException Failed to read data from an underlying stream.
		 * @throw tc::NotSupportedException Stream does not support reading.
		 * @throw tc::ObjectDisposedException Methods were called after the stream was closed.
		 **/
	size_t read(byte_t* ptr, size_t count);

		/**
		 * @brief Writes a sequence of bytes to the current stream and advances the current position within this stream by the number of bytes written.
		 * 
		 * @param[in] ptr Pointer to an array of bytes. This method copies @p count bytes from @p ptr to the current stream.
		 * @param[in] count The number of bytes to be written to the current stream.
		 * 
		 * @return The total number of bytes written to the stream. This can be less than the number of bytes requested if that many bytes are not currently available, or zero (0) if the end of the stream has been reached.
		 * 
		 * @pre A stream must support writing for @ref write to work. 
		 * @note Use @ref canWrite to determine if this stream supports writing.
		 * @note Exceptions thrown by the base stream are not altered/intercepted, refer to that module's documentation for those exceptions.
		 * 
		 * @throw tc::io::IOException Failed to write data to an underlying stream.
		 * @throw tc::NotSupportedException Stream does not support writing.
		 * @throw tc::ObjectDisposedException Methods were called after the stream was closed.
		 **/
	size_t write(const byte_t* ptr, size_t count);

		/**
		 * @brief Sets the position within the current stream.
		 * 
		 * @param[in] offset A byte offset relative to the origin parameter.
		 * @param[in] origin A value of type @ref tc::io::SeekOrigin indicating the reference point used to obtain the new position.
		 * 
		 * @return The new position within the current stream.
		 * 
		 * @pre A stream must support seeking for @ref seek to work. 
		 * @note Use @ref canSeek to determine if this stream supports seeking.
		 * @note Exceptions thrown by the base stream are not altered/intercepted, refer to that module's documentation for those exceptions.
		 * 
		 * @throw tc::io::IOException Failed to seek because underlying stream could not be determined.
		 * @throw tc::ArgumentOutOfRangeException @p origin contains an invalid value.
		 * @throw tc::NotSupportedException Stream does not support seeking.
		 * @throw tc::ObjectDisposedException Methods were called after the stream was closed.
		 **/
	int64_t seek(int64_t offset, SeekOrigin origin);

		/**
		 * @brief Sets the length of the current stream. This is not implemented for @ref ConcatenatedStream.
		 * @throw tc::NotImplementedException @ref setLength is not implemented for @ref ConcatenatedStream
		 * @throw tc::ObjectDisposedException Methods were called after the stream was closed.
		 **/
	void setLength(int64_t length);

		/**
		 * @brief Clears all buffers for this and the base stream and causes any buffered data to be written to the underlying device.
		 * 
		 * @throw tc::io::IOException An I/O error occurs.
		 * @throw tc::ObjectDisposedException Methods were called after the stream was closed.
		 **/
	void flush();
	
		/**
		 * @brief Releases internal resources including base stream and clears internal state.
		 **/
	void dispose();
private:
	static const std::string kClassName;

	// delete copy constructor
	ConcatenatedStream(const ConcatenatedStream&);

	// delete copy assignment
	ConcatenatedStream& operator=(const ConcatenatedStream&);

	struct StreamRange
	{
		int64_t offset;
		int64_t length;

		StreamRange() : offset(0), length(0) {}
		StreamRange(int64_t offset) : offset(offset), length(0) {}
		StreamRange(int64_t offset, int64_t length) : offset(offset), length(length) {}

		bool operator<(const StreamRange& other) const 
		{
			return (this->offset < other.offset && (this->offset + this->length) <= other.offset);
		}
	};

	struct StreamInfo
	{
		StreamRange range;
		std::shared_ptr<tc::io::IStream> stream;
	};

	std::vector<StreamInfo> mStreamList;
	std::map<StreamRange, size_t> mStreamListMap;
	tc::Optional<std::vector<StreamInfo>::iterator> mCurrentStream;

	inline bool isStreamDisposed() const { return mStreamList.empty() || mCurrentStream.isNull() || mCurrentStream.get() == mStreamList.end(); }
	void updateCurrentStream(std::vector<StreamInfo>::iterator stream_itr);

	// static stream properties
	bool mCanRead;
	bool mCanWrite;
	bool mCanSeek;
	int64_t mStreamLength;
};

}} // namespace tc::io