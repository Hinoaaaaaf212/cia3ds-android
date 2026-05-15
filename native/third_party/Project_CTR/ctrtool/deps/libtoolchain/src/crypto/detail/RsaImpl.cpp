#include <tc/crypto/detail/RsaImpl.h>
#include <mbedtls/rsa.h>

struct tc::crypto::detail::RsaImpl::ImplCtx
{
	mbedtls_rsa_context mContext;
};

tc::crypto::detail::RsaImpl::RsaImpl() :
	mState(State::None),
	mImplCtx(new ImplCtx())
{
	mbedtls_rsa_init(&(mImplCtx->mContext), MBEDTLS_RSA_PKCS_V15, 0);
}

tc::crypto::detail::RsaImpl::~RsaImpl()
{
	mbedtls_rsa_free(&(mImplCtx->mContext));
}

void tc::crypto::detail::RsaImpl::initialize(size_t key_bit_size, const byte_t* n, size_t n_size, const byte_t* p, size_t p_size, const byte_t* q, size_t q_size, const byte_t* d, size_t d_size, const byte_t* e, size_t e_size)
{
	if ((key_bit_size % 8) != 0) { throw tc::ArgumentOutOfRangeException("RsaImpl::initialize()", "key_bit_size was not a multiple of 8 bits."); }
	if (n == nullptr && n_size != 0) { throw tc::ArgumentNullException("RsaImpl::initialize()", "n was null when n_size was not 0."); }
	if (p == nullptr && p_size != 0) { throw tc::ArgumentNullException("RsaImpl::initialize()", "p was null when p_size was not 0."); }
	if (q == nullptr && q_size != 0) { throw tc::ArgumentNullException("RsaImpl::initialize()", "q was null when q_size was not 0."); }
	if (d == nullptr && d_size != 0) { throw tc::ArgumentNullException("RsaImpl::initialize()", "d was null when d_size was not 0."); }
	if (e == nullptr && e_size != 0) { throw tc::ArgumentNullException("RsaImpl::initialize()", "e was null when e_size was not 0."); }
	if (n != nullptr && n_size == 0) { throw tc::ArgumentNullException("RsaImpl::initialize()", "n was not null but n_size was 0."); }
	if (p != nullptr && p_size == 0) { throw tc::ArgumentNullException("RsaImpl::initialize()", "p was not null but p_size was 0."); }
	if (q != nullptr && q_size == 0) { throw tc::ArgumentNullException("RsaImpl::initialize()", "q was not null but q_size was 0."); }
	if (d != nullptr && d_size == 0) { throw tc::ArgumentNullException("RsaImpl::initialize()", "d was not null but d_size was 0."); }
	if (e != nullptr && e_size == 0) { throw tc::ArgumentNullException("RsaImpl::initialize()", "e was not null but e_size was 0."); }
	if (n_size > 0 && n_size != (key_bit_size/8)) { throw tc::ArgumentNullException("RsaImpl::initialize()", "n_size was non-zero but not expected size."); }
	if (p_size > 0 && p_size != (key_bit_size/8)/2) { throw tc::ArgumentNullException("RsaImpl::initialize()", "p_size was non-zero but not expected size."); }
	if (q_size > 0 && q_size != (key_bit_size/8)/2) { throw tc::ArgumentNullException("RsaImpl::initialize()", "q_size was non-zero but not expected size."); }
	if (d_size > 0 && d_size != (key_bit_size/8)) { throw tc::ArgumentNullException("RsaImpl::initialize()", "d_size was non-zero but not expected size."); }
	if (e_size > 0 && e_size != 3 && e_size != 4) { throw tc::ArgumentNullException("RsaImpl::initialize()", "e_size was non-zero but not expected size."); }

	mImplCtx->mContext.len = key_bit_size / 8;

	int ret = mbedtls_rsa_import_raw(&(mImplCtx->mContext), n, n_size, p, p_size, q, q_size, d, d_size, e, e_size);

	// TODO: Confirm these error codes
	if (ret != 0)
	{
		if (ret < MBEDTLS_ERR_RSA_BAD_INPUT_DATA) { throw tc::crypto::CryptoException("RsaImpl::initialize()", "Bad input parameters to function."); }
		else { throw tc::crypto::CryptoException("RsaImpl::initialize()", "An unexpected error occurred."); }
	}

	mState = State::Initialized;
}

