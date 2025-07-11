#include "exitboot.h"

EFI_MEMORY_DESCRIPTOR *mem_map;
UINT64 MemMapCount, MemMapDescSize;

#define MACRO_WRAPPER(stmt) do {stmt} while (FALSE)
#define PTR_ASSIGN(ptr, val) MACRO_WRAPPER(if (ptr) {*(ptr) = val;})

void GetMemMap(
        EFI_MEMORY_DESCRIPTOR **outMemMap,
        UINT64 *outMemMapCount,
        UINT64 *outMemMapDescSize,
        UINT64 *outMapKey
) {
    EFI_STATUS status;

    UINTN MapSize = 0, MapKey, DescSize;
    UINT32 DescVer;

    status = BS->GetMemoryMap(&MapSize, NULL, &MapKey, &DescSize, &DescVer);
    if (status != EFI_BUFFER_TOO_SMALL){
        Err(L"[ERROR] GetMemoryMap (size query) failed.\r\n");
    }

    MemMapCount = MapSize / DescSize;
    MemMapDescSize = DescSize;

    MapSize += DescSize * 8;
    mem_map = (EFI_MEMORY_DESCRIPTOR*) Allocate(MapSize);
    if (!mem_map){
        Err(L"[ERROR] Failed to allocate memory map.\r\n");
    }

    status = BS->GetMemoryMap(&MapSize, mem_map, &MapKey, &DescSize, &DescVer);
    if (EFI_ERROR(status)){
        Err(L"[ERROR] GetMemoryMap (actual) failed.\r\n");
    }

    PTR_ASSIGN(outMemMap, mem_map);
    PTR_ASSIGN(outMemMapCount, MemMapCount);
    PTR_ASSIGN(outMemMapDescSize, DescSize);
    PTR_ASSIGN(outMapKey, MapKey);
}

void ExitBootDevices(EFI_HANDLE ImageHandle, UINTN MapKey){
    EFI_STATUS status = BS->ExitBootServices(ImageHandle, MapKey);
    if (EFI_ERROR(status)){
        Err(L"[ERROR] ExitBootDevices failed. Memory map changed?\r\n");
    }
}
