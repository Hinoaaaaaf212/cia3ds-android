#include <tc/crypto/detail/Sha2Impl.h>
#include <mbedtls/md.h>

struct tc::crypto::detail::Sha2Impl::ImplCtx
{
	mbedtls_md_context_t mMdContext;
};

tc::crypto::detail::Sha2Impl::Sha2Impl(SHA2BitSize algo) :
	mState(State::None),
	mHashSize(0),
	mImplCtx(new ImplCtx())
{
	mbedtls_md_init(&(mImplCtx->mMdContext));
	switch(algo)
	{
		case (SHA2BitSize_256):
			mHashSize = kSha2_256_HashSize;
			mbedtls_md_setup(&(mImplCtx->mMdContext), mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
			break;
		case (SHA2BitSize_512):
			mHashSize = kSha2_512_HashSize;
			mbedtls_md_setup(&(mImplCtx->mMdContext), mbedtls_md_info_from_type(MBEDTLS_MD_SHA512), 0);
			break;
		default:
			throw tc::crypto::CryptoException("tc::crypto::detail::Sha2Impl", "Invalid value for SHA2BitSize");
	}
	
}

tc::crypto::detail::Sha2Impl::~Sha2Impl()
{
	mbedtls_md_free(&(mImplCtx->mMdContext));
}

void tc::crypto::detail::Sha2Impl::initialize()
{
	mbedtls_md_starts(&(mImplCtx->mMdContext));
	mState = State::Initialized;
}

void tc::crypto::detail::Sha2Impl::update(const byte_t* src, size_t src_size)
{
	mbedtls_md_update(&(mImplCtx->mMdContext), src, src_size);
}

void tc::crypto::detail::Sha2Impl::getHash(byte_t* hash)
{
	if (mState == State::Initialized)
	{
		mbedtls_md_finish(&(mImplCtx->mMdContext), mHash.data());
		mState = State::Done;
	}
	if (mState == State::Done)
	{
		memcpy(hash, mHash.data(), mHashSize);
	}	
}