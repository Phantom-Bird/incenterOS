#pragma once
#include <efi.h>
#include <efilib.h>
#include "alloc.h"
#include "err.h"
#include "output.h"
#include <elf.h>
#include "../shared/bootinfo.h"
#include "../shared/addr.h"


typedef void (*KERNEL_ENTRY)(BootInfo*);

KERNEL_ENTRY LoadKernel(void *KernelBuffer);