void tc::crypto::detail::RsaImpl::publicTransform(byte_t* dst, const byte_t* src)
{
	if (mState != State::Initialized) { return; }
	if (dst == nullptr) { throw tc::ArgumentNullException("RsaImpl::publicTransform()", "dst was null."); }
	if (src == nullptr) { throw tc::ArgumentNullException("RsaImpl::publicTransform()", "src was null."); }

	int ret = mbedtls_rsa_public(&(mImplCtx->mContext), src, dst);
	
	switch (ret)
	{
		case (0):
			break;
		case (MBEDTLS_ERR_RSA_BAD_INPUT_DATA):
			throw tc::crypto::CryptoException("RsaImpl::publicTransform()", "Bad input parameters to function.");
			break;
		case (MBEDTLS_ERR_RSA_INVALID_PADDING):
			throw tc::crypto::CryptoException("RsaImpl::publicTransform()", "Input data contains invalid padding and is rejected.");
			break;
		case (MBEDTLS_ERR_RSA_KEY_GEN_FAILED):
			throw tc::crypto::CryptoException("RsaImpl::publicTransform()", "Something failed during generation of a key.");
			break;
		case (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED):
			throw tc::crypto::CryptoException("RsaImpl::publicTransform()", "Key failed to pass the validity check of the library.");
			break;
		case (MBEDTLS_ERR_RSA_PUBLIC_FAILED):
			throw tc::crypto::CryptoException("RsaImpl::publicTransform()", "The public key operation failed.");
			break;
		case (MBEDTLS_ERR_RSA_PRIVATE_FAILED):
			throw tc::crypto::CryptoException("RsaImpl::publicTransform()", "The private key operation failed.");
			break;
		case (MBEDTLS_ERR_RSA_VERIFY_FAILED):
			throw tc::crypto::CryptoException("RsaImpl::publicTransform()", "The PKCS#1 verification failed.");
			break;
		case (MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE):
			throw tc::crypto::CryptoException("RsaImpl::publicTransform()", "The output buffer for decryption is not large enough.");
			break;
		case (MBEDTLS_ERR_RSA_RNG_FAILED):
			throw tc::crypto::CryptoException("RsaImpl::publicTransform()", "The random generator failed to generate non-zeros.");
			break;
		default:
			throw tc::crypto::CryptoException("RsaImpl::publicTransform()", "An unexpected error occurred.");
	} 
}

void tc::crypto::detail::RsaImpl::privateTransform(byte_t* dst, const byte_t* src)
{
	if (mState != State::Initialized) { return; }
	if (dst == nullptr) { throw tc::ArgumentNullException("RsaImpl::privateTransform()", "dst was null."); }
	if (src == nullptr) { throw tc::ArgumentNullException("RsaImpl::privateTransform()", "src was null."); }

	int ret = mbedtls_rsa_private(&(mImplCtx->mContext), nullptr, nullptr, src, dst);

	switch (ret)
	{
		case (0):
			break;
		case (MBEDTLS_ERR_RSA_BAD_INPUT_DATA):
			throw tc::crypto::CryptoException("RsaImpl::privateTransform()", "Bad input parameters to function.");
			break;
		case (MBEDTLS_ERR_RSA_INVALID_PADDING):
			throw tc::crypto::CryptoException("RsaImpl::privateTransform()", "Input data contains invalid padding and is rejected.");
			break;
		case (MBEDTLS_ERR_RSA_KEY_GEN_FAILED):
			throw tc::crypto::CryptoException("RsaImpl::privateTransform()", "Something failed during generation of a key.");
			break;
		case (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED):
			throw tc::crypto::CryptoException("RsaImpl::privateTransform()", "Key failed to pass the validity check of the library.");
			break;
		case (MBEDTLS_ERR_RSA_PUBLIC_FAILED):
			throw tc::crypto::CryptoException("RsaImpl::privateTransform()", "The public key operation failed.");
			break;
		case (MBEDTLS_ERR_RSA_PRIVATE_FAILED):
			throw tc::crypto::CryptoException("RsaImpl::privateTransform()", "The private key operation failed.");
			break;
		case (MBEDTLS_ERR_RSA_VERIFY_FAILED):
			throw tc::crypto::CryptoException("RsaImpl::privateTransform()", "The PKCS#1 verification failed.");
			break;
		case (MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE):
			throw tc::crypto::CryptoException("RsaImpl::privateTransform()", "The output buffer for decryption is not large enough.");
			break;
		case (MBEDTLS_ERR_RSA_RNG_FAILED):
			throw tc::crypto::CryptoException("RsaImpl::privateTransform()", "The random generator failed to generate non-zeros.");
			break;
		default:
			throw tc::crypto::CryptoException("RsaImpl::privateTransform()", "An unexpected error occurred.");
	} 
}