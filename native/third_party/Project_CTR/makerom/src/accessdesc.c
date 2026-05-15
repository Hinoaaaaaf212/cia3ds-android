#include "lib.h"
#include "ncch_build.h"
#include "exheader_build.h"
#include "accessdesc.h"

#include "desc/presets.h"
#include "desc/dev_sigdata.h"

const size_t RSF_RSA_DATA_LEN = 344;
const size_t RSF_DESC_DATA_LEN = 684;


int accessdesc_SignWithKey(exheader_settings *exhdrset);
int accessdesc_GetSignFromRsf(exheader_settings *exhdrset);
int accessdesc_GetSignFromPreset(exheader_settings *exhdrset);
const CtrSdkDesc* accessdesc_GetPresetData(keys_struct *keys);
const CtrSdkDescSignData* accessdesc_GetPresetSignData(keys_struct *keys);
const CtrSdkDepList* accessdesc_GetPresetDependencyList(keys_struct *keys);

int set_AccessDesc(exheader_settings *exhdrset)
{
	if(exhdrset->useAccessDescPreset == true) // Use AccessDesc Template
		return accessdesc_GetSignFromPreset(exhdrset);
	else if(exhdrset->rsf->CommonHeaderKey.Found == true) // Keydata exists in RSF
		return accessdesc_GetSignFromRsf(exhdrset);
	return accessdesc_SignWithKey(exhdrset);	
}

int accessdesc_SignWithKey(exheader_settings *exhdrset)
{
	/* Set RSA Keys */
	memcpy(exhdrset->keys->rsa.cxi.pvt, exhdrset->keys->rsa.cciCfa.pvt, 0x100);
	memcpy(exhdrset->keys->rsa.cxi.pub, exhdrset->keys->rsa.cciCfa.pub, 0x100);
	memcpy(&exhdrset->acexDesc->ncchRsaPubKey, exhdrset->keys->rsa.cxi.pub, 0x100);

	/* Copy Data From ExHeader */
	memcpy(&exhdrset->acexDesc->arm11SystemLocalCapabilities, &exhdrset->exHdr->arm11SystemLocalCapabilities, sizeof(exhdr_ARM11SystemLocalCapabilities));
	memcpy(&exhdrset->acexDesc->arm11KernelCapabilities, &exhdrset->exHdr->arm11KernelCapabilities, sizeof(exhdr_ARM11KernelCapabilities));
	memcpy(&exhdrset->acexDesc->arm9AccessControlInfo, &exhdrset->exHdr->arm9AccessControlInfo, sizeof(exhdr_ARM9AccessControlInfo));

	/* Adjust Data */
	exhdr_ARM11SystemLocalCapabilities *arm11 = &exhdrset->acexDesc->arm11SystemLocalCapabilities;

	arm11->idealProcessor = 1 << arm11->idealProcessor;
	arm11->threadPriority /= 2;

	/* Sign AccessDesc */
	if (Rsa2048Key_CanSign(&exhdrset->keys->rsa.acex) == false)
	{
		printf("[ACEXDESC WARNING] Failed to sign access descriptor (key was incomplete)\n");
		memset(exhdrset->acexDesc->signature, 0xFF, 0x100);
		return 0;
	}

	int rsa_ret = SignAccessDesc(exhdrset->acexDesc, exhdrset->keys);
	if (rsa_ret != 0)
	{
		printf("[ACEXDESC WARNING] Failed to sign access descriptor (mbedtls error = -0x%x)\n", -rsa_ret);
		memset(exhdrset->acexDesc->signature, 0xFF, 0x100);
		return 0;
	}

	return 0;
}

