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
    uint64_t magic;
    GraphicsInfo graphics;
    StackInfo stack;
    MemMapInfo mem;
} BootInfo;
