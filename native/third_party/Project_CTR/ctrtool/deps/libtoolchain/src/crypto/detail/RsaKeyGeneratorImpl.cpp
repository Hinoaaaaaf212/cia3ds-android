#include <tc/crypto/detail/RsaKeyGeneratorImpl.h>
#include <mbedtls/rsa.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>

const std::string tc::crypto::detail::RsaKeyGeneratorImpl::kClassName = "tc::crypto::detail::RsaKeyGeneratorImpl";

struct tc::crypto::detail::RsaKeyGeneratorImpl::ImplCtx
{
	mbedtls_rsa_context rsa;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_entropy_context entropy;
};

tc::crypto::detail::RsaKeyGeneratorImpl::RsaKeyGeneratorImpl() :
	mImplCtx(new ImplCtx())
{
	mbedtls_entropy_init( &(mImplCtx->entropy) );
	mbedtls_ctr_drbg_init( &(mImplCtx->ctr_drbg) );
	mbedtls_rsa_init( &(mImplCtx->rsa), 0, 0 );

	int ret = mbedtls_ctr_drbg_seed(&(mImplCtx->ctr_drbg), mbedtls_entropy_func, &(mImplCtx->entropy), (const unsigned char *)kClassName.c_str(), kClassName.size());
	switch (ret)
	{
		case (0):
			break;
		case (MBEDTLS_ERR_ENTROPY_SOURCE_FAILED):
			throw tc::crypto::CryptoException(kClassName, "mbedtls_ctr_drbg_seed() Entropy source failed");
		default:
			throw tc::crypto::CryptoException(kClassName, "mbedtls_ctr_drbg_seed() An unexpected error occurred");
	}
}

tc::crypto::detail::RsaKeyGeneratorImpl::~RsaKeyGeneratorImpl()
{
	mbedtls_rsa_free( &(mImplCtx->rsa) );
	mbedtls_ctr_drbg_free( &(mImplCtx->ctr_drbg) );
	mbedtls_entropy_free( &(mImplCtx->entropy) );
}

void tc::crypto::detail::RsaKeyGeneratorImpl::generateKey(size_t key_bit_size, byte_t* n, size_t n_size, byte_t* p, size_t p_size, byte_t* q, size_t q_size, byte_t* d, size_t d_size, byte_t* e, size_t e_size)
{
	if ((key_bit_size % 8) != 0) { throw tc::ArgumentOutOfRangeException(kClassName, "key_bit_size was not a multiple of 8 bits."); }
	if (n != nullptr && n_size < (key_bit_size/8)) { throw tc::ArgumentNullException(kClassName, "n was not null, but n_size was not large enough"); }
	if (p != nullptr && p_size < (key_bit_size/8)/2) { throw tc::ArgumentNullException(kClassName, "p was not null, but p_size was not large enough"); }
	if (q != nullptr && q_size < (key_bit_size/8)/2) { throw tc::ArgumentNullException(kClassName, "q was not null, but q_size was not large enough"); }
	if (d != nullptr && d_size < (key_bit_size/8)) { throw tc::ArgumentNullException(kClassName, "d was not null, but d_size was not large enough"); }
	if (e != nullptr && e_size < 3) { throw tc::ArgumentNullException(kClassName, "e was not null, but e_size was not large enough"); }

	int ret = 1;
	
	// generate key
	ret = mbedtls_rsa_gen_key(&(mImplCtx->rsa), mbedtls_ctr_drbg_random, &(mImplCtx->ctr_drbg), uint32_t(key_bit_size), 0x10001);
	switch (ret)
	{
		case (0):
			break;
		case (MBEDTLS_ERR_RSA_KEY_GEN_FAILED):
			throw tc::crypto::CryptoException(kClassName, "mbedtls_rsa_gen_key() Something failed during generation of a key.");
		case (MBEDTLS_ERR_RSA_RNG_FAILED):
			throw tc::crypto::CryptoException(kClassName, "mbedtls_rsa_gen_key() The random generator failed to generate non-zeros.");
		default:
			throw tc::crypto::CryptoException(kClassName, "mbedtls_rsa_gen_key() An unexpected error occurred.");
	}
	
	// export key from mbedtls context
	ret = mbedtls_rsa_export_raw(&(mImplCtx->rsa), \
		n, n_size, \
		p, p_size, \
		q, q_size, \
		d, d_size, \
		e, e_size \
	);

	switch (ret)
	{
		case (0):
			break;
		default:
			throw tc::crypto::CryptoException(kClassName, "mbedtls_rsa_export_raw() An unexpected error occurred.");
	}

	// clear key from mbedtls context
	mbedtls_rsa_init( &(mImplCtx->rsa), 0, 0 );
}