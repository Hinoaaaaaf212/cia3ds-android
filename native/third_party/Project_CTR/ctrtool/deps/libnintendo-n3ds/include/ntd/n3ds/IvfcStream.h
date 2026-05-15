#pragma once
#include <tc/ByteData.h>
#include <tc/io/IStream.h>
#include <tc/io/IOUtil.h>
#include <tc/crypto/Sha256Generator.h>
#include <tc/crypto/CryptoException.h>

#include <tc/ArgumentOutOfRangeException.h>
#include <tc/ObjectDisposedException.h>

namespace ntd { namespace n3ds {

class IvfcStream : public tc::io::IStream
{
public:
	IvfcStream();
	IvfcStream(const std::shared_ptr<tc::io::IStream>& stream);

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
		 * @throw tc::ArgumentOutOfRangeException @p count exceeds the length of readable data in the sub stream.
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
		 * @throw tc::ArgumentOutOfRangeException @p count exceeds the length of writeable data in the sub stream.
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
		 * @throw tc::ArgumentOutOfRangeException @p origin contains an invalid value.
		 * @throw tc::ObjectDisposedException Methods were called after the stream was closed.
		 **/
	int64_t seek(int64_t offset, tc::io::SeekOrigin origin);

		/**
		 * @brief Sets the length of the current stream. This is not implemented for @ref SubStream.
		 * @throw tc::NotImplementedException @ref setLength is not implemented for @ref SubStream
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
	std::string mModuleLabel;

	// base stream
	std::shared_ptr<tc::io::IStream> mBaseStream;

	// data layer stream
	size_t mDataStreamBlockSize;
	int64_t mDataStreamLogicalLength;
	std::shared_ptr<tc::io::IStream> mDataStream;
	inline size_t offsetToBlock(int64_t offset) { return tc::io::IOUtil::castInt64ToSize(offset / tc::io::IOUtil::castSizeToInt64(mDataStreamBlockSize)); };
	inline size_t offsetInBlock(int64_t offset) { return tc::io::IOUtil::castInt64ToSize(offset % tc::io::IOUtil::castSizeToInt64(mDataStreamBlockSize)); };
	inline int64_t blockToOffset(size_t block) { return tc::io::IOUtil::castSizeToInt64(block) * tc::io::IOUtil::castSizeToInt64(mDataStreamBlockSize); };

	// hash cache for data layer
	tc::ByteData mHashCache;
	inline byte_t* getBlockHash(size_t begin_block) { return mHashCache.data() + (begin_block * tc::crypto::Sha256Generator::kHashSize); }

	// hash calc temp
	std::array<byte_t, tc::crypto::Sha256Generator::kHashSize> mHash;
	tc::crypto::Sha256Generator mHashCalc;

	bool validateLayerBlocksWithHashLayer(const byte_t* layer, size_t block_size, size_t block_num, const byte_t* hash_layer);
};

}} // namespace ntd::n3ds