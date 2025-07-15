#pragma once

#include <efi.h>
#include <efilib.h>
#include "err.h"
#include "output.h"

#define PageSize 0x1000
#define PageMask 0xFFFULL

void* Allocate(UINTN size);

void Free(void *ptr);

#define MAX_ALLOCATED 1024

void* AllocatePagesAt(EFI_PHYSICAL_ADDRESS addr, UINTN num_pages);

void* AllocatePageAt(EFI_PHYSICAL_ADDRESS addr);

BOOLEAN IsPageAllocated(EFI_PHYSICAL_ADDRESS addr);

void *TryAllocPagesAt(EFI_PHYSICAL_ADDRESS addr, UINTN num_pages);
