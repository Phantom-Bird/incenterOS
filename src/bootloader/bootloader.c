#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include "alloc.h"
#include "fs.h"
#include "loadkernel.h"
#include "exitboot.h"
#include "output.h"
#include "gop.h"
#include "../shared/graphics/logo.h"

EFI_SYSTEM_TABLE *ST;
EFI_BOOT_SERVICES *BS;
BootInfo BI;

void init_paging(BootInfo boot_info);

EFI_STATUS EFIAPI 
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    ST = SystemTable;
    BS = SystemTable->BootServices;

    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    PutStr(L"Booting...\r\n");

    EFI_FILE_HANDLE Volume = GetVolume(ImageHandle);
    PutStr(L"[BOOT] Reading kernel...\r\n");
    void *KernelBuffer = ReadFile(Volume, L"\\kernel.elf", NULL);
    PutStr(L"[BOOT] Loading kernel...\r\n");
    KERNEL_ENTRY KernelEntry = LoadKernel(KernelBuffer);
    PutStr(L"[BOOT] Loaded kernel.\r\n");
    SystemTable->BootServices->FreePool(KernelBuffer);

    PutStr(L"[BOOT] Locating GOP...\r\n");
    InitGOP();

    PutStr(L"[BOOT] Allocating stack...\r\n");
    const UINTN 
        M = 0x100000,
        KernelStackSize = 0x8000,
        KernelStackPages = KernelStackSize / PageSize,
        KernelStackBase = 24*M,
        KernelStackTop = KernelStackBase + KernelStackSize;
    AllocatePagesAt(KernelStackBase, KernelStackPages);

    PutStr(L"[BOOT] Loading initrd...\r\n");
    UINT64 InitRDSize;
    void *InitRDStart = ReadFile(Volume, L"\\initrd.img", &InitRDSize);

    // GetMapKey();

    paging_set_root();

    BI.magic = MAGIC;
    BI.graphics.framebuffer = fb;
    BI.graphics.width = ScreenWidth;
    BI.graphics.height = ScreenHeight;
    BI.graphics.size_bytes = FBSize;
    BI.graphics.pitch = Pitch;
    BI.stack.base_addr = KernelStackBase;
    BI.stack.size = KernelStackSize;
    GetMemMap(&BI.mem.mem_map, &BI.mem.count, &BI.mem.desc_size, NULL);
    BI.initrd.start = (EFI_PHYSICAL_ADDRESS)InitRDStart;
    BI.initrd.size = InitRDSize;
    BI.pml4_phys = pml4_phys;
    
    init_paging(BI);

    PutStr(L"[BOOT] Exiting boot...\r\n");
    // UINTN MapKey = GetMapKey();
    UINTN MapKey;
    GetMemMap(NULL, NULL, NULL, &MapKey);
    ExitBootDevices(ImageHandle, MapKey);
    
    set_cr3();

    __asm__ volatile (
        "mov %[stack], %%rsp\n"
        "mov %[info], %%rdi\n"
        "jmp *%[entry]\n"
        :
        : [stack] "r"(KernelStackTop), 
          [info] "r"(&BI),
          [entry] "r"(KernelEntry)
    );
    
    // 至此，永不返回。

    return EFI_SUCCESS;
}

#define MB (1024 * 1024)
#define APIC_DEFAULT_BASE 0xFEE00000
#define IOAPIC_DEFAULT_BASE  0xFEC00000

void init_paging(BootInfo boot_info){
    // 内核
    map_pages(0, 1024*MB, 0, PRESENT | WRITABLE);  // 恒等
    // 已经包括栈

    // 帧缓冲区
    uint64_t fb_base = (uint64_t)(boot_info.graphics.framebuffer);
    uint64_t fb_size = boot_info.graphics.size_bytes;  // BI 新增项目
    map_pages(fb_base, fb_size, fb_base, PRESENT | WRITABLE);

    // APIC MMIO
    map_pages(APIC_DEFAULT_BASE, 0x1000, APIC_DEFAULT_BASE, PRESENT | WRITABLE);
    map_pages(IOAPIC_DEFAULT_BASE, 0x1000, IOAPIC_DEFAULT_BASE, PRESENT | WRITABLE);

    // initrd
    map_pages(boot_info.initrd.start, 
              boot_info.initrd.size, 
              boot_info.initrd.start,
              PRESENT | WRITABLE);
}

