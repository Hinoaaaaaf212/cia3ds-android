	/**
	 * @file Sha512Generator.h
	 * @brief Declarations for API resources for SHA2-512 calculations.
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2020/06/01
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/detail/Sha2Impl.h>

namespace tc { namespace crypto {

	/**
	 * @class Sha512Generator
	 * @brief Class for calculating SHA2-512 hash.
	 * 
	 * @details
	 * This class has three states:
	 * - None : Not ready
	 * - Initialized : Ready to process input data
	 * - Done : Hash value is calculated
	 * 
	 * General usage of this class is as follows:
	 * - Initialize Hash Generator state with @ref initialize().
	 * - Update hash value with input data with @ref update().
	 * - Complete hash calculation and export hash value with @ref getHash().
	 * 
	 * Below is code sample for calculating hash value with one call to @ref update():
	 * @code
	 * // open file stream
	 * auto stream = tc::io::FileStream("a_file.bin", tc::io::FileMode::Open, tc::io::FileAccess::Read);
	 * 
	 * // create array to store hash value
	 * std::array<byte_t, tc::crypto::Sha512Generator::kHashSize> hash;
	 * 
	 * // initialize generator. Sha512Generator is now in a ready state. 
	 * tc::crypto::Sha512Generator impl;
	 * impl.initialize();
	 * 
	 * // reset stream position to beginning (not strictly necessary for an unused tc::io::FileStream)
	 * stream.seek(0, tc::io::SeekOrigin::Begin);
	 * 
	 * // read whole file into memory. This is unsafe for large file sizes especially on 32-bit systems.
	 * tc::ByteData data = tc::ByteData((size_t)stream.length());
	 * stream.read(data.data(), data.size());
	 * 
	 * // update generator state with stream data
	 * impl.update(data.data(), data.size());
	 * 
	 * // complete generator state and write hash value to hash
	 * impl.getHash(hash.data());
	 * @endcode 
	 * 
	 * Below is code sample for calculating hash value with sequential calls to @ref update():
	 * @code
	 * // open file stream
	 * auto stream = tc::io::FileStream("a_file.bin", tc::io::FileMode::Open, tc::io::FileAccess::Read);
	 * 
	 * // create read block (size 512)
	 * static const size_t kReadBlockSize = 0x200;
	 * std::array<byte_t, kReadBlockSize> block;
	 * 
	 * // create array to store hash value
	 * std::array<byte_t, tc::crypto::Sha512Generator::kHashSize> hash;
	 * 
	 * // initialize generator. Sha512Generator is now in a ready state. 
	 * tc::crypto::Sha512Generator impl;
	 * impl.initialize();
	 * 
	 * // reset stream position to beginning (not strictly necessary for an unused tc::io::FileStream)
	 * stream.seek(0, tc::io::SeekOrigin::Begin);
	 * 
	 * // iterate over blocks in stream until no more data can be read
	 * size_t read_count = 0;
	 * while ( 0 != (read_count = tc::io::IOUtil::getReadableCount(stream.position(), stream.length(), block.size())) )
	 * {
	 *   // read block from stream
	 *   stream.read(block.data(), read_count);
	 * 
	 *   // update generator state with stream data
	 *   impl.update(block.data(), read_count);
	 * }
	 * 
	 * // complete generator state and write hash value to hash
	 * impl.getHash(hash.data());
	 * @endcode 
	 */
class Sha512Generator
{
public:
	static const size_t kAsn1OidDataSize = 19; /**< SHA2-512 ASN.1 Encoded OID length */
	static const std::array<byte_t, kAsn1OidDataSize> kAsn1OidData; /**< SHA2-512 ASN.1 Encoded OID */

	static const size_t kHashSize  = 64; /**< SHA2-512 hash size */
	static const size_t kBlockSize = 128; /**< SHA2-512 processing block size */

		/**
		 * @brief Default constructor.
		 * 
		 * @post
		 * - State is None. @ref initialize() must be called before use.
		 */
	Sha512Generator() :
		mImpl(detail::Sha2Impl::SHA2BitSize_512)
	{}

