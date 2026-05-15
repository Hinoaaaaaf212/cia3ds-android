	/**
	 * @file RsaKeyGeneratorImpl.h
	 * @brief Declaration of tc::crypto::detail::RsaKeyGeneratorImpl
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
	 * @class RsaKeyGeneratorImpl
	 * @brief This class implements the RSA key generation.
	 */
class RsaKeyGeneratorImpl
{
public:
		/**
		 * @brief Default constructor
		 * @details
		 * This initializes RSA key generator state.
		 */
	RsaKeyGeneratorImpl();

		/**
		 * @brief Destructor
		 * @details
		 * Cleans up RSA key generator state.
		 */
	~RsaKeyGeneratorImpl();

		/**
		 * @brief Generate an RSA key.
		 * 
		 * @param[in]  key_bit_size Size of rsa key in bits.
		 * @param[out] n      Buffer to store modulus.
		 * @param[in]  n_size Size of modulus buffer.
		 * @param[out] p      Buffer to store prime p.
		 * @param[in]  p_size Size of prime p buffer.
		 * @param[out] q      Buffer to store prime q.
		 * @param[in]  q_size Size of prime q buffer.
		 * @param[out] d      Buffer to store private exponent.
		 * @param[in]  d_size Size of private exponent buffer.
		 * @param[out] e      Buffer to store public exponent.
		 * @param[in]  e_size Size of public exponent buffer.
		 * 
		 * @pre 
		 * - @p key_bit_size must a multiple of 8 bits (byte aligned).
		 * @post
		 * - Key components are exported if the related buffers were not null.
		 * 
		 * @note
		 * - Key components can be optionally not exported if the corresponding input variables are null and zero.
		 * 
		 * @throw tc::ArgumentOutOfRangeException @p key_bit_size was not a multiple of 8 bits.
		 * @throw tc::crypto::CryptoException An unexpected error has occurred.
		 * @throw tc::crypto::CryptoException Something failed during generation of a key.
		 * @throw tc::crypto::CryptoException The random generator failed to generate non-zeros.
		 * @throw tc::ArgumentException @p n was not null, but @p n_size was not large enough.
		 * @throw tc::ArgumentException @p p was not null, but @p p_size was not large enough.
		 * @throw tc::ArgumentException @p q was not null, but @p q_size was not large enough.
		 * @throw tc::ArgumentException @p d was not null, but @p d_size was not large enough.
		 * @throw tc::ArgumentException @p e was not null, but @p e_size was not large enough.
		 */
	void generateKey(size_t key_bit_size, byte_t* n, size_t n_size, byte_t* p, size_t p_size, byte_t* q, size_t q_size, byte_t* d, size_t d_size, byte_t* e, size_t e_size);
private:
	static const std::string kClassName;	

	struct ImplCtx;
	std::unique_ptr<ImplCtx> mImplCtx;
};

}}} // namespace tc::crypto::detail