#include "lib.h"
#include "cia_build.h"
#include "tik_build.h"

// Private Prototypes
int SetupTicketBuffer(cia_settings *set);
void SetupTicketHeader(tik_hdr *hdr, cia_settings *ciaset);
int SignTicketHeader(buffer_struct *tik, keys_struct *keys);
void SetLimits(tik_hdr *hdr, cia_settings *ciaset);
u32 GetContentIndexSegNum(cia_settings *set);
void SetContentIndexHeader(tik_content_index_hdr *hdr, cia_settings *set);
void SetContentIndexData(tik_content_index_struct *data, cia_settings *set);

int CryptTitleKey(u8 *input, u8 *output, u8 *titleId, keys_struct *keys, u8 mode);

int BuildTicket(cia_settings *set)
{
	if(SetupTicketBuffer(set)) 
		return MEM_ERROR;
	
	// Setting Ticket Struct Ptrs
	buffer_struct *tik = &set->ciaSections.tik;
	
	tik_hdr *hdr = (tik_hdr*) (tik->buffer + sizeof(tik_signature));
	tik_content_index_hdr *idxHdr = (tik_content_index_hdr*) (tik->buffer + sizeof(tik_signature) + sizeof(tik_hdr));
	tik_content_index_struct *idxData = (tik_content_index_struct*) (tik->buffer + sizeof(tik_signature) + sizeof(tik_hdr) + sizeof(tik_content_index_hdr));
	
	
	SetupTicketHeader(hdr,set);
	SetContentIndexHeader(idxHdr,set);
	SetContentIndexData(idxData,set);
	
	return SignTicketHeader(tik,set->keys);
}

int SetupTicketBuffer(cia_settings *set)
{
	buffer_struct *tik = &set->ciaSections.tik;
	
	tik->size = sizeof(tik_signature) + sizeof(tik_hdr) + sizeof(tik_content_index_hdr) + sizeof(tik_content_index_struct)*GetContentIndexSegNum(set);
	tik->buffer = calloc(1,tik->size);
	if(!tik->buffer) { 
		fprintf(stderr,"[TIK ERROR] Not enough memory\n"); 
		return MEM_ERROR; 
	}
	return 0;
}

void SetupTicketHeader(tik_hdr *hdr, cia_settings *ciaset)
{
	clrmem(hdr,sizeof(tik_hdr));
	
	memcpy(hdr->issuer,ciaset->tik.issuer,0x40);
	hdr->formatVersion = ciaset->tik.formatVersion;
	hdr->caCrlVersion = ciaset->cert.caCrlVersion;
	hdr->signerCrlVersion = ciaset->cert.signerCrlVersion;
	u64_to_u8(hdr->ticketId,ciaset->tik.ticketId,BE);
	u32_to_u8(hdr->deviceId,ciaset->tik.deviceId,BE);
	u64_to_u8(hdr->titleId,ciaset->common.titleId,BE);
	u16_to_u8(hdr->ticketVersion,ciaset->tik.version,BE);
	hdr->licenceType = ciaset->tik.licenceType;
	hdr->keyId = ciaset->keys->aes.currentCommonKey;
	u32_to_u8(hdr->eshopAccId,ciaset->tik.eshopAccId,BE);
	hdr->audit = ciaset->tik.audit;
	SetLimits(hdr,ciaset);
	
	// Crypt TitleKey
	if(ciaset->content.encryptCia)
		CryptTitleKey(ciaset->common.titleKey, hdr->encryptedTitleKey, hdr->titleId, ciaset->keys, ENC);
	else
		rndset(hdr->encryptedTitleKey,AES_128_KEY_SIZE);
}

