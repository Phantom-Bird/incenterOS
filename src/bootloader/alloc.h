#pragma once

#include <efi.h>
#include <efilib.h>
#include "err.h"

#define PageSize 0x1000
#define PageMask 0xFFF;

void* Allocate(UINTN size);

void Free(void *ptr);

#define MAX_ALLOCATED 128

void* AllocatePagesAt(EFI_PHYSICAL_ADDRESS addr, UINTN num_pages);

BOOLEAN IsPageAllocated(EFI_PHYSICAL_ADDRESS addr, UINTN num_pages);
