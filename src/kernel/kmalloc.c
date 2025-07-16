#include <stdint.h>
#include <stddef.h>
#include "lock.h"
#include "paging.h"
#include "kmalloc.h"

#define PAGE_SIZE 4096
#define MAX_LEVEL 12  // 2^12 = 4096
#define KMALLOC_MAGIC 0x66CCFF

void *free_head[MAX_LEVEL + 1];  // 0~12
Lock kmalloc_locks[MAX_LEVEL + 1];

uint64_t get_level(uint64_t size) {
    if (size > PAGE_SIZE){
        return 0;
    }

    uint32_t level = 0;
    while ((1UL << level) < size) {
        level++;
    }

    return level;
}

// 空闲块链表插入
void kmalloc_insert(uint32_t level, void *addr) {
    WITH_LOCK(&kmalloc_locks[level], {
        ((KMallocHeader*)addr)->next_free = free_head[level];
        free_head[level] = addr;
    });
}

void kmalloc_extend(uint32_t level) {
    void *page = phys2virt(alloc_page());
    uint64_t size = 1UL << level;
    for (uint64_t i = 0; i < PAGE_SIZE / size; i++) {
        kmalloc_insert(level, page + i * size);
    }
}

void *kmalloc(uint64_t size) {
    size += sizeof(KMallocHeader);

    if (size > PAGE_SIZE) {
        return NULL;
    }

    uint32_t level = get_level(size);
    if (!free_head[level]) {
        kmalloc_extend(level);
    }
    
    void *addr = NULL;
    WITH_LOCK(&kmalloc_locks[level], {
        addr = free_head[level];
        free_head[level] = ((KMallocHeader*)addr)->next_free;
    });

    *(KMallocHeader*)addr = (KMallocHeader){.magic=KMALLOC_MAGIC, .level=level};
    return addr + sizeof(KMallocHeader);
}

void kfree(void *addr) {
    if (!addr){
        return;
    }

    addr -= sizeof(KMallocHeader);

    if (((KMallocHeader*)addr) -> magic != KMALLOC_MAGIC){
        return;
    }

    uint32_t level = ((KMallocHeader*)addr) -> level;
    if (level > MAX_LEVEL){
        return;
    }
    
    kmalloc_insert(level, addr);
}