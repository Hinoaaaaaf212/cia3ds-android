#include "lib.h"

#include <mbedtls/ccm.h>
#include "aes_keygen.h"

#include "ncch_read.h"
#include "ncsd_build.h"
#include "cardinfo.h"

void InitCardInfoHdr(cardinfo_hdr **cihdr, devcardinfo_hdr **dcihdr, cci_settings *set);
int SetWriteableAddress(cardinfo_hdr *hdr, cci_settings *set);
int SetCardInfoBitmask(cardinfo_hdr *hdr, cci_settings *set);
int SetCardInfoNotes(cardinfo_hdr *hdr, cci_settings *set);
void SetNcchHeader(cardinfo_hdr *hdr, cci_settings *set);
void SetCardSeedData(cardinfo_hdr *hdr, devcardinfo_hdr *devhdr, cci_settings *set);


int GenCardInfoHdr(cci_settings *set)
{
	cardinfo_hdr *cihdr;
	devcardinfo_hdr *dcihdr;
	
	InitCardInfoHdr(&cihdr,&dcihdr,set);
	
	if(SetWriteableAddress(cihdr,set))
		return GEN_HDR_FAIL;
	if(SetCardInfoBitmask(cihdr,set))
		return GEN_HDR_FAIL;
	if(SetCardInfoNotes(cihdr,set))
		return GEN_HDR_FAIL;
	SetNcchHeader(cihdr,set);
	SetCardSeedData(cihdr,dcihdr,set);
	
	return 0;
}

void InitCardInfoHdr(cardinfo_hdr **cihdr, devcardinfo_hdr **dcihdr, cci_settings *set)
{
	set->headers.cardinfohdr.size = sizeof(cardinfo_hdr) + sizeof(devcardinfo_hdr);
	set->headers.cardinfohdr.buffer = calloc(1,set->headers.cardinfohdr.size);
	
	*cihdr = (cardinfo_hdr*)set->headers.cardinfohdr.buffer;
	*dcihdr = (devcardinfo_hdr*)(set->headers.cardinfohdr.buffer+sizeof(cardinfo_hdr));
	
	clrmem(set->headers.cardinfohdr.buffer, set->headers.cardinfohdr.size);

	return;
}

u64 GetCciUnusedSize(u64 mediaSize, u8 cardType)
{
	if(cardType == mediatype_CARD1){
		switch(mediaSize){
			case (u64)MB*128: return (u64)2621440;
			case (u64)MB*256: return (u64)5242880;
			case (u64)MB*512: return (u64)10485760;
			case (u64)GB*1: return (u64)73924608;
			case (u64)GB*2: return (u64)147324928;
			case (u64)GB*4: return (u64)294649856;
			case (u64)GB*8: return (u64)587202560;
			default: return 0;
		}
	}
	else if(cardType == mediatype_CARD2){
		switch(mediaSize){
			case (u64)MB*512: return (u64)37224448;
			case (u64)GB*1: return (u64)73924608;
			case (u64)GB*2: return (u64)147324928;
			case (u64)GB*4: return (u64)294649856;
			case (u64)GB*8: return (u64)587202560;
			default: return 0;
		}
	}
	return 0;
}

int SetWriteableAddress(cardinfo_hdr *hdr, cci_settings *set)
{
	if(set->romInfo.mediaType != mediatype_CARD2){ // Can only be set for Card2 Media
		u32_to_u8(hdr->writableAddress,(u32)-1,LE);
		return 0;
	} 
	
	char *str = set->rsf->CardInfo.WritableAddress;
	set->romInfo.card2SaveOffset = -1;
	
	if(str){
		if(strncmp(str,"0x",2) != 0){
			fprintf(stderr,"[CCI ERROR] WritableAddress requires a Hexadecimal value\n");
			return INVALID_RSF_OPT;
		}	
		set->romInfo.card2SaveOffset = strtoull(str,NULL,16);
	}
	else{
		if ((set->romInfo.mediaSize / 2) < set->romInfo.saveSize || set->romInfo.saveSize > (u64)(2047*MB)){
			u64 saveDataSize = set->romInfo.saveSize / KB;
			fprintf(stderr,"[CCI ERROR] Too large SavedataSize %"PRIu64"K\n",saveDataSize);
			return SAVE_DATA_TOO_LARGE;
		}
		if(set->options.closeAlignWR)
			set->romInfo.card2SaveOffset = align(set->romInfo.usedSize, set->romInfo.blockSize); // invalid for "real" chips
		else{
			u64 unusedSize = GetCciUnusedSize(set->romInfo.mediaSize,set->romInfo.mediaType); // Some value related to the physical implementation of gamecards
			if(unusedSize > 0)
				set->romInfo.card2SaveOffset = set->romInfo.mediaSize - unusedSize - set->romInfo.saveSize; // Nintendo's method for calculating writable region offset
			else{
				fprintf(stderr,"[CCI WARNING] Nintendo does not support CARD2 for the current MediaSize, aligning save offset after last NCCH\n");
				set->romInfo.card2SaveOffset = align(set->romInfo.usedSize, set->romInfo.blockSize); // invalid for "real" chips
			}
		}
	}
	
	u32_to_u8(hdr->writableAddress,(u32)(set->romInfo.card2SaveOffset/set->romInfo.blockSize),LE);
	
	return 0;
}

