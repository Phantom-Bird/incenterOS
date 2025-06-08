#pragma once

#include <efi.h>
#include <efilib.h>

void Err(CHAR16 *e);

void ErrStatus(CHAR16 *e, EFI_STATUS status);
