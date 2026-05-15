	/**
	 * @file    Aes128CbcEncryptedStream.h
	 * @brief   Declaration of tc::crypto::Aes128CbcEncryptedStream
	 * @author  Jack (jakcron)
	 * @version 0.2
	 * @date    2022/02/13
	 **/
#pragma once
#include <list>
#include <tc/ByteData.h>
#include <tc/io/IStream.h>
#include <tc/crypto/Aes128CbcEncryptor.h>

#include <tc/ArgumentOutOfRangeException.h>
#include <tc/ObjectDisposedException.h>
#include <tc/NotSupportedException.h>
#include <tc/NotImplementedException.h>
#include <tc/io/IOException.h>

namespace tc { namespace crypto {

	/**
	 * @class Aes128CbcEncryptedStream
	 * @brief Class for reading from a stream that is encrypted with AES128-CBC.
	 * @details This class takes an encrypted IStream, encryption parameters and creates an IStream that will transparently decrypt data when reading.
	 */
class Aes128CbcEncryptedStream : public tc::io::IStream
{
public:
		/**
		 * @brief This is the data-type for AES128-CBC key used with Aes128CbcEncryptedStream.
		 * 
		 */
	using key_t = std::array<byte_t, tc::crypto::Aes128CbcEncryptor::kKeySize>;

		/**
		 * @brief This is the data-type for AES128-CBC initialization vector used with Aes128CbcEncryptedStream.
		 * 
		 */
	using iv_t = std::array<byte_t, tc::crypto::Aes128CbcEncryptor::kBlockSize>;

		/**
		 * @brief This is the data-type for AES128-CBC data block used with Aes128CbcEncryptedStream.
		 * 
		 */
	using block_t = std::array<byte_t, tc::crypto::Aes128CbcEncryptor::kBlockSize>;

		/**
		 * @brief Default Constructor
		 * @post This will create an uninitialized Aes128CbcEncryptedStream, it will have to be assigned from a valid Aes128CbcEncryptedStream object to be usable.
		 **/
	Aes128CbcEncryptedStream();

		/** 
		 * @brief Create Aes128CbcEncryptedStream
		 * 
		 * @param[in] stream  The base IStream object which this stream will decrypt data from.
		 * @param[in] key     AES128 Encryption Key. See @ref Aes128CbcEncryptedStream::key_t.
		 * @param[in] iv      AES128-CBC Initilization vector relative to offset 0x0 of the base stream. See @ref Aes128CbcEncryptedStream::iv_t.
		 * 
		 * @pre The sub stream must be a subset of the base stream.
		 * @pre A stream must support seeking for @ref seek to work. 
		 * 
		 * @throw tc::ArgumentNullException @p stream is a @p nullptr.
		 * @throw tc::NotSupportedException The base stream does not support seeking or reading.
		 * @throw tc::NotSupportedException The base stream is not block aligned.
		 **/
	Aes128CbcEncryptedStream(const std::shared_ptr<tc::io::IStream>& stream, const key_t& key, const iv_t& iv);

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
		 * @brief Writes a sequence of bytes to the current stream and advances the current position within this stream by the number of bytes written. @ref write is not implemented for @ref Aes128CbcEncryptedStream.
		 * @throw tc::NotImplementedException @ref write is not implemented for @ref Aes128CbcEncryptedStream.
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
		 * @brief Sets the length of the current stream. This is not implemented for @ref Aes128CbcEncryptedStream.
		 * @throw tc::NotImplementedException @ref setLength is not implemented for @ref Aes128CbcEncryptedStream.
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

	// base source
	std::shared_ptr<tc::io::IStream> mBaseStream;

	// encryption cfg
	std::shared_ptr<tc::crypto::Aes128CbcEncryptor> mCryptor;
	iv_t mBaseIv;
};

}} // namespace tc::crypto