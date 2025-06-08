#pragma once
#include <efi.h>
#include <efilib.h>
#include "alloc.h"
#include "err.h"


UINTN GetMapKey();

void ExitBootDevices(EFI_HANDLE ImageHandle, UINTN MapKey);
