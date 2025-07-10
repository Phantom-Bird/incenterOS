#pragma once

#include <stdint.h>
#include <efi.h>

#define MAGIC 0x00796c6665726946

typedef struct {
    uint32_t *framebuffer;
    uint32_t width;
    uint32_t height;
    uint64_t size_bytes;
    uint32_t pitch;
} GraphicsInfo;

typedef struct {
    uint64_t base_addr;
    uint64_t size;
} StackInfo;

typedef struct {
    EFI_MEMORY_DESCRIPTOR *mem_map;
    uint64_t count;
    uint64_t desc_size;
} MemMapInfo;

typedef struct {
    uint64_t start;
    uint64_t size;
} InitrdInfo;

typedef struct {
    void (*kernel)(struct BootInfo* bootinfo);
    uint64_t kernel_size;

    // init pmm
    uint8_t page_bitmap_virt;  // 是否被占用（不可用或已分配）
    uint8_t page_available_virt;  // 是否可用
    size_t total_pages;
    size_t phys_mem_size;

    // init paging
    void *pml4_virt;
} HighLoaderInfo;

typedef struct {
    uint64_t magic;
    GraphicsInfo graphics;
    StackInfo stack;
    MemMapInfo mem;
    InitrdInfo initrd;
    HighLoaderInfo high_loader_info;
} BootInfo;
