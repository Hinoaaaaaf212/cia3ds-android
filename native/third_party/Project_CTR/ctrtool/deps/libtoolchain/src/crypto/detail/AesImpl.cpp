#include <tc/crypto/detail/AesImpl.h>
#include <mbedtls/aes.h>

struct tc::crypto::detail::AesImpl::ImplCtx
{
	mbedtls_aes_context mEncContext;
	mbedtls_aes_context mDecContext;
};

tc::crypto::detail::AesImpl::AesImpl() :
	mState(State::None),
	mImplCtx(new ImplCtx())
{
	mbedtls_aes_init(&(mImplCtx->mEncContext));
	mbedtls_aes_init(&(mImplCtx->mDecContext));
}

tc::crypto::detail::AesImpl::~AesImpl()
{
	mbedtls_aes_free(&(mImplCtx->mEncContext));
	mbedtls_aes_free(&(mImplCtx->mDecContext));
}

void tc::crypto::detail::AesImpl::initialize(const byte_t* key, size_t key_size)
{
	if (key == nullptr) { throw tc::ArgumentNullException("AesImpl::initialize()", "key was null."); }
	if (key_size != 16 && key_size != 24 && key_size != 32) { throw tc::ArgumentOutOfRangeException("AesImpl::initialize()", "key_size did not equal 16, 24 or 32."); }

	mbedtls_aes_setkey_enc(&(mImplCtx->mEncContext), key, uint32_t(key_size) * 8);
	mbedtls_aes_setkey_dec(&(mImplCtx->mDecContext), key, uint32_t(key_size) * 8);

	mState = State::Initialized;
}

void tc::crypto::detail::AesImpl::encrypt(byte_t* dst, const byte_t* src)
{
	if (mState != State::Initialized) { return; }
	if (dst == nullptr) { throw tc::ArgumentNullException("AesImpl::encrypt()", "dst was null."); }
	if (src == nullptr) { throw tc::ArgumentNullException("AesImpl::encrypt()", "src was null."); }

	mbedtls_aes_crypt_ecb(&(mImplCtx->mEncContext), MBEDTLS_AES_ENCRYPT, src, dst);
}

void tc::crypto::detail::AesImpl::decrypt(byte_t* dst, const byte_t* src)
{
	if (mState != State::Initialized) { return; }
	if (dst == nullptr) { throw tc::ArgumentNullException("AesImpl::decrypt()", "dst was null."); }
	if (src == nullptr) { throw tc::ArgumentNullException("AesImpl::decrypt()", "src was null."); }

	mbedtls_aes_crypt_ecb(&(mImplCtx->mDecContext), MBEDTLS_AES_DECRYPT, src, dst);
}