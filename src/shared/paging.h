#pragma once
#include <stdint.h>
#include <stddef.h>
#include "mem.h"

typedef uint64_t PhysicalAddress;
// 需要实现以下接口：
extern PhysicalAddress alloc_page();
extern void raise_err(char *s);

static inline void* phys2virt(PhysicalAddress phys){
    return (void*) phys;
}

#define PRESENT     0x1
#define WRITABLE    0x2
#define USER        0x4
#define UNCACHEABLE 0x10

typedef union PageTableEntry {
    struct __attribute__((packed)) {
        uint64_t present   : 1;
        uint64_t writable  : 1;
        uint64_t user      : 1;
        uint64_t writethru : 1;
        uint64_t cache_dis : 1;
        uint64_t accessed  : 1;
        uint64_t dirty     : 1;  // For PT only
        uint64_t hugepage  : 1;  // For PD/PT entries
        uint64_t global    : 1;
        uint64_t ignored1  : 3;
        uint64_t addr      : 40;  // 高 12 位
        uint64_t ignored2  : 11;
        uint64_t no_exec   : 1;
    };
    uint64_t value;
} PageTableEntry;

typedef PageTableEntry* PageTable;
extern PhysicalAddress pml4_phys;
extern PageTable kernel_pml4;

void map_page(uint64_t virt, uint64_t phys, uint64_t flags);

void map_pages(uint64_t virt_start, size_t bytes, uint64_t phys_start, uint64_t flags);


void paging_set_root();
void set_cr3();
void paging_load_root(PhysicalAddress pml4_phys);
