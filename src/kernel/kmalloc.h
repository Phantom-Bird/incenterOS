#pragma once

#include <stdint.h>
#include <stddef.h>

#define KERNEL_HEAP_BASE 0xFFFF880000000000ULL
#define KERNEL_HEAP_SIZE 0x01000000ULL
#define KERNEL_HEAP_END  (KERNEL_HEAP_BASE + KERNEL_HEAP_SIZE)

typedef struct BlockHeader {
    size_t size;
    struct BlockHeader *next;
    struct BlockHeader *prev;
    int free;
} BlockHeader;

void* kmalloc(size_t size);
void kfree(void* ptr);
