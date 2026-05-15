#pragma once
#include "exefs.h"

/* ExeFs Read Functions */
bool DoesExeFsSectionExist(char *section, u8 *ExeFs);
u8* GetExeFsSection(char *section, u8 *ExeFs);
u8* GetExeFsSectionHash(char *section, u8 *ExeFs);
u32 GetExeFsSectionSize(char *section, u8 *ExeFs);
u32 GetExeFsSectionOffset(char *section, u8 *ExeFs);