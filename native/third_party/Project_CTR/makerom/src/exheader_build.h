#pragma once
#include "exheader.h"

typedef enum
{
	COMMON_HEADER_KEY_NOT_FOUND = -10,
	EXHDR_BAD_RSF_OPT = -11,
	CANNOT_SIGN_ACCESSDESC = -12
} exheader_errors;

typedef struct
{
	keys_struct *keys;
	rsf_settings *rsf;
	bool useAccessDescPreset;

	/* Output, these ptrs where created originally in ncchset */
	extended_hdr *exHdr;
	access_descriptor *acexDesc;
} exheader_settings;

/* ExHeader Build Functions */
int BuildExHeader(ncch_settings *ncchset);