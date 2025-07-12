#include "hloader.h"
#include "paging.h"
#include "../shared/addr.h"

#define MB (1024 * 1024)

void InitPaging(BootInfo boot_info){
    // 全内存（主要是内核，供内核使用）
    uint64_t mem_size = boot_info.mem.phys_mem_size;
    if (mem_size > PHYS_MAP_MAX){
        mem_size = PHYS_MAP_MAX;
    }

    map_pages(HIGH_ADDR, mem_size, 0, PRESENT | WRITABLE); 
    map_pages(0, 8*MB, 0, PRESENT | WRITABLE);  // 恒等，此处只映射内核以确保其他数据映射完整
    // 内核栈
    map_pages(STACK_VIRT_BASE, boot_info.stack.size, boot_info.stack.base_addr, PRESENT | WRITABLE);

    // 帧缓冲区
    uint64_t fb_base = (uint64_t)(boot_info.graphics.framebuffer);
    uint64_t fb_size = boot_info.graphics.size_bytes;
    map_pages(FB_VIRT_BASE, fb_size, fb_base, PRESENT | WRITABLE | UNCACHEABLE);

    // APIC MMIO
    map_pages(APIC_VIRT_BASE, 0x1000, APIC_DEFAULT_BASE, PRESENT | WRITABLE | UNCACHEABLE);
    map_pages(IOAPIC_VIRT_BASE, 0x1000, IOAPIC_DEFAULT_BASE, PRESENT | WRITABLE | UNCACHEABLE);
}

BootInfo* TranslateBI(BootInfo *BI){
    BI->mem.mem_map = (void*)BI->mem.mem_map + HIGH_ADDR;
    BI->initrd.start = (void*)BI->initrd.start + HIGH_ADDR;

    BI->graphics.framebuffer = (void*)FB_VIRT_BASE;
    BI->stack.base_addr = STACK_VIRT_BASE;

    return (void*)BI + HIGH_ADDR;
    // 必须转成 (void*)，因为 ptr + idx 相当于 &ptr[idx]
}