#pragma once

#include "efi.h"
#include "efilib.h"

#define PAGE_SIZE 4096
#define MAX_PHYSP_MEM (1ull << 32)  // 4GB
#define MAX_PAGE_COUNT (MAX_PHYSP_MEM / PAGE_SIZE)

void pmm_init(const EFI_MEMORY_DESCRIPTOR *mem_map, 
              size_t count, size_t desc_size);

void* alloc_page();

void free_page(size_t addr);
