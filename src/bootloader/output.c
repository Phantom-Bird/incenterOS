#include "output.h"

void PutStr(CHAR16 *s){
    ST->ConOut->OutputString(ST->ConOut, s);
}

void PutChar(CHAR16 c){
    CHAR16 s[2] = {c, 0};
    ST->ConOut->OutputString(ST->ConOut, s);
}

void PrintHex(UINTN val){
    CHAR16 buf[32];
    int len = 0;

    if (val == 0){
        len = 1;
        buf[0] = L'0';
    }

    while (val){
        UINTN nibble = val & 0xF;
        buf[len] = (nibble < 10)? (L'0'+nibble) : (L'A'+nibble-10);
        len++;
        val >>= 4;
    }

    PutStr(L"0x");
    for (int i=len-1; i>=0; i--){
        PutChar(buf[i]);
    }
}

void PrintDec(UINTN val){
    CHAR16 buf[32];
    int len = 0;

    if (val == 0){
        len = 1;
        buf[0] = L'0';
    }

    while (val){
        buf[len] = L'0' + val % 10;
        val /= 10;
        len++;
    }

    for (int i=len-1; i>=0; i--){
        PutChar(buf[i]);
    }
}