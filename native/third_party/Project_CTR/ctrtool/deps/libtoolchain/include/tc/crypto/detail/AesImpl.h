	/**
	 * @file AesImpl.h
	 * @brief Declaration of tc::crypto::detail::AesImpl
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/07/04
	 **/
#pragma once
#include <tc/types.h>

#include <tc/ArgumentNullException.h>
#include <tc/ArgumentOutOfRangeException.h>

namespace tc { namespace crypto { namespace detail {

	/**
	 * @class AesImpl
	 * @brief This class implements the AES block encryption algorithm.
	 */
class AesImpl
{
public:
	static const size_t kBlockSize = 16; /**< AES processing block size. */

		/**
		 * @brief Default constructor.
		 * 
		 * @post
		 * - State is None. @ref initialize() must be called before use.
		 */
	AesImpl();
	~AesImpl();

		/**
		 * @brief Initialize AES state with key.
		 * 
		 * @param[in] key Pointer to key data.
		 * @param[in] key_size Size in bytes of key data.
		 * 
		 * @pre 
		 * - @p key_size must be 16, 24 or 32.
		 * @post
		 * - Instance is now in initialized state.
		 * 
		 * @throw tc::ArgumentNullException @p key was null.
		 * @throw tc::ArgumentOutOfRangeException @p key_size did not equal 16, 24 or 32.
		 */
	void initialize(const byte_t* key, size_t key_size);

		/**
		 * @brief Encrypt data block.
		 * 
		 * @param[out] dst Buffer to store encrypted block.
		 * @param[in]  src Pointer to block to encrypt.
		 * 
		 * @pre 
		 * - Instance is in initialized state.
		 * 
		 * @details
		 * This encrypts @ref kBlockSize number of bytes of data from @p src, writing it to @p dst.
		 * 
		 * @note 
		 *  - @p dst and @p src can be the same pointer.
		 * 
		 * @throw tc::ArgumentNullException @p dst was null.
		 * @throw tc::ArgumentNullException @p src was null.
		 */
	void encrypt(byte_t* dst, const byte_t* src);

		/**
		 * @brief Decrypt data block.
		 * 
		 * @param[out] dst Buffer to store decrypted block.
		 * @param[in]  src Pointer to block to decrypt.
		 * 
		 * @pre 
		 * - Instance is in initialized state.
		 * 
		 * @details
		 * This decrypts @ref kBlockSize number of bytes of data from @p src, writing it to @p dst.
		 * 
		 * @note 
		 *  - @p dst and @p src can be the same pointer.
		 * 
		 * @throw tc::ArgumentNullException @p dst was null.
		 * @throw tc::ArgumentNullException @p src was null.
		 */
	void decrypt(byte_t* dst, const byte_t* src);
private:
	enum class State
	{
		None,
		Initialized
	};

	State mState;

	struct ImplCtx;
	std::unique_ptr<ImplCtx> mImplCtx;
};

}}} // namespace tc::crypto::detail