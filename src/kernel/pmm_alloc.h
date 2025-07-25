#pragma once

#include "efi.h"
#include "efilib.h"
#include "../shared/addr.h"

#define MAX_PHYSP_MEM (1ull << 32)  // 4GB
#define MAX_PAGE_COUNT (MAX_PHYSP_MEM / PAGE_SIZE)

typedef uint64_t PhysicalAddress;

void pmm_init(const EFI_MEMORY_DESCRIPTOR *mem_map, 
              size_t count, size_t desc_size);

PhysicalAddress alloc_page();

void free_page(PhysicalAddress addr);
