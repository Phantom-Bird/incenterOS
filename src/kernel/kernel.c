// 在 bootloader 中，为了和 UEFI 统一，使用了驼峰命名法
// 在此处就换回来了。

#include "../shared/bootinfo.h"
#include "gop.h"
#include "../shared/graphics/logo.h"
#include "mem.h"
#include "pmm_alloc.h"
#include "print.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "paging.h"
#include "apic.h"
#include "timer.h"
#include "keyboard.h"
#include "cpio.h"
#include "initramfs.h"

#define MB 0x100000ull
#define KERNEL_VIRT 0xffff800000000000
#define FB_VIRT     0xffff900000000000

void kernel_main();

BootInfo boot_info;

void kernel_entry(BootInfo *bi) {
    boot_info = *bi;
    kernel_main();
}

void init_kernel_paging();

void kernel_main(){
    if (boot_info.magic != MAGIC){
        while (1){
            __asm__ volatile ("hlt");
        }
    }

    paging_load_root(boot_info.pml4_phys);
    // init_kernel_paging();

    InitGOPFrom(boot_info.graphics);
    ClearScreen();
    set_scaling(1, 2);

    // print("[KERNEL] Successfully loaded page table!\n");

    __asm__ volatile ("cli");

    print("[KERNEL] Initalizing GDT...\n");
    init_gdt();
    reload_segments();

    print("[KERNEL] Initializing IDT...\n");
    init_isr();
    init_idt();

    print("[KERNEL] Initializing physical memory allocater...\n");
    print("cnt=");print_dec(boot_info.mem.count);print("\n");
    pmm_init(boot_info.mem.mem_map,
             boot_info.mem.count,
             boot_info.mem.desc_size);
    
    // print("[KERNEL] Initializing paging...\n");
    // paging_set_root();
    
    // print("[KERNEL] Loading page table...\n");
    // set_cr3();
    // print("[KERNEL] Now kernel runs at 0xFFFF800000000000!\n");
    
    apic_enable();
    init_timer();
    init_keyboard();
    
    __asm__ volatile ("sti");

    print("[KERNEL] Loading initrd...\n");
    cpio_init((void*)boot_info.initrd.start, boot_info.initrd.size);
    
    print("[KERNEL]\n");
    FSItem *file_info = initramfs_find(NULL, "/dir/ciallo.txt");
    if (!file_info){
        raise_err("[ERROR] Kernel was caught 0721ing!");
    }
    print("=== file ===\n");
    print(file_info->file_data);
    print("\n============\n");

    print("[KERNEL] logo of incenterOS -> \n");

    PIXEL color_map[256] = {['@'] = (PIXEL){255, 255, 255, 255}, ['R'] = (PIXEL){0, 0, 255, 255}};
    int unit = 4;
    DrawTextImage(
        logo, color_map, unit, 
        (ScreenWidth-logo_size*unit)/2, 
        (ScreenHeight-logo_size*unit)/2);

    while (1){
        __asm__ volatile ("hlt");
    }
}

void init_kernel_paging(){
    // 内核
    map_pages(0, 256*MB, 0, PRESENT | WRITABLE);  // 恒等
    map_pages(KERNEL_VIRT, 32*MB, 0, PRESENT | WRITABLE);  // 高地址
    // 已经包括栈

    // 帧缓冲区
    uint64_t fb_base = (uint64_t)(boot_info.graphics.framebuffer);
    uint64_t fb_size = boot_info.graphics.size_bytes;  // BI 新增项目
    map_pages(fb_base, fb_size, fb_base, PRESENT | WRITABLE);
    map_pages(FB_VIRT, fb_size, fb_base, PRESENT | WRITABLE);

    // APIC MMIO
    map_pages(APIC_DEFAULT_BASE, 0x1000, APIC_DEFAULT_BASE, PRESENT | WRITABLE);
    map_pages(IOAPIC_DEFAULT_BASE, 0x1000, IOAPIC_DEFAULT_BASE, PRESENT | WRITABLE);

    // initrd
    map_pages(boot_info.initrd.start, 
              boot_info.initrd.size, 
              boot_info.initrd.start,
              PRESENT | WRITABLE);
}

