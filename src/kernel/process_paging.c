#include "process_paging.h"
#include "paging.h"
#include "pmm_alloc.h"
#include "../shared/addr.h"

#define HUGE_PAGE_SIZE  (1ull << 21)
#define ENTRY_COUNT     512
#define HUGE_PAGE       0x80

#define BITS9           0x1FF
#define PML4_IDX(virt)  (((virt) >> 39) & BITS9)
#define PDPT_IDX(virt)  (((virt) >> 30) & BITS9)
#define PD_IDX(virt)    (((virt) >> 21) & BITS9)
#define PT_IDX(virt)    (((virt) >> 12) & BITS9)

PhysicalAddress proc_alloc_page(Process *process, uint16_t flags){
    PhysicalAddress page = alloc_page();
    ProcessPage *pp = kmalloc(sizeof(ProcessPage));
    *pp = (ProcessPage){
        .flags = flags,
        .page = page,
        .next = process->pages,
    };
    process->pages = pp;
}

PhysicalAddress proc_alloc_table(Process *process){
    PhysicalAddress page_phys = proc_alloc_page(process, PAGE_TABLE);
    if (!page_phys) {
        raise_err("[ERROR] Cannot alloc new page!");
    }

    PageTable page = (PageTable) phys2virt(page_phys);
    memset(page, 0, PAGE_SIZE);

    return page_phys;
}

PageTable proc_get_or_alloc_table_of(Process *process, PageTableEntry* entry, uint64_t flags) {
    if (!(entry->present)) {
        PhysicalAddress table_phys = proc_alloc_table(process);
        entry->value = table_phys | flags;
    }
    
    return (uint64_t*) phys2virt(entry->value & ~(PAGE_SIZE-1));
}

void proc_map_page(Process *process, uint64_t virt, uint64_t phys, uint64_t flags) {
    size_t pml4_index = PML4_IDX(virt);
    size_t pdpt_index = PDPT_IDX(virt);
    size_t pd_index   = PD_IDX(virt);
    size_t pt_index   = PT_IDX(virt);

    PageTable pdpt = proc_get_or_alloc_table_of(process, (PageTable)phys2virt(process->cr3)+pml4_index, PRESENT | WRITABLE);
    PageTable pd   = proc_get_or_alloc_table_of(process, pdpt+pdpt_index, PRESENT | WRITABLE);
    PageTable pt   = proc_get_or_alloc_table_of(process, pd+pd_index, PRESENT | WRITABLE);

    pt[pt_index].value = (phys & ~0xFFFULL) | flags;
}
