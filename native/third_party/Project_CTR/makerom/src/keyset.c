#include "lib.h"
#include "aes_keygen.h"

// KeyData
#include "pki/test.h" // Test PKI
#include "pki/prod.h" // Production PKI
#include "pki/dev.h" // Development PKI

// Private Prototypes
int SetRsaKeySet(u8 **priv_exp_dst, const u8 *priv_exp_src, u8 **modulus_dst, const u8 *modulus_src);
void InitCommonKeySlots(keys_struct *keys);
void InitNcchKeyXSlots(keys_struct *keys);
int SetNcchKeyX(keys_struct *keys, const u8 *keyX, u8 index);

void keysetOpenError(char *file);
FILE* keyset_OpenFile(char *dir, char *name, bool FileRequired);

int SetCaCert(keys_struct *keys, const u8 *cert);
int SetTikCert(keys_struct *keys, const u8 *cert);
int SetTmdCert(keys_struct *keys, const u8 *cert);

int LoadKeysFromResources(keys_struct *keys);
void SetDummyRsaData(keys_struct *keys);
int LoadKeysFromKeyfile(keys_struct *keys);
void DumpKeyset(keys_struct *keys);



// Code
void InitKeys(keys_struct *keys)
{
	memset(keys,0,sizeof(keys_struct));
	InitCommonKeySlots(keys);
	InitNcchKeyXSlots(keys);
	Rsa2048Key_Alloc(&keys->rsa.xs);
	Rsa2048Key_Alloc(&keys->rsa.cp);
	Rsa2048Key_Alloc(&keys->rsa.cciCfa);
	Rsa2048Key_Alloc(&keys->rsa.acex);
	Rsa2048Key_Alloc(&keys->rsa.cxi);
	keys->aes.ncchKey0 = malloc(AES_128_KEY_SIZE);
	keys->aes.ncchKey1 = malloc(AES_128_KEY_SIZE);
}

void PrintBadKeySize(char *path, u32 size)
{
	fprintf(stderr,"[KEYSET ERROR] %s has invalid size (0x%x)\n",path,size);
}

int SetKeys(keys_struct *keys)
{	
	if (LoadKeysFromResources(keys) != 0)
	{
		return KEYSET_ERROR;
	}

	if (!keys->keysetLoaded)
	{
		return KEYSET_ERROR;
	}

	if (keys->dumpkeys)
	{
		DumpKeyset(keys);
	}
		
	return 0;
}

