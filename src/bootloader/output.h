#pragma once

#include <efi.h>
#include <efilib.h>

void PutStr(CHAR16 *s);

void PutChar(CHAR16 c);

void PrintHex(UINTN val);

void PrintDec(UINTN val);
