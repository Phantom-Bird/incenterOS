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

    PutStr(L"[BOOT] Exiting boot...\r\n");
    UINTN MapKey = GetMapKey();
    ExitBootDevices(ImageHandle, MapKey);

    BI.magic = MAGIC;
    BI.graphics.framebuffer = fb;
    BI.graphics.width = ScreenWidth;
    BI.graphics.height = ScreenHeight;
    BI.graphics.size_bytes = FBSize;
    BI.graphics.pitch = Pitch;
    BI.stack.base_addr = KernelStackBase;
    BI.stack.size = KernelStackSize;
    BI.mem.mem_map = mem_map;
    BI.mem.count = MemMapCount;
    BI.mem.desc_size = MemMapDescSize;
    BI.initrd.start = (EFI_PHYSICAL_ADDRESS)InitRDStart;
    BI.initrd.size = InitRDSize;

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
