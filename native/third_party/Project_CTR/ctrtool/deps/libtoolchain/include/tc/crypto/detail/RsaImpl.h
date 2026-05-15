	/**
	 * @file RsaImpl.h
	 * @brief Declaration of tc::crypto::detail::RsaImpl
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/09/12
	 **/
#pragma once
#include <tc/types.h>

#include <tc/ArgumentNullException.h>
#include <tc/ArgumentOutOfRangeException.h>
#include <tc/crypto/CryptoException.h>

namespace tc { namespace crypto { namespace detail {

	/**
	 * @class RsaImpl
	 * @brief This class implements the RSA algorithm.
	 */
class RsaImpl
{
public:
		/**
		 * @brief Default constructor.
		 * 
		 * @post
		 * - State is None. @ref initialize() must be called before use.
		 */
	RsaImpl();
	~RsaImpl();

		/**
		 * @brief Initialize RSA state with key.
		 * 
		 * @param[in] key_bit_size Size of rsa key in bits.
		 * @param[in] n Pointer to modulus data.
		 * @param[in] n_size Size in bytes of modulus data.
		 * @param[in] p Pointer to prime p data.
		 * @param[in] p_size Size in bytes of prime p data.
		 * @param[in] q Pointer to prime q data.
		 * @param[in] q_size Size in bytes of prime q data.
		 * @param[in] d Pointer to private exponent data.
		 * @param[in] d_size Size in bytes of private exponent data.
		 * @param[in] e Pointer to public exponent data.
		 * @param[in] e_size Size in bytes of public exponent data.
		 * 
		 * @pre 
		 * - @p key_bit_size must a multiple of 8 bits (byte aligned).
		 * @post
		 * - Instance is now in initialized state.
		 * 
		 * @throw tc::ArgumentNullException @p n was null when @p n_size was not 0.
		 * @throw tc::ArgumentNullException @p n was not null when @p n_size was 0.
		 * @throw tc::ArgumentNullException @p p was null when @p p_size was not 0.
		 * @throw tc::ArgumentNullException @p p was not null when @p p_size was 0.
		 * @throw tc::ArgumentNullException @p q was null when @p q_size was not 0.
		 * @throw tc::ArgumentNullException @p q was not null when @p q_size was 0.
		 * @throw tc::ArgumentNullException @p d was null when @p d_size was not 0.
		 * @throw tc::ArgumentNullException @p d was not null when @p d_size was 0.
		 * @throw tc::ArgumentNullException @p e was null when @p e_size was not 0.
		 * @throw tc::ArgumentNullException @p e was not null when @p e_size was 0.
		 * @throw tc::ArgumentOutOfRangeException @p key_bit_size was not a multiple of 8 bits.
		 */
	void initialize(size_t key_bit_size, const byte_t* n, size_t n_size, const byte_t* p, size_t p_size, const byte_t* q, size_t q_size, const byte_t* d, size_t d_size, const byte_t* e, size_t e_size);

		/**
		 * @brief Transform data block using public key.
		 * 
		 * @param[out] dst Buffer to store transformed block.
		 * @param[in]  src Pointer to block to transform.
		 * 
		 * @pre 
		 * - Instance is in initialized state.
		 * 
		 * @details
		 * This transforms block_size number of bytes of data from @p src, writing it to @p dst.
		 * 
		 * @note 
		 *  - @p dst and @p src can be the same pointer.
		 * 
		 * @throw tc::ArgumentNullException @p dst was null.
		 * @throw tc::ArgumentNullException @p src was null.
		 */
	void publicTransform(byte_t* dst, const byte_t* src);

		/**
		 * @brief Transform data block using private key.
		 * 
		 * @param[out] dst Buffer to store transformed block.
		 * @param[in]  src Pointer to block to transform.
		 * 
		 * @pre 
		 * - Instance is in initialized state.
		 * 
		 * @details
		 * This transforms block_size number of bytes of data from @p src, writing it to @p dst.
		 * 
		 * @note 
		 *  - @p dst and @p src can be the same pointer.
		 * 
		 * @throw tc::ArgumentNullException @p dst was null.
		 * @throw tc::ArgumentNullException @p src was null.
		 */
	void privateTransform(byte_t* dst, const byte_t* src);
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