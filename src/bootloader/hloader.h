#pragma once

#include "../shared/bootinfo.h"

void InitPaging(BootInfo boot_info);

void TranslateBI(BootInfo *BI);
