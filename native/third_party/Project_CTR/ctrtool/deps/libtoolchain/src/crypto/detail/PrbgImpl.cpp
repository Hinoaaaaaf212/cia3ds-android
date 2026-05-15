#include <tc/crypto/detail/PrbgImpl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>

const std::string tc::crypto::detail::PrbgImpl::kClassName = "tc::crypto::detail::PrbgImpl";

struct tc::crypto::detail::PrbgImpl::ImplCtx
{
	mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_context entropy;
};

tc::crypto::detail::PrbgImpl::PrbgImpl() :
	mImplCtx(new ImplCtx())
{
	mbedtls_ctr_drbg_init(&(mImplCtx->ctr_drbg));
	mbedtls_entropy_init(&(mImplCtx->entropy));
	int ret = mbedtls_ctr_drbg_seed(&(mImplCtx->ctr_drbg), mbedtls_entropy_func, &(mImplCtx->entropy), (const unsigned char *)kClassName.c_str(), kClassName.size());
	switch (ret)
	{
		case (0):
			break;
		case (MBEDTLS_ERR_ENTROPY_SOURCE_FAILED):
			throw tc::crypto::CryptoException(kClassName, "Entropy source failed");
		default:
			throw tc::crypto::CryptoException(kClassName, "An unexpected error occurred");
	}
}

tc::crypto::detail::PrbgImpl::~PrbgImpl()
{
	mbedtls_ctr_drbg_free(&(mImplCtx->ctr_drbg));
	mbedtls_entropy_free(&(mImplCtx->entropy));
}

void tc::crypto::detail::PrbgImpl::getBytes(byte_t* data, size_t data_size)
{
	int ret = mbedtls_ctr_drbg_random(&(mImplCtx->ctr_drbg), data, data_size);
	switch (ret)
	{
		case (0):
			break;
		case (MBEDTLS_ERR_CTR_DRBG_REQUEST_TOO_BIG):
			throw tc::crypto::CryptoException(kClassName, "Request too big");
		case (MBEDTLS_ERR_ENTROPY_SOURCE_FAILED):
			throw tc::crypto::CryptoException(kClassName, "Entropy source failed");
		default:
			throw tc::crypto::CryptoException(kClassName, "An unexpected error occurred");
	}
}