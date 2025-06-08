#pragma once
#include <efi.h>
#include <efilib.h>
#include "alloc.h"
#include "err.h"
#include "output.h"
#include <elf.h>


typedef UINTN (*KERNEL_ENTRY)(UINTN, UINTN);

KERNEL_ENTRY LoadKernel(void *KernelBuffer);