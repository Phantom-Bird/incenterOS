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
        Err(L"[ERROR] Page allocation failed!\r\n");
    }

    for (int i=0; i<=num_pages; i++){
        if (AllocatedCnt >= MAX_ALLOCATED){
            Err(L"[ERROR] Allocated records overflow.\r\n");
        }
        AllocatedPages[AllocatedCnt] = addr + i * PageSize;
        AllocatedCnt++;
    }

    return (void*) (UINTN) addr;
}

BOOLEAN IsPageAllocated(EFI_PHYSICAL_ADDRESS addr, UINTN num_pages){
    EFI_PHYSICAL_ADDRESS range_end = addr + num_pages * PageSize;

    for (int i=0; i<AllocatedCnt; i++){
        EFI_PHYSICAL_ADDRESS base = AllocatedPages[i];
        EFI_PHYSICAL_ADDRESS end = base + PageSize;

        if (!(addr>=end || range_end<=base)){
            return TRUE;
        }
    }

    return FALSE;
}

