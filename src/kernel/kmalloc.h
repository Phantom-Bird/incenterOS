#pragma once

#include "stdint.h"

typedef union {
    struct __attribute__((packed)) {
        uint32_t magic;
        uint32_t level;
    }; 

    void *next_free;
} KMallocHeader;

void *kmalloc(uint64_t size);

void kfree(void *addr);
