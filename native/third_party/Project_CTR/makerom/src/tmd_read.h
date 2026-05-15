#pragma once
#include "tmd.h"

// Read TMD
tmd_hdr *GetTmdHdr(u8 *tmd);
tmd_content_chunk* GetTmdContentInfo(u8 *tmd);
u64 GetTmdTitleId(tmd_hdr *hdr);
u32 GetTmdSaveSize(tmd_hdr *hdr);
u16 GetTmdContentCount(tmd_hdr *hdr);
u16 GetTmdVersion(tmd_hdr *hdr);

u32 GetTmdContentId(tmd_content_chunk info);
u16 GetTmdContentIndex(tmd_content_chunk info);
u16 GetTmdContentFlags(tmd_content_chunk info);
u64 GetTmdContentSize(tmd_content_chunk info);
u8* GetTmdContentHash(tmd_content_chunk info);

bool IsTmdContentEncrypted(tmd_content_chunk info);
bool ValidateTmdContent(u8 *data, tmd_content_chunk info);