#pragma once;

#include <stdint.h>
#include "paging.h"
#include "process.h"

PhysicalAddress proc_alloc_page(Process *process, uint8_t is_page_table);

void proc_map_page(Process *process, uint64_t virt, uint64_t phys, uint64_t flags);
