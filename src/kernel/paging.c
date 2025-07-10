#include <stdint.h>
#include "paging.h"
#include "print.h"
#include "pmm_alloc.h"
#include "mem.h"

#define PAGE_SIZE   0x1000
#define ENTRY_COUNT 512
#define HUGE_PAGE   0x80

#define BITS9           0x1FF
#define PML4_IDX(virt)  (((virt) >> 39) & BITS9)
#define PDPT_IDX(virt)  (((virt) >> 30) & BITS9)
#define PD_IDX(virt)    (((virt) >> 21) & BITS9)
#define PT_IDX(virt)    (((virt) >> 12) & BITS9)

PageTable kernel_pml4;  // 内核页表根

PhysicalAddress alloc_table(){
    PhysicalAddress page_phys = alloc_page();
    if (!page_phys) {
        raise_err("[ERROR] Cannot alloc new page!");
    }

    PageTable page = (PageTable) phys2virt(page_phys);
    memset(page, 0, PAGE_SIZE);

    return page_phys;
}

PageTable get_or_alloc_table_of(PageTableEntry* entry, uint64_t flags) {
    if (!(entry->present)) {
        PhysicalAddress table_phys = alloc_table();
        entry->value = table_phys | flags;
    }
    
    return (uint64_t*) phys2virt(entry->value & ~(PAGE_SIZE-1));
}

void map_page(uint64_t virt, uint64_t phys, uint64_t flags) {
    // print("mapping ~~...\n");
    size_t pml4_index = PML4_IDX(virt);
    size_t pdpt_index = PDPT_IDX(virt);
    size_t pd_index   = PD_IDX(virt);
    size_t pt_index   = PT_IDX(virt);

    uint64_t* pdpt = get_or_alloc_table_of(kernel_pml4+pml4_index, PRESENT | WRITABLE);
    uint64_t* pd   = get_or_alloc_table_of(pdpt+pdpt_index, PRESENT | WRITABLE);
    uint64_t* pt   = get_or_alloc_table_of(pd+pd_index, PRESENT | WRITABLE);

    pt[pt_index] = (phys & ~0xFFFULL) | flags | PRESENT;
}

void map_pages(uint64_t virt_start, size_t bytes, uint64_t phys_start, uint64_t flags){
    for (uint64_t off=0; off < bytes; off += PAGE_SIZE){
        map_page(virt_start + off, phys_start + off, flags);
    }
}

void paging_set_root(){
    kernel_pml4 = phys2virt(alloc_table());
    if (!kernel_pml4){
        raise_err("[ERROR] Cannot alloc PML4 table.");
    }
}

void set_cr3(){
    if (!kernel_pml4){
        raise_err("[ERROR] Set CR3 with root unset.");
    }

    __asm__ volatile ("mov %0, %%cr3" :: "r"(kernel_pml4));
}
