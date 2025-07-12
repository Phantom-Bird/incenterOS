#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include "alloc.h"
#include "fs.h"
#include "loadkernel.h"
#include "memmap.h"
#include "exitboot.h"
#include "output.h"
#include "gop.h"
#include "../shared/graphics/logo.h"
#include "hloader.h"
#include "../shared/addr.h"
#include "paging.h"

EFI_SYSTEM_TABLE *ST;
EFI_BOOT_SERVICES *BS;
BootInfo BI;
KERNEL_ENTRY KernelEntry;
void *BIVirt;

EFI_STATUS EFIAPI 
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    ST = SystemTable;
    BS = SystemTable->BootServices;

    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    PutStr(L"incenterOS bootloader\r\n");

    EFI_FILE_HANDLE Volume = GetVolume(ImageHandle);

    PutStr(L"[BOOT] Loading kernel...\r\n");
    void *KernelBuffer = ReadFile(Volume, L"\\kernel.elf", NULL);
    KernelEntry = LoadKernel(KernelBuffer);
    SystemTable->BootServices->FreePool(KernelBuffer);

    PutStr(L"[BOOT] Locating GOP...\r\n");
    InitGOP();

    PutStr(L"[BOOT] Allocating stack...\r\n");
    const UINTN 
        KernelStackSize = 0x8000,
        KernelStackPages = KernelStackSize / PageSize,
        KernelStackBase = 24*MB,
        KernelStackTop = KernelStackBase + KernelStackSize;
    AllocatePagesAt(KernelStackBase, KernelStackPages);

    PutStr(L"[BOOT] Loading initrd...\r\n");
    UINT64 InitRDSize;
    void *InitRDStart = ReadFile(Volume, L"\\initrd.img", &InitRDSize);

    PutStr(L"[BOOT] Initalizing paging...\r\n");
    paging_set_root();

    PutStr(L"[BOOT] Initalizing boot info...\r\n");
    BI.magic = MAGIC;
    BI.graphics.framebuffer = fb;
    BI.graphics.width = ScreenWidth;
    BI.graphics.height = ScreenHeight;
    BI.graphics.size_bytes = FBSize;
    BI.graphics.pitch = Pitch;
    BI.stack.base_addr = KernelStackBase;
    BI.stack.size = KernelStackSize;
    BI.mem.phys_mem_size = GetMemSize();
    BI.initrd.start = InitRDStart;
    BI.initrd.size = InitRDSize;
    BI.pml4_phys = pml4_phys;

    PutStr(L"[BOOT] Mapping virtual address...\r\n");
    InitPaging(BI);

    // 临时映射 bootloader
    uint64_t rip;
    __asm__ volatile ("leaq (%%rip), %0" : "=r"(rip));
    PutStr(L"[BOOT] Runs at "); PrintHex(rip); PutStr(L"\r\n");
    map_pages(rip &~0xFFF, 2*MB, rip &~0xFFF, WRITABLE | PRESENT);
    
    PutStr(L"[BOOT] Exiting boot...\r\n");
    UINTN MapKey;
    GetMemMap(&BI.mem.mem_map, &BI.mem.count, &BI.mem.desc_size, &MapKey);
    ExitBootDevices(ImageHandle, MapKey);

    BIVirt = TranslateBI(&BI);

    __asm__ volatile ("mov %0, %%cr3" :: "r"(pml4_phys));

    __asm__ volatile (
        // "r" 的参数将在内联汇编之前放入寄存器，因此可以使用局部变量
        "mov %[stack], %%rsp\n"
        "mov %[info], %%rdi\n"
        "jmp *%[entry]\n"
        :
        : [stack] "r"(BI.stack.base_addr + BI.stack.size),  // 注意：x86 是满递减栈；rsp 指向有效内存的下一个地址
          [info] "r"(BIVirt),
          [entry] "r"(KernelEntry)
    );
    
    // 至此，永不返回。

    return EFI_SUCCESS;
}
