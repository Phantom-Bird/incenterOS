#pragma once
#include <efi.h>
#include <efilib.h>
#include "alloc.h"
#include "err.h"
#include "output.h"

extern EFI_MEMORY_DESCRIPTOR *mem_map;
extern UINT64 MemMapCount, MemMapDescSize;

UINTN GetMapKey();

void ExitBootDevices(EFI_HANDLE ImageHandle, UINTN MapKey);
