#pragma once

typedef enum
{
	RSA_4096_SHA1 = 0x00010000,
	RSA_2048_SHA1 = 0x00010001,
	ECC_SHA1 = 0x00010002,
	RSA_4096_SHA256 = 0x00010003,
	RSA_2048_SHA256 = 0x00010004,
	ECC_SHA256 = 0x00010005
} ctr_sig_types;

typedef enum
{
	CTR_RSA_VERIFY,
	CTR_RSA_SIGN,
} ctr_rsa_mode;

typedef enum
{
	RSA_4096,
	RSA_2048,
	ECC,
	INVALID_SIG_TYPE,
} sig_types;

typedef enum
{
	CTR_SHA_1,
	CTR_SHA_256,
} ctr_sha_modes;

typedef enum
{
	SHA_1_LEN = 0x14,
	SHA_256_LEN = 0x20,
} sha_hash_len;

typedef enum
{
	RSA_4096_PUBK = 0,
	RSA_2048_PUBK,
	ECC_PUBK
} pubk_types;

typedef enum
{
	ENC,
	DEC
} aes_mode;


#ifdef __cplusplus
extern "C" {
#endif
// SHA
bool VerifySha256(void *data, u64 size, u8 hash[32]);
void ShaCalc(void *data, u64 size, u8 *hash, int mode);
// AES
void AesCtrCrypt(u8 *key, u8 *ctr, u8 *input, u8 *output, u64 length, u64 offset);
void AesCbcCrypt(u8 *key, u8 *iv, u8 *input, u8 *output, u64 length, u8 mode);
// RSA
int RsaSignVerify(void *data, u64 len, u8 *sign, u8 *mod, u8 *priv_exp, u32 sig_type, u8 rsa_mode);


					
#ifdef __cplusplus
}
#endif