int LoadKeysFromResources(keys_struct *keys)
{
	if(keys->keyset == pki_TEST){
		keys->keysetLoaded = true;
		/* AES Keys */
		// CIA
		//SetCommonKey(keys, zeros_aesKey,1);
		if(keys->aes.currentCommonKey > MAX_CMN_KEY)
			SetCurrentCommonKey(keys,0);
	
		// NCCH
		SetNormalKey(keys,zeros_aesKey);
		SetSystemFixedKey(keys,zeros_aesKey);

		// CCI
		SetCciInitialDataKeyX(keys, zeros_aesKey);

		/* RSA Keys */
		// CIA
		Rsa2048Key_Set(&keys->rsa.xs, tpki_rsa.priv_exponent, tpki_rsa.modulus);
		Rsa2048Key_Set(&keys->rsa.cp, tpki_rsa.priv_exponent, tpki_rsa.modulus);
		// CCI/CFA
		Rsa2048Key_Set(&keys->rsa.cciCfa, tpki_rsa.priv_exponent, tpki_rsa.modulus);
		// CXI
		Rsa2048Key_Set(&keys->rsa.acex, tpki_rsa.priv_exponent, tpki_rsa.modulus);

		/* Certs */
		SetCaCert(keys, ca3_tpki_cert);
		SetTikCert(keys, xsC_tpki_cert);
		SetTmdCert(keys, cpB_tpki_cert);
	}
	else if(keys->keyset == pki_DEVELOPMENT){
		keys->keysetLoaded = true;
		/* AES Keys */
		// CIA
		for(int i = 0; i < 6; i++)
			SetCommonKey(keys, ctr_common_etd_key_dpki[i],i);

		if(keys->aes.currentCommonKey > MAX_CMN_KEY)
			SetCurrentCommonKey(keys,0);
	
		// NCCH
		SetNormalKey(keys, dev_fixed_ncch_key[0]);
		SetSystemFixedKey(keys, dev_fixed_ncch_key[1]);
		
		
		for(int i = 0; i < 4; i++)
			SetNcchKeyX(keys, dev_unfixed_ncch_keyX[i],i);
		
		// CCI
		SetCciInitialDataKeyX(keys, dev_initial_data_keyx);

		/* RSA Keys */
		// CIA
		Rsa2048Key_Set(&keys->rsa.xs, xs9_dpki_rsa.priv_exponent, xs9_dpki_rsa.modulus);
		Rsa2048Key_Set(&keys->rsa.cp, cpA_dpki_rsa.priv_exponent, cpA_dpki_rsa.modulus);
		// CCI/CFA
		Rsa2048Key_Set(&keys->rsa.cciCfa, dev_ncsd_cfa_rsa.priv_exponent, dev_ncsd_cfa_rsa.modulus);
		// CXI
		Rsa2048Key_Set(&keys->rsa.acex, dev_accessdesc_rsa.priv_exponent, dev_accessdesc_rsa.modulus);
	
		/* Certs */
		SetCaCert(keys, ca4_dpki_cert);
		SetTikCert(keys, xs9_dpki_cert);
		SetTmdCert(keys, cpA_dpki_cert);
	}
	else if(keys->keyset == pki_PRODUCTION){
		keys->keysetLoaded = true;
		/* AES Keys */
		// CIA
		for (int i = 0; i < 6; i++)
			SetCommonKey(keys, ctr_common_etd_key_ppki[i], i);

		if(keys->aes.currentCommonKey > MAX_CMN_KEY)
			SetCurrentCommonKey(keys,0);
	
		// NCCH
		keys->aes.normalKey = NULL;
		keys->aes.systemFixedKey = NULL;
		
		for(int i = 0; i < 4; i++)
			SetNcchKeyX(keys, prod_unfixed_ncch_keyX[i],i);
		
		// CCI
		SetCciInitialDataKeyX(keys, prod_initial_data_keyx);

		/* RSA Keys */
		// CIA
		Rsa2048Key_Set(&keys->rsa.xs, xsC_ppki_rsa.priv_exponent, xsC_ppki_rsa.modulus);
		Rsa2048Key_Set(&keys->rsa.cp, cpB_ppki_rsa.priv_exponent, cpB_ppki_rsa.modulus);
		// CCI/CFA
		Rsa2048Key_Set(&keys->rsa.cciCfa, prod_ncsd_cfa_rsa.priv_exponent, prod_ncsd_cfa_rsa.modulus);
		// CXI
		Rsa2048Key_Set(&keys->rsa.acex, prod_accessdesc_rsa.priv_exponent, prod_accessdesc_rsa.modulus);
	
		/* Certs */
		SetCaCert(keys, ca3_ppki_cert);
		SetTikCert(keys, xsC_ppki_cert);
		SetTmdCert(keys, cpB_ppki_cert);
	}
	return 0;
}

/*
void SetDummyRsaData(keys_struct *keys)
{
	// CIA
	if (Rsa2048Key_CanSign(&keys->rsa.xs) == false)
		Rsa2048Key_Set(&keys->rsa.xs, tpki_rsa.priv_exponent, tpki_rsa.modulus);
	if (Rsa2048Key_CanSign(&keys->rsa.cp) == false)
		Rsa2048Key_Set(&keys->rsa.cp, tpki_rsa.priv_exponent, tpki_rsa.modulus);
	// CCI/CFA
	if (Rsa2048Key_CanSign(&keys->rsa.cciCfa) == false)
		Rsa2048Key_Set(&keys->rsa.cciCfa, tpki_rsa.priv_exponent, tpki_rsa.modulus);
	// CXI
	if (Rsa2048Key_CanSign(&keys->rsa.acex) == false)
		Rsa2048Key_Set(&keys->rsa.acex, tpki_rsa.priv_exponent, tpki_rsa.modulus);

	// Certs
	if(!keys->certs.caCert)
		SetCaCert(keys, ca3_tpki_cert);
	if(!keys->certs.xsCert)
		SetTikCert(keys, xsC_tpki_cert);
	if(!keys->certs.cpCert)
		SetTmdCert(keys, cpB_tpki_cert);
}
*/