int SetCardInfoBitmask(cardinfo_hdr *hdr, cci_settings *set)
{
	u32 bitmask = 0;

	char *str = set->rsf->CardInfo.CardType;
	if(!str) 
		bitmask |= 0 << 5;
	else{
		if(strcasecmp(str,"s1") == 0) 
			bitmask |= 0 << 5;
		else if(strcasecmp(str,"s2") == 0) 
			bitmask |= 1 << 5;
		else {
			fprintf(stderr,"[CCI ERROR] Invalid CardType: %s\n",str);
			return INVALID_RSF_OPT;
		}
	}
	
	str = set->rsf->CardInfo.CryptoType;
	if(!str) {
		u32 val = 0;
		if(set->keys->keyset == pki_DEVELOPMENT) {
			val = 3;
		}
		else{
			val = 0;
		}

		bitmask |= (val << 6);
	}
	else{
		int val = strtol(str,NULL,10);
		if(val < 0 || val > 3) {
			fprintf(stderr,"[CCI ERROR] Invalid CryptoType: %s\n",str);
			return INVALID_RSF_OPT;
		}
		if(val != 3 && set->keys->keyset == pki_DEVELOPMENT) {
			fprintf(stderr,"[CCI WARNING] Card crypto type = '%d', this is not supported for development target.\n",val);
		}
		if(val == 3 && set->keys->keyset == pki_PRODUCTION) {
			fprintf(stderr,"[CCI WARNING] Card crypto type = '%d', this is not supported for production target.\n",val);
		}
			
		bitmask |= val << 6;
	}
	
	u32_to_u8(hdr->cardInfoBitmask,bitmask,BE);
	
	return 0;
}

int SetCardInfoNotes(cardinfo_hdr *hdr, cci_settings *set)
{
	u64_to_u8(hdr->notes.mediaSizeUsed,set->romInfo.usedSize,LE);
	u32_to_u8(hdr->notes.unknown,0,LE);
	
	if(set->options.tmdHdr){
		u64_to_u8(hdr->notes.cverTitleId,GetTmdTitleId(set->options.tmdHdr),LE);
		u16_to_u8(hdr->notes.cverTitleId,GetTmdVersion(set->options.tmdHdr),LE);
	}
		
	return 0;
}

void SetNcchHeader(cardinfo_hdr *hdr, cci_settings *set)
{
	u8 *ncch;
	ncch_hdr *ncchHdr;
	
	ncch = set->content.data + set->content.dOffset[0];
	ncchHdr = (ncch_hdr*)ncch;
	
	memcpy(hdr->ncch0Hdr,GetNcchHdrData(ncchHdr),GetNcchHdrDataLen(ncchHdr));
	
	return;
}

void SetCardSeedData(cardinfo_hdr *hdr, devcardinfo_hdr *devhdr, cci_settings *set)
{
	u8 *ncch;
	ncch_hdr *ncchHdr;

	ncch = set->content.data + set->content.dOffset[0];
	ncchHdr = (ncch_hdr*)ncch;

	/*
	if (set->options.useExternalSdkCardInfo) {
		// initial data
		clrmem(hdr->cardSeedKeyY, 0x10);
		memcpy(hdr->cardSeedKeyY, ncchHdr->titleId, 8);
		clrmem(hdr->encCardSeed, 0x10);
		memcpy(hdr->cardSeedMac, stock_card_seed_mac, 0x10);
		clrmem(hdr->cardSeedNonce, 0xC);
		
		// dev card info
		memcpy(devhdr->titleKey, stock_title_key, 0x10);
		
		return;
	}
	*/
	

	// select title_key
	u8 title_key[0x10] = {0};
	if (set->options.useExternalSdkCardInfo)
	{
		memcpy(title_key, stock_title_key, 0x10);
	}
	else
	{
		rndset(title_key, 0x10);
	}

	// generate initial data
	{
		// set the keyY
		clrmem(hdr->cardSeedKeyY, 0x10);
		memcpy(hdr->cardSeedKeyY, ncchHdr->titleId, 8);

		// use crypto type to determine initial data key
		uint32_t crypto_type = (u8_to_u32(hdr->cardInfoBitmask, BE) >> 6) & 3;
		u8 initial_data_key[0x10] = {0};
		if (crypto_type == 3)
		{
			clrmem(initial_data_key, 0x10);
		}
		else
		{
			ctr_aes_keygen(set->keys->aes.initialDataKeyX, hdr->cardSeedKeyY, initial_data_key);
		}

		// determine nonce
		if (set->options.useExternalSdkCardInfo)
		{
			clrmem(hdr->cardSeedNonce, 0xC);
		}
		else
		{
			rndset(hdr->cardSeedNonce, 0xC);
		}

		// encrypt title key (& generate MAC)
		mbedtls_ccm_context ccm_ctx;
		mbedtls_ccm_init(&ccm_ctx);
		mbedtls_ccm_setkey(&ccm_ctx, MBEDTLS_CIPHER_ID_AES, initial_data_key, 128);

		int ccm_ret = mbedtls_ccm_encrypt_and_tag(&ccm_ctx, sizeof(title_key), hdr->cardSeedNonce, sizeof(hdr->cardSeedNonce), NULL, 0, title_key, hdr->encCardSeed, hdr->cardSeedMac, sizeof(hdr->cardSeedMac));
		if (ccm_ret != 0)
		{
			printf("[CARDINFO WARNING] Failed to encrypt initial data (mbedtls error: -0x%04X)\n", -ccm_ret);
		}

		mbedtls_ccm_free(&ccm_ctx);
	}

	// generate dev card info header
	{
		memcpy(devhdr->titleKey, title_key, 0x10);
	}
	
		
	return;
}

void SetDevCardInfo(devcardinfo_hdr *hdr, cci_settings *set)
{
	clrmem(hdr,sizeof(devcardinfo_hdr));
	if(set->options.useExternalSdkCardInfo)
		memcpy(hdr->titleKey,(u8*)stock_title_key,0x10);
	else
		rndset(hdr->titleKey,0x10);
		
	return;
}