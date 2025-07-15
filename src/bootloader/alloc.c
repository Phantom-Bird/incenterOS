#include "alloc.h"


void* Allocate(UINTN size){
    void *buf = NULL;
    EFI_STATUS status = BS->AllocatePool(EfiLoaderData, size, &buf);
    if (EFI_ERROR(status)){
        Err(L"[ERROR] Memory allocation failed!\r\n");
    }
    return buf;
}

void Free(void *ptr){
    BS->FreePool(ptr);
}

static EFI_PHYSICAL_ADDRESS AllocatedPages[MAX_ALLOCATED];
static UINTN AllocatedCnt = 0;

void* AllocatePagesAt(EFI_PHYSICAL_ADDRESS addr, UINTN num_pages){
    EFI_STATUS status = BS->AllocatePages(AllocateAddress, EfiLoaderData, num_pages, &addr);
    if (EFI_ERROR(status)){
        Err(L"[ERROR] Pages allocation failed!\r\n");
    }

    for (int i=0; i<num_pages; i++){
        if (AllocatedCnt >= MAX_ALLOCATED){
            PutStr(L"[ERROR] Allocated records overflow.\r\n");
            for (int i=0; i<AllocatedCnt; i++){
                PrintHex(AllocatedPages[i]); PutChar(L' ');
            }
            Err(L"");
        }

        AllocatedPages[AllocatedCnt] = addr + num_pages * PageSize;
        AllocatedCnt++;
    }

    return (void*) (UINTN) addr;
}

void* AllocatePageAt(EFI_PHYSICAL_ADDRESS addr){
    EFI_STATUS status = BS->AllocatePages(AllocateAddress, EfiLoaderData, 1, &addr);
    if (EFI_ERROR(status)){
        PutStr(L"[ERROR] Page allocation failed at\r\n");
        PrintHex(addr);
        Err(L"");
    }

    if (AllocatedCnt >= MAX_ALLOCATED){
        PutStr(L"[ERROR] Allocated records overflow.\r\n");
        for (int i=0; i<AllocatedCnt; i++){
            PrintHex(AllocatedPages[i]); PutChar(L' ');
        }
        Err(L"");
    }

    AllocatedPages[AllocatedCnt] = addr;
    AllocatedCnt++;

    return (void*) (UINTN) addr;
}

BOOLEAN IsPageAllocated(EFI_PHYSICAL_ADDRESS addr){
    EFI_PHYSICAL_ADDRESS range_end = addr + PageSize;

    for (int i=0; i<AllocatedCnt; i++){
        EFI_PHYSICAL_ADDRESS base = AllocatedPages[i];
        EFI_PHYSICAL_ADDRESS end = base + PageSize;

        if (!(addr>=end || range_end<=base)){
            return TRUE;
        }
    }

    return FALSE;
}

void *TryAllocPagesAt(EFI_PHYSICAL_ADDRESS addr, UINTN num_pages){
    UINTN skipped = 0;

    for (int i=0; i<num_pages; i++){
        if (IsPageAllocated(addr + i * PageSize)) {
            skipped++;
        } else {
            AllocatePageAt(addr + i * PageSize);
        }
    }

    PutStr(L"Skipped ");
    PrintDec(skipped);
    PutStr(L"\r\n");

    return (void*) (UINTN) addr;
}

