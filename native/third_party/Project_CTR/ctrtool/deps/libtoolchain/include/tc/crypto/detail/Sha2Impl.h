	/**
	 * @file Sha2Impl.h
	 * @brief Declaration of tc::crypto::detail::Sha2Impl
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2022/02/27
	 **/
#pragma once
#include <tc/types.h>

#include <tc/crypto/CryptoException.h>

namespace tc { namespace crypto { namespace detail {

	/**
	 * @class Sha2Impl
	 * @brief This class implements the SHA2 family of hash algorithms.
	 */
class Sha2Impl
{
public:
	enum SHA2BitSize
	{
		SHA2BitSize_256 = 256,
		SHA2BitSize_512 = 512
	};

	static const size_t kSha2_256_HashSize = 32;
	static const size_t kSha2_256_BlockSize = 64;

	static const size_t kSha2_512_HashSize = 64;
	static const size_t kSha2_512_BlockSize = 128;

	Sha2Impl(SHA2BitSize algo = SHA2BitSize_256);
	~Sha2Impl();

	void initialize();
	void update(const byte_t* data, size_t data_size);
	void getHash(byte_t* hash);
private:
	enum class State
	{
		None,
		Initialized,
		Done
	};

	State mState;

	size_t mHashSize;
	std::array<byte_t, kSha2_512_HashSize> mHash;

	struct ImplCtx;
	std::unique_ptr<ImplCtx> mImplCtx;
};

}}} // namespace tc::crypto::detail