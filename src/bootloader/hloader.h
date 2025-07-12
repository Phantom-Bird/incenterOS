#pragma once

#include "../shared/bootinfo.h"

void InitPaging(BootInfo boot_info);

BootInfo* TranslateBI(BootInfo *BI);
