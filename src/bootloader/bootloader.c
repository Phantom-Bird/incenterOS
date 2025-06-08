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

CHAR8 *Image = 
    "#  #  ####  #     #     ####    @@@@  @@@@  @@@@  \n"
    "#  #  #     #     #     #  #    @     @  @  @  @  \n"
    "####  ####  #     #     #  #    @     @  @  @@@@  \n"
    "#  #  #     #     #     #  #    @  @  @  @  @     \n"
    "#  #  ####  ####  ####  ####    @@@@  @@@@  @     \n";
PIXEL Map[256];

EFI_STATUS EFIAPI 
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    ST = SystemTable;
    BS = SystemTable->BootServices;

    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    PutStr(L"Booting...\r\n");

    EFI_FILE_HANDLE Volume = GetVolume(ImageHandle);
    PutStr(L"[BOOT] Reading kernel...\r\n");
    void *KernelBuffer = ReadFile(Volume, L"\\kernel.elf");
    PutStr(L"[BOOT] Loading kernel...\r\n");
    KERNEL_ENTRY KernelEntry = LoadKernel(KernelBuffer);
    PutStr(L"[BOOT] Loaded kernel.\r\n");
    SystemTable->BootServices->FreePool(KernelBuffer);

    PutStr(L"[BOOT] Locating GOP...\r\n");
    InitGOP();
    Map['#'] = (PIXEL){73, 86, 228, 255};
    Map['@'] = (PIXEL){254, 220, 156, 255};
    DrawTextImage(Image, Map, 10, 256, 256);
    Map['@'] = (PIXEL){255, 255, 255, 255};
    Map['R'] = (PIXEL){0, 0, 255, 255};
    DrawTextImage(logo, Map, 2, 0, 64);

    // UINTN MapKey = GetMapKey();
    // ExitBootDevices(ImageHandle, MapKey);

    // PutStr(L"123 + 456 = ");
    // PrintDec(KernelEntry(123, 456));
    
    // 至此，永不返回。

    while (1)   {
        __asm__ volatile ("hlt");
    }
    

    return EFI_SUCCESS;
}
