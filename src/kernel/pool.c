#include "pool.h"
#include "pmm_alloc.h"
#include "paging.h"
#include "mem.h"

#define PADDING 4

LargePool create_pool(uint64_t virt_start, uint64_t size, uint64_t capacity){
    LargePool pool = (LargePool){
        .virt_start = virt_start,
        .size = 0,
        .capacity = capacity,
        .used = 0,
    };
    extend_pool(&pool, size);
    return pool;
}

uint8_t extend_pool(LargePool *pool, uint64_t size) {
    uint64_t pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    if (pages * PAGE_SIZE > pool->capacity) {
        return 0;
    }

    uint64_t old_pages = pool->size / PAGE_SIZE;
    uint64_t new_pages = pages - old_pages;

    for (uint64_t i = 0; i < new_pages; i++) {
        map_page(
            pool->virt_start + (old_pages + i) * PAGE_SIZE,
            (uint64_t)alloc_page(),
            PRESENT | WRITABLE
        );
    }

    pool->size += new_pages * PAGE_SIZE;
    return 1;
}

static inline uint64_t align_up(uint64_t x){
    return (x + 3ull) & ~3ull;
}

// 从池中分配内存
void *pool_alloc(LargePool *pool, uint64_t size) {
    uint64_t aligned_size = align_up(size);  // 8字节对齐

    if (pool->used + aligned_size > pool->size) {
        if (!extend_pool(pool, aligned_size)) {
            return NULL;
        }
    }

    void *ptr = (void*)(pool->virt_start + pool->used + PADDING);
    pool->used += aligned_size;
    return ptr;
}

char *pool_alloc_copy(LargePool *pool, uint64_t size, void *src){
    char *copy = pool_alloc(pool, size);
    if (copy) {
        memcpy(copy, src, size);
    }
    return copy;
}

char *pool_alloc_set(LargePool *pool, uint64_t size, uint8_t val){
    char *copy = pool_alloc(pool, size);
    if (copy) {
        memset(copy, val, size);
    }
    return copy;
}