int accessdesc_GetSignFromRsf(exheader_settings *exhdrset)
{
	/* Yaml Option Sanity Checks */
	if(!exhdrset->rsf->CommonHeaderKey.Found){
		fprintf(stderr,"[ACEXDESC ERROR] RSF Section \"CommonHeaderKey\" not found\n");
		return COMMON_HEADER_KEY_NOT_FOUND;
	}

	if(!exhdrset->rsf->CommonHeaderKey.D){
		ErrorParamNotFound("CommonHeaderKey/D");
		return COMMON_HEADER_KEY_NOT_FOUND;
	}
	if(b64_strlen(exhdrset->rsf->CommonHeaderKey.D) != RSF_RSA_DATA_LEN){
		fprintf(stderr,"[ACEXDESC ERROR] \"CommonHeaderKey/D\" has invalid length (%d)\n", (int)b64_strlen(exhdrset->rsf->CommonHeaderKey.D));
		return COMMON_HEADER_KEY_NOT_FOUND;
	}

	if(!exhdrset->rsf->CommonHeaderKey.Modulus){
		ErrorParamNotFound("CommonHeaderKey/Modulus");
		return COMMON_HEADER_KEY_NOT_FOUND;
	}
	if(b64_strlen(exhdrset->rsf->CommonHeaderKey.Modulus) != RSF_RSA_DATA_LEN){
		fprintf(stderr,"[ACEXDESC ERROR] \"CommonHeaderKey/Modulus\" has invalid length (%d)\n", (int)b64_strlen(exhdrset->rsf->CommonHeaderKey.Modulus));
		return COMMON_HEADER_KEY_NOT_FOUND;
	}

	if(!exhdrset->rsf->CommonHeaderKey.AccCtlDescSign){
		ErrorParamNotFound("CommonHeaderKey/Signature");
		return COMMON_HEADER_KEY_NOT_FOUND;
	}
	if(b64_strlen(exhdrset->rsf->CommonHeaderKey.AccCtlDescSign) != RSF_RSA_DATA_LEN){
		fprintf(stderr,"[ACEXDESC ERROR] \"CommonHeaderKey/Signature\" has invalid length (%d)\n", (int)b64_strlen(exhdrset->rsf->CommonHeaderKey.AccCtlDescSign));
		return COMMON_HEADER_KEY_NOT_FOUND;
	}

	if(!exhdrset->rsf->CommonHeaderKey.AccCtlDescBin){
		ErrorParamNotFound("CommonHeaderKey/Descriptor");
		return COMMON_HEADER_KEY_NOT_FOUND;
	}
	if(b64_strlen(exhdrset->rsf->CommonHeaderKey.AccCtlDescBin) != RSF_DESC_DATA_LEN){
		fprintf(stderr,"[ACEXDESC ERROR] \"CommonHeaderKey/Descriptor\" has invalid length (%d)\n", (int)b64_strlen(exhdrset->rsf->CommonHeaderKey.AccCtlDescBin));
		return COMMON_HEADER_KEY_NOT_FOUND;
	}

	/* Set RSA Keys */
	int result = 0;
	// NCCH Header pubk
	result = b64_decode(exhdrset->keys->rsa.cxi.pub,exhdrset->rsf->CommonHeaderKey.Modulus,0x100);
	if(result) return result;
	// NCCH Header privk
	result = b64_decode(exhdrset->keys->rsa.cxi.pvt,exhdrset->rsf->CommonHeaderKey.D,0x100);
	if(result) return result;

	/* Set AccessDesc */
	// Signature
	result = b64_decode(exhdrset->acexDesc->signature,exhdrset->rsf->CommonHeaderKey.AccCtlDescSign,0x100);
	if(result) return result;
	// NCCH Header pubk
	memcpy(exhdrset->acexDesc->ncchRsaPubKey,exhdrset->keys->rsa.cxi.pub,0x100);
	// Access Control
	result = b64_decode((u8*)&exhdrset->acexDesc->arm11SystemLocalCapabilities,exhdrset->rsf->CommonHeaderKey.AccCtlDescBin,0x200);
	if(result) return result;
	
	return 0;	
}

