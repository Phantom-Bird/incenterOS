#pragma once
#include <efi.h>
#include <efilib.h>
#include "alloc.h"
#include "err.h"

EFI_FILE_HANDLE GetVolume(EFI_HANDLE image);

EFI_FILE_INFO* GetFileInfo(EFI_FILE_HANDLE FileHandle);

UINT64 FileSize(EFI_FILE_HANDLE FileHandle);

void* ReadFile(EFI_FILE_HANDLE Volume, CHAR16 *Path, UINT64 *outSize);
