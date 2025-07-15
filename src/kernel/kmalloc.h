#include "stdint.h"

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint32_t level;
} KMallocHeader;

void *kmalloc(uint64_t size);

void kfree(void *addr);
