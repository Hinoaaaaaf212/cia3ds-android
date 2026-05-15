#pragma once
#include "tik.h"

tik_hdr *GetTikHdr(u8 *tik);
bool GetTikTitleKey(u8 *titleKey, tik_hdr *hdr, keys_struct *keys);