		/**
		 * @brief Initializes the hash calculation.
		 * 
		 * @post
		 * - Instance is now in a Initialized state
		 * 
		 * @details
		 * Resets the hash calculation state to the begin state.
		 * 
		 * @note
		 * - This must be called before calculating a new hash.
		 */
	void initialize()
	{
		mImpl.initialize();
	}

		/**
		 * @brief Update hash value with specified data.
		 * 
		 * @param[in] data Pointer to input data.
		 * @param[in] data_size Size of input data.
		 * 
		 * @details
		 * Data can be input to the generator in one @ref update() call or split across multiple sequential calls.
		 * 
		 * For example the following scenarios all generate the same hash value.
		 * @code
		 * // generate data to be hashed
		 * tc::ByteData data = tc::ByteData(0x30);
		 * memset(data.data(), 0xff, data.size());
		 * 
		 * // create generator instance
		 * tc::crypto::Sha512Generator impl;
		 * 
		 * // scenario 1 (one call to update() 0x30 bytes, totaling 0x30 bytes inputted)
		 * std::array<byte_t, tc::crypto::Sha512Generator::kHashSize> hash1;
		 * impl.initialize();
		 * impl.update(data.data(), data.size());
		 * impl.getHash(hash1.data());
		 * 
		 * // scenario 2 (three calls to update() 0x10 bytes each, totaling 0x30 bytes inputted)
		 * std::array<byte_t, tc::crypto::Sha512Generator::kHashSize> hash2;
		 * impl.initialize();
		 * impl.update(data.data() + 0x00, 0x10);
		 * impl.update(data.data() + 0x10, 0x10);
		 * impl.update(data.data() + 0x20, 0x10);
		 * impl.getHash(hash2.data());
		 * 
		 * // scenario 3 (two calls to update() one 0x10 bytes, the second 0x20 bytes, totaling 0x30 bytes inputted)
		 * std::array<byte_t, tc::crypto::Sha512Generator::kHashSize> hash3;
		 * impl.initialize();
		 * impl.update(data.data() + 0x00, 0x10);
		 * impl.update(data.data() + 0x10, 0x20);
		 * impl.getHash(hash3.data());
		 * @endcode
		 * 
		 * @note 
		 * - If input data is broken up into blocks and supplied via multiple @ref update() calls, the order must be consistent with the original input data.
		 */
	void update(const byte_t* data, size_t data_size)
	{
		mImpl.update(data, data_size);
	}

		/**
		 * @brief Completes hash calculation and output hash value.
		 * 
		 * @param[out] hash Pointer to buffer storing hash value.
		 * 
		 * @pre
		 * - Instance is in either Initialized or Done state.
		 * - The size of the <tt><var>hash</var></tt> buffer must be >= @ref kHashSize.
		 * 
		 * @post
		 * - Instance is now in a Done state.
		 * - The calculated hash value is written to <tt><var>hash</var></tt>.
		 * 
		 * @note 
		 * - If the instance is in a None state, then this call does nothing.
		 */ 
	void getHash(byte_t* hash)
	{
		mImpl.getHash(hash);
	}

private:
	detail::Sha2Impl mImpl;
};

	/**
	 * @brief Utility function for calculating the SHA2-512 hash.
	 * 
	 * @param[out] hash Pointer to buffer storing hash value.
	 * @param[in] data Pointer to input data.
	 * @param[in] data_size Size of input data.
	 * 
	 * @pre
	 * - The size of the <tt><var>hash</var></tt> buffer must be >= @ref Sha512Generator::kHashSize.
	 * 
	 * @post
	 * - The calculated hash value is written to <tt><var>hash</var></tt>.
	 * 
	 * @details
	 * This function calculates the hash value for input passed in the <tt><var>data</var></tt> array.
	 * To calculate the hash value for input split across multiple arrays, use the @ref Sha512Generator class.
	 */
void GenerateSha512Hash(byte_t* hash, const byte_t* data, size_t data_size);

}} // namespace tc::crypto