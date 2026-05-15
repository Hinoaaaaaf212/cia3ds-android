	/**
	 * @file Pbkdf1KeyDeriver.h
	 * @brief Declaration of tc::crypto::Pbkdf1KeyDeriver
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/06/06
	 **/
#pragma once
#include <tc/types.h>
#include <tc/crypto/detail/Pbkdf1Impl.h>

namespace tc { namespace crypto {

	/**
	 * @class Pbkdf1KeyDeriver
	 * @brief Class for deriving a key using Password-Based Key Derivation Function 1 (PBKDF1).
	 * 
	 * @tparam HashFunction The class that implements the hash function used for key derivation.
	 * 
	 * @details
	 * PBKDF1 is a hash based key derivation function, as defined in RFC 8018.
	 * As such this template class requires @p HashFunction to implement one of the following hash functions to be compliant with RFC 8018.
	 * -# MD4
	 * -# MD5 (see @ref Md5Generator)
	 * -# SHA-1 (see @ref Sha1Generator)
	 * 
	 * The implementation of <var>HashFunction</var> must satisfies the following conditions.
	 * See @ref Sha1Generator or similar class, for more information including parameters to each function.
	 * 
	 * -# Has a <tt>kBlockSize</tt> constant that defines the size of the block to process.
	 * -# Has a <tt>kHashSize</tt> constant that defines the output size of the hash value.
	 * -# Has an <tt>initialize</tt> method that begins processing.
	 * -# Has an <tt>update</tt> method that updates the hash value on input.
	 * -# Has a <tt>getHash</tt> method that gets the final hash value.
	 * 
	 * This class has three states:
	 * - None : Not ready
	 * - Initialized : Ready to derive key data
	 * 
	 * General usage of this class is as follows:
	 * - Initialize PBKDF1 calculation with @ref initialize().
	 * - Derive key data with @ref getBytes().
	 */
template <class HashFunction>
class Pbkdf1KeyDeriver
{
public:
	static const uint64_t kMaxDerivableSize = HashFunction::kHashSize; /**< Maximum total key data that can be derived */

		/**
		 * @brief Default constructor
		 * 
		 * @post
		 * - State is None. @ref initialize() must be called before use.
		 */
	Pbkdf1KeyDeriver() :
		mImpl()
	{}

		/**
		 * @brief Initializes the PBKDF1 calculation.
		 * 
		 * @param[in] password Pointer to password.
		 * @param[in] password_size Size in bytes of password.
		 * @param[in] salt Pointer to salt.
		 * @param[in] salt_size Size in bytes of salt.
		 * @param[in] n_rounds Number of PBKDF1 rounds.
		 * 
		 * @pre
		 * - @p n_rounds >= 1 
		 * - @p salt is optional however the strength of the derived key is reduced if the salt is not sufficently random.
		 * 
		 * @post
		 * - Instance is now in an Initialized state.
		 * 
		 * @throw tc::crypto::CryptoException @p n_rounds was < 1
		 * 
		 * @details
		 * Resets the PBKDF1 calculation state to the begin state.
		 * 
		 * @note
		 * - This must be called before deriving new key data.
		 */
	void initialize(const byte_t* password, size_t password_size, const byte_t* salt, size_t salt_size, size_t n_rounds)
	{
		mImpl.initialize(password, password_size, salt, salt_size, n_rounds);
	}

		/**
		 * @brief Performs PBKDF1 calculation, deriving key data.
		 * 
		 * @param[out] key Pointer to the buffer storing the derived key.
		 * @param[in]  key_size Size of key to derive.
		 * 
		 * @pre
		 * - Instance is in an Initialized state.
		 * 
		 * @post
		 * - The derived key is written to <tt><var>key</var></tt>.
		 * 
		 * @throw tc::crypto::CryptoException @p key_size was too large.
		 * 
		 * @note 
		 * - This method can be called successively to continue deriving key data for up to @ref kMaxDerivableSize bytes.
		 * - If the instance is in a None state, then this call does nothing.
		 */ 
	void getBytes(byte_t* key, size_t key_size)
	{
		mImpl.getBytes(key, key_size);
	}

private:
	detail::Pbkdf1Impl<HashFunction> mImpl;
};

}} // namespace tc::crypto