int accessdesc_GetSignFromPreset(exheader_settings *exhdrset)
{
	const CtrSdkDesc *desc = accessdesc_GetPresetData(exhdrset->keys);
	const CtrSdkDescSignData *pre_sign = accessdesc_GetPresetSignData(exhdrset->keys);
	const CtrSdkDepList *dependency_list = accessdesc_GetPresetDependencyList(exhdrset->keys);


	// Error Checking
	if (!desc || !dependency_list) {
		fprintf(stderr, "[ACEXDESC ERROR] AccessDesc template is unavailable, please configure RSF file\n");
		return CANNOT_SIGN_ACCESSDESC;
	}

	// Setting data in Exheader
	// Dependency List
	memcpy(exhdrset->exHdr->dependencyList, dependency_list->dependency, 0x180);

	// Backing Up Non Preset Data
	u8 ProgramID[8];
	exhdr_StorageInfo StorageInfoBackup;
	exhdr_ARM9AccessControlInfo Arm9Desc;
	memcpy(ProgramID, exhdrset->exHdr->arm11SystemLocalCapabilities.programId, 8);
	memcpy(&StorageInfoBackup, &exhdrset->exHdr->arm11SystemLocalCapabilities.storageInfo, sizeof(exhdr_StorageInfo));
	memcpy(&Arm9Desc, &exhdrset->exHdr->arm9AccessControlInfo, sizeof(exhdr_ARM9AccessControlInfo));

	// Setting Preset Data
	memcpy(&exhdrset->exHdr->arm11SystemLocalCapabilities, desc->exheader_desc, 0x200);

	// Restoring Non Preset Data
	memcpy(exhdrset->exHdr->arm11SystemLocalCapabilities.programId, ProgramID, 8);
	memcpy(&exhdrset->exHdr->arm11SystemLocalCapabilities.storageInfo, &StorageInfoBackup, sizeof(exhdr_StorageInfo));
	memcpy(&exhdrset->exHdr->arm9AccessControlInfo, &Arm9Desc, sizeof(exhdr_ARM9AccessControlInfo));


	// Setting AccessDesc Area		
	// If presign available set static data & ncch hdr sig info
	if (pre_sign)
	{
		memcpy(exhdrset->keys->rsa.cxi.pub, pre_sign->modulus, 0x100);
		memcpy(exhdrset->keys->rsa.cxi.pvt, pre_sign->priv_exponent, 0x100);
		memcpy(&exhdrset->acexDesc->signature, pre_sign->access_desc_signature, 0x100);
		memcpy(&exhdrset->acexDesc->ncchRsaPubKey, pre_sign->modulus, 0x100);
		memcpy(&exhdrset->acexDesc->arm11SystemLocalCapabilities, desc->signed_desc, 0x200);
	}
	// otherwise sign properly
	else
	{
		return accessdesc_SignWithKey(exhdrset);
	}
	

	return 0;
}

const CtrSdkDesc* accessdesc_GetPresetData(keys_struct *keys)
{
	for (int i = 0; i < sizeof(kDescPresets) / sizeof(CtrSdkDesc); i++) {
		if (kDescPresets[i].type == keys->accessDescSign.presetType && kDescPresets[i].fw_minor == keys->accessDescSign.targetFirmware) {
			return &kDescPresets[i];
		}
	}
	return NULL;
}

const CtrSdkDescSignData* accessdesc_GetPresetSignData(keys_struct *keys)
{
	if (keys->keyset != pki_DEVELOPMENT) {
		return NULL;
	}

	for (int i = 0; i < sizeof(kDevDescSignData) / sizeof(CtrSdkDescSignData); i++) {
		if (kDevDescSignData[i].type == keys->accessDescSign.presetType && kDevDescSignData[i].fw_minor == keys->accessDescSign.targetFirmware) {
			return &kDevDescSignData[i];
		}
	}
	
	return NULL;
}

const CtrSdkDepList* accessdesc_GetPresetDependencyList(keys_struct *keys)
{
	for (int i = 0; i < sizeof(kExheaderDependencyLists) / sizeof(CtrSdkDepList); i++) {
		if (kExheaderDependencyLists[i].fw_minor == keys->accessDescSign.targetFirmware) {
			return &kExheaderDependencyLists[i];
		}
	}
	return NULL;
}