int SignTicketHeader(buffer_struct *tik, keys_struct *keys)
{
	tik_signature *sig = (tik_signature*)tik->buffer;
	u8 *data = tik->buffer + sizeof(tik_signature);
	u32 len = tik->size - sizeof(tik_signature);
	

	clrmem(sig,sizeof(tik_signature));
	u32_to_u8(sig->sigType,RSA_2048_SHA256,BE);
	
	if (Rsa2048Key_CanSign(&keys->rsa.xs) == false)
	{
		printf("[TIK WARNING] Failed to sign header (key was incomplete)\n");
		memset(sig->data, 0xFF, 0x100);
		return 0;
	}

	int rsa_ret = RsaSignVerify(data, len, sig->data, keys->rsa.xs.pub, keys->rsa.xs.pvt, RSA_2048_SHA256, CTR_RSA_SIGN);
	if (rsa_ret != 0)
	{
		printf("[TIK WARNING] Failed to sign header (mbedtls error = -0x%x)\n", -rsa_ret);
		memset(sig->data, 0xFF, 0x100);
		return 0;
	}

	return 0;
}

int CryptTitleKey(u8 *input, u8 *output, u8 *titleId, keys_struct *keys, u8 mode)
{
	//Generating IV
	u8 iv[16];
	clrmem(&iv,16);
	memcpy(iv,titleId,0x8);
		
	//Crypting TitleKey
	AesCbcCrypt(keys->aes.commonKey[keys->aes.currentCommonKey],iv,input,output,0x10,mode);
	
	// Return
	return 0;
}

void SetLimits(tik_hdr *hdr, cia_settings *ciaset) // TODO?
{
	memset(hdr->limits,0,0x40);
}

u32 GetContentIndexSegNum(cia_settings *set)
{
	u32 num, level, i;
	
	num = level = 0;
	
	for( i = 0; i < set->content.count; i++)
	{
		if(set->content.index[i] >= level)
		{
			level = roundup(set->content.index[i],0x400);
			num++;
		}
	}
	return num;
}

void SetContentIndexHeader(tik_content_index_hdr *hdr, cia_settings *set)
{
	u32 hdrSize = sizeof(tik_content_index_hdr);
	u32 segNum = GetContentIndexSegNum(set);
	u32 segSize = sizeof(tik_content_index_struct);
	u32 segTotalSize = segSize * segNum;
	u32 totalSize = hdrSize + segTotalSize;
	
	u32_to_u8(hdr->unk0,0x00010014,BE);
	u32_to_u8(hdr->totalSize,totalSize,BE);
	u32_to_u8(hdr->unk1,0x00000014,BE);
	u32_to_u8(hdr->unk2,0x00010014,BE);
	u32_to_u8(hdr->unk3,0x00000000,BE);
	u32_to_u8(hdr->hdrSize,hdrSize,BE);
	u32_to_u8(hdr->segNum,segNum,BE);
	u32_to_u8(hdr->segSize,segSize,BE);
	u32_to_u8(hdr->segTotalSize,segTotalSize,BE);
	u32_to_u8(hdr->unk4,0x00030000,BE);
}

void SetContentIndexData(tik_content_index_struct *data, cia_settings *set)
{
	u32 level, i;
	int j;
	
	j = -1;
	level = 0;
	
	for( i = 0; i < set->content.count; i++)
	{
		if(set->content.index[i] >= level)
		{
			level = roundup(set->content.index[i],0x400);
			j++;
			u32_to_u8(data[j].level,(set->content.index[i]/0x400)*0x400,BE);
		}
		data[j].index[(set->content.index[i] & 0x3ff)/8] |= 1 << (set->content.index[i] & 0x7);
	}
	
}

tik_hdr *GetTikHdr(u8 *tik)
{
	u32 sigType = u8_to_u32(tik,BE);

	switch(sigType){
		case(RSA_4096_SHA1):
		case(RSA_4096_SHA256):
			return (tik_hdr*)(tik+0x240);
		case(RSA_2048_SHA1):
		case(RSA_2048_SHA256):
			return (tik_hdr*)(tik+0x140);
		case(ECC_SHA1):
		case(ECC_SHA256):
			return (tik_hdr*)(tik+0x7C);
	}

	return NULL;
}

bool GetTikTitleKey(u8 *titleKey, tik_hdr *hdr, keys_struct *keys)
{
	if(keys->aes.commonKey[hdr->keyId] == NULL)
		return false;
		
	keys->aes.currentCommonKey = hdr->keyId;
	
	CryptTitleKey(hdr->encryptedTitleKey, titleKey, hdr->titleId, keys, DEC);
	
	return true;
}