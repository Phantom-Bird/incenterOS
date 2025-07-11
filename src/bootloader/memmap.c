#include "memmap.h"

#define PAGE_SIZE 0x1000

#define STMT_WRAPPER(stmt) do {stmt} while (FALSE)
#define PTR_ASSIGN(ptr, val) STMT_WRAPPER(if (ptr) {*(ptr) = val;})

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

    UINT64 MemMapCount = MapSize / DescSize;
    UINT64 MemMapDescSize = DescSize;

    MapSize += DescSize * 8;
    EFI_MEMORY_DESCRIPTOR *mem_map = (EFI_MEMORY_DESCRIPTOR*) Allocate(MapSize);
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

UINT64 GetMemSize(){
    EFI_MEMORY_DESCRIPTOR *MemMap;
    UINT64 MemMapCount, MemMapDescSize;
    GetMemMap(&MemMap, &MemMapCount, &MemMapDescSize, NULL);

    UINT64 MemSize = 0;

    for (size_t i=0; i < MemMapCount; i++){
        const EFI_MEMORY_DESCRIPTOR *desc = 
            (const EFI_MEMORY_DESCRIPTOR*) ((uint8_t*)MemMap + i * MemMapDescSize);

        size_t end =  desc->VirtualStart + desc->NumberOfPages * PAGE_SIZE;
        if (end > MemSize){
            MemSize = end;
        }
    }

    return MemSize;
}
