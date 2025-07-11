#include "exitboot.h"
#include "err.h"

void ExitBootDevices(EFI_HANDLE ImageHandle, UINTN MapKey){
    EFI_STATUS status = BS->ExitBootServices(ImageHandle, MapKey);
    if (EFI_ERROR(status)){
        Err(L"[ERROR] ExitBootDevices failed. Memory map changed?\r\n");
    }
}
