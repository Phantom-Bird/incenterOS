#include "pmm_alloc.h"
#include "bitarray.h"
#include "mem.h"
#include "print.h"

uint8_t page_bitmap[MAX_PAGE_COUNT / 8];  // 是否被占用（不可用或已分配）
uint8_t page_available[MAX_PAGE_COUNT / 8];  // 是否可用
size_t total_pages;
size_t phys_mem_size;

void pmm_init(const EFI_MEMORY_DESCRIPTOR * const mem_map, 
              size_t count, size_t desc_size){
    total_pages = 0;
    memset(page_bitmap, 0xFF, sizeof page_bitmap);
    memset(page_available, 0, sizeof page_available);

    for (size_t i=0; i < count; i++){
        const EFI_MEMORY_DESCRIPTOR *desc = 
            (const EFI_MEMORY_DESCRIPTOR*) ((uint8_t*)mem_map + i * desc_size);

        size_t end =  desc->VirtualStart + desc->NumberOfPages * PAGE_SIZE;
        if (end > phys_mem_size){
            phys_mem_size = end;
        }

        if (desc->Type != EfiConventionalMemory){
            continue;
        }

        size_t start_page = desc->PhysicalStart / PAGE_SIZE;
        size_t len = desc->NumberOfPages;

        for (size_t p=0; p < len; p++){
            if (start_page + p >= MAX_PAGE_COUNT){
                return;
            }

            clear_bit(page_bitmap, start_page + p);
            set_bit(page_available, start_page + p);
            total_pages++;
        }
    }
}

static size_t last_pos = 0;

PhysicalAddress alloc_page(){
    // print("pmm_alloc_page called...\n");
    for (size_t i = last_pos; i < MAX_PAGE_COUNT; i++){
        // print("!\n");
        if (!test_bit(page_bitmap, i)){
            // print(".\n");
            set_bit(page_bitmap, i);
            last_pos = i + 1;
            return i * PAGE_SIZE;
        }
    }
    return 0;
}

void free_page(PhysicalAddress addr){
    size_t idx = addr / PAGE_SIZE;
    if (idx >= MAX_PAGE_COUNT){
        return;
    }

    if (!test_bit(page_available, idx)){
        warning("[WARNING] Double free.\n");
        return;
    }

    clear_bit(page_bitmap, idx);
    if (idx < last_pos){
        last_pos = idx;
    }
}
