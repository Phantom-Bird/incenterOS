#include "err.h"
#include "output.h"

void Err(CHAR16 *e){
    PutStr(e);
    while (1){
        __asm__ volatile ("hlt");
    }
}

void ErrStatus(CHAR16 *e, EFI_STATUS status){
    PutStr(e);
    PutStr(L"[ERROR] Status: ");
    PrintHex(status);
    while (1){
        __asm__ volatile ("hlt");
    }
}
