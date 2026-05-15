#pragma once
#include "cia.h"

// Cia Read Functions
bool IsCia(u8 *cia);
u64 GetCiaCertOffset(cia_hdr *hdr);
u64 GetCiaCertSize(cia_hdr *hdr);
u64 GetCiaTikOffset(cia_hdr *hdr);
u64 GetCiaTikSize(cia_hdr *hdr);
u64 GetCiaTmdOffset(cia_hdr *hdr);
u64 GetCiaTmdSize(cia_hdr *hdr);
u64 GetCiaContentOffset(cia_hdr *hdr);
u64 GetCiaContentSize(cia_hdr *hdr);
u64 GetCiaMetaOffset(cia_hdr *hdr);
u64 GetCiaMetaSize(cia_hdr *hdr);

u8* GetCiaCert(u8 *cia);
u8* GetCiaTik(u8 *cia);
u8* GetCiaTmd(u8 *cia);
u8* GetCiaContent(u8 *cia);
u8* GetCiaMeta(u8 *cia);