void DumpKeyset(keys_struct *keys)
{
	bool showNcchFixedKeys = (keys->aes.normalKey || keys->aes.systemFixedKey);
	bool showCommonKeys = false;
	bool showNcchKeyXs = false;
	for(int i = 0; i < 256; i++){
		if(keys->aes.commonKey[i]){
			showCommonKeys = true;
			break;
		}
	}

	for (int i = 0; i < 256; i++) {
		if (keys->aes.ncchKeyX[i]) {
			showNcchKeyXs = true;
			break;
		}
	}

	printf("[*] Keyset\n");
		
	if(showCommonKeys){
		printf(" > eTicket Common Keys\n");
		for(int i = 0; i < 256; i++){
			if(keys->aes.commonKey[i]){
				printf(" [0x%02x]     ",i);
				memdump(stdout,"",keys->aes.commonKey[i],16);
			}
		}
	}

	if (showNcchKeyXs) {
		printf(" > Unfixed NCCH KeyXs\n");
		for (int i = 0; i < 256; i++) {
			if (keys->aes.ncchKeyX[i]) {
				printf(" [0x%02x]     ", i);
				memdump(stdout, "", keys->aes.ncchKeyX[i], 16);
			}
		}
	}

	if(showNcchFixedKeys){
		printf(" > Fixed NCCH Keys\n");
		if(keys->aes.normalKey)
			memdump(stdout," [Normal]   ",keys->aes.normalKey,16);
		if(keys->aes.systemFixedKey)
			memdump(stdout," [System]   ",keys->aes.systemFixedKey,16);
	}

	printf(" > TIK RSA Keys\n");
	memdump(stdout," [PUB]      ",keys->rsa.xs.pub,0x100);
	memdump(stdout," [PVT]      ",keys->rsa.xs.pvt,0x100);
	printf(" > TMD RSA Keys\n");
	memdump(stdout," [PUB]      ",keys->rsa.cp.pub,0x100);
	memdump(stdout," [PVT]      ",keys->rsa.cp.pvt,0x100);
	printf(" > AcexDesc RSA Keys\n");
	memdump(stdout," [PUB]      ",keys->rsa.acex.pub,0x100);
	memdump(stdout," [PVT]      ",keys->rsa.acex.pvt,0x100);
	printf(" > NcsdCfa RSA Keys\n");
	memdump(stdout," [PUB]      ",keys->rsa.cciCfa.pub,0x100);
	memdump(stdout," [PVT]      ",keys->rsa.cciCfa.pvt,0x100);
}

void keysetOpenError(char *file)
{
	fprintf(stderr, "[KEYSET ERROR] Failed to open: %s\n", file);
}

FILE* keyset_OpenFile(char *dir, char *name, bool is_required)
{
	int file_path_len = sizeof(char)*(strlen(dir)+strlen(name)+1);
	char *file_path = malloc(file_path_len);
	memset(file_path,0,file_path_len);

	sprintf(file_path,"%s%s",dir,name);

	FILE *fp = fopen(file_path,"rb");
	
	if (!fp && is_required)
		keysetOpenError(file_path);

	free(file_path);
	return fp;
}



void FreeKeys(keys_struct *keys)
{
	// AES
	if(keys->aes.commonKey){
		for(int i = 0; i <= MAX_CMN_KEY; i++)
			free(keys->aes.commonKey[i]);
	}
	free(keys->aes.commonKey);
	free(keys->aes.normalKey);
	free(keys->aes.systemFixedKey);
	if(keys->aes.ncchKeyX){
		for(int i = 0; i <= MAX_NCCH_KEYX; i++)
			free(keys->aes.ncchKeyX[i]);
	}
	free(keys->aes.ncchKeyX);
	free(keys->aes.ncchKey0);
	free(keys->aes.ncchKey1);
	
	// RSA
	Rsa2048Key_Free(&keys->rsa.xs);
	Rsa2048Key_Free(&keys->rsa.cp);
	Rsa2048Key_Free(&keys->rsa.cciCfa);
	Rsa2048Key_Free(&keys->rsa.acex);
	Rsa2048Key_Free(&keys->rsa.cxi);
	
	// Certs
	free(keys->certs.caCert);
	free(keys->certs.xsCert);
	free(keys->certs.cpCert);
	memset(keys,0,sizeof(keys_struct));
}

int SetRsaKeySet(u8 **priv_exp_dst, const u8 *priv_exp_src, u8 **modulus_dst, const u8 *modulus_src)
{
	int result = 0;
	if(priv_exp_src){
		result = CopyData(priv_exp_dst,priv_exp_src,0x100);
		if(result) return result;
	}
	if(modulus_src){
		result = CopyData(modulus_dst,modulus_src,0x100);
		if(result) return result;
	}
	return 0;
}

int SetCommonKey(keys_struct *keys, const u8 *key, u8 index)
{
	if(!keys) return -1;
	return CopyData(&keys->aes.commonKey[index],key,AES_128_KEY_SIZE);
}

void InitCommonKeySlots(keys_struct *keys)
{
	if(!keys->aes.commonKey)
		keys->aes.commonKey = calloc(MAX_CMN_KEY+1,sizeof(u8*));
}

int SetNcchKeyX(keys_struct *keys, const u8 *keyX, u8 index)
{
	if(!keys) return -1;
	return CopyData(&keys->aes.ncchKeyX[index],keyX,AES_128_KEY_SIZE);
}

void InitNcchKeyXSlots(keys_struct *keys)
{
	if(!keys->aes.ncchKeyX)
		keys->aes.ncchKeyX = calloc(MAX_NCCH_KEYX+1,sizeof(u8*));
}

int SetCurrentCommonKey(keys_struct *keys, u8 Index)
{
	if(!keys) return -1;
	keys->aes.currentCommonKey = Index;
	return 0;
}

int SetNormalKey(keys_struct *keys, const u8 *key)
{
	if(!keys) return -1;
	return CopyData(&keys->aes.normalKey,key,16);
}

int SetSystemFixedKey(keys_struct *keys, const u8 *key)
{
	if(!keys) return -1;
	return CopyData(&keys->aes.systemFixedKey,key,16);
}

int SetCciInitialDataKeyX(keys_struct *keys, const u8 *key)
{
	if(!keys) return -1;
	return CopyData(&keys->aes.initialDataKeyX,key,16);
}

int SetCaCert(keys_struct *keys, const u8 *cert)
{
	if(!keys) return -1;
	return CopyData(&keys->certs.caCert,cert,0x400);
}
int SetTikCert(keys_struct *keys, const u8 *cert)
{
	if(!keys) return -1;
	return CopyData(&keys->certs.xsCert,cert,0x300);
}

int SetTmdCert(keys_struct *keys, const u8 *cert)
{
	if(!keys) return -1;
	return CopyData(&keys->certs.cpCert,cert,0x400);
}

void Rsa2048Key_Alloc(rsa2048_key* key)
{
	key->pub = malloc(RSA_2048_KEY_SIZE);
	key->pvt = malloc(RSA_2048_KEY_SIZE);
}

void Rsa2048Key_Free(rsa2048_key* key)
{
	free(key->pub);
	free(key->pvt);
}

void Rsa2048Key_Set(rsa2048_key* key, const u8* pvt, const u8* pub)
{
	memcpy(key->pub, pub, RSA_2048_KEY_SIZE);
	memcpy(key->pvt, pvt, RSA_2048_KEY_SIZE);
}

bool Rsa2048Key_CanSign(const rsa2048_key* key)
{
	static const u8 rsa2048[RSA_2048_KEY_SIZE] = { 0 };
	return memcmp(key->pub, rsa2048, RSA_2048_KEY_SIZE) != 0 && memcmp(key->pvt, rsa2048, RSA_2048_KEY_SIZE) != 0;
}