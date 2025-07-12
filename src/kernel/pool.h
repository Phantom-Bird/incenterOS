#pragma once

#include <stdint.h>
#include "paging.h"

typedef struct {
    uint64_t virt_start;
    uint64_t size;
    uint64_t capacity;
    uint64_t used;
    PhysicalAddress pml4_phys;
} LargePool;

LargePool create_pool(uint64_t virt_start, uint64_t size, uint64_t capacity, PhysicalAddress pml4_phys);
uint8_t extend_pool(LargePool *pool, uint64_t size);
void *pool_alloc(LargePool *pool, uint64_t size);
char *pool_alloc_copy(LargePool *pool, uint64_t size, void *src);
char *pool_alloc_set(LargePool *pool, uint64_t size, uint8_t val);
