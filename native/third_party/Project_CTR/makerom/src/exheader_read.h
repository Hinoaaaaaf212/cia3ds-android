#pragma once
#include "exheader.h"

/* ExHeader Binary Print Functions */
void exhdr_Print_ServiceAccessControl(extended_hdr *hdr);

/* ExHeader Binary Read Functions */
u8* GetAcexRsaSig(access_descriptor *acexDesc);
u8* GetAcexNcchPubKey(access_descriptor *acexDesc);
u16 GetRemasterVersion_frm_exhdr(extended_hdr *hdr);
u64 GetSaveDataSize_frm_exhdr(extended_hdr *hdr);
int GetDependencyList_frm_exhdr(u8 *Dest,extended_hdr *hdr);
void GetCoreVersion_frm_exhdr(u8 *Dest, extended_hdr *hdr);
