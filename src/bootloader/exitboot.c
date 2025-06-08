#include "exitboot.h"


UINTN GetMapKey() {
    EFI_STATUS status;

    UINTN MapSize = 0, MapKey, DescSize;
    UINT32 DescVer;

    status = BS->GetMemoryMap(&MapSize, NULL, &MapKey, &DescSize, &DescVer);
    if (status != EFI_BUFFER_TOO_SMALL){
        Err(L"[ERROR] GetMemoryMap (size query) failed.\r\n");
    }

    MapSize += DescSize * 8;

    EFI_MEMORY_DESCRIPTOR *mem_map = (EFI_MEMORY_DESCRIPTOR*) Allocate(MapSize);
    if (!mem_map){
        Err(L"[ERROR] Failed to allocate memory map.\r\n");
    }

    status = BS->GetMemoryMap(&MapSize, mem_map, &MapKey, &DescSize, &DescVer);
    if (EFI_ERROR(status)){
        Err(L"[ERROR] GetMemoryMap (actual) failed.\r\n");
    }

    return MapKey;
}

void ExitBootDevices(EFI_HANDLE ImageHandle, UINTN MapKey){
    EFI_STATUS status = BS->ExitBootServices(ImageHandle, MapKey);
    if (EFI_ERROR(status)){
        Err(L"[ERROR] ExitBootDevices failed. Memory map changed?\r\n");
    }
}
