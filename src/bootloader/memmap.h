#pragma once
#include <efi.h>
#include <efilib.h>
#include "alloc.h"
#include "err.h"
#include "output.h"

void GetMemMap(
    EFI_MEMORY_DESCRIPTOR **outMemMap,
    UINT64 *outMemMapCount,
    UINT64 *outMemMapDescSize,
    UINT64 *outMapKey
);
UINT64 GetMemSize();
