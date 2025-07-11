#include "output.h"
#include "err.h"
#include "efi.h"

void raise_err(char *err){
    for (; *err; err++){
        PutChar((CHAR16){*err, 0});
    }

    Err(L"");
}

EFI_PHYSICAL_ADDRESS alloc_page(){
    EFI_STATUS status;
    EFI_PHYSICAL_ADDRESS Memory;

    status = gBS->AllocatePages(
        AllocateAnyPages,        // 分配任意可用页
        EfiLoaderData,
        1,
        &Memory                 // 返回分配的物理地址
    );

    if (EFI_ERROR(status)) {
        return 0;
    }
    return Memory;
}