#include <stdint.h>
#include <stddef.h>
#include "paging.h"
#include "pmm_alloc.h"
#include "kmalloc.h"
#include "print.h"

static BlockHeader* heap_head = NULL;
static void* heap_curr_end = (void*)KERNEL_HEAP_BASE;

static void heap_expand(size_t size_needed) {
    while ((uintptr_t)heap_curr_end + size_needed > (uintptr_t)heap_head + KERNEL_HEAP_SIZE) {
        void* phys = (void*)alloc_page();
        if (!phys) {
            raise_err("[ERROR] OOM");
        }
        map_page((uintptr_t)heap_curr_end, (uintptr_t)phys, 0b11); // PRESENT | WRITABLE
        heap_curr_end += PAGE_SIZE;
    }
}

void init_heap() {
    heap_expand(PAGE_SIZE);
    heap_head = (BlockHeader*)KERNEL_HEAP_BASE;
    heap_head->size = sizeof(BlockHeader);
    heap_head->next = NULL;
    heap_head->prev = NULL;
    heap_head->free = 1;
}

static void coalesce(BlockHeader* block) {
    // Forward merge
    if (block->next && block->next->free) {
        block->size += block->next->size;
        block->next = block->next->next;
        if (block->next)
            block->next->prev = block;
    }

    // Backward merge
    if (block->prev && block->prev->free) {
        block->prev->size += block->size;
        block->prev->next = block->next;
        if (block->next)
            block->next->prev = block->prev;
    }
}

void* kmalloc(size_t size) {
    if (size == 0) return NULL;
    size_t total_size = (size + sizeof(BlockHeader) + 15) & ~((size_t)15);

    BlockHeader* current = heap_head;
    while (current) {
        if (current->free && current->size >= total_size) {
            current->free = 0;
            return (void*)(current + 1);
        }
        if (!current->next) break;
        current = current->next;
    }

    void* block_start = (void*)((uintptr_t)current + current->size);
    heap_expand(total_size);
    BlockHeader* new_block = (BlockHeader*)block_start;
    new_block->size = total_size;
    new_block->free = 0;
    new_block->prev = current;
    new_block->next = NULL;
    current->next = new_block;

    return (void*)(new_block + 1);
}

void kfree(void* ptr) {
    if (!ptr) return;
    BlockHeader* block = (BlockHeader*)ptr - 1;
    block->free = 1;
    coalesce(block);
}
