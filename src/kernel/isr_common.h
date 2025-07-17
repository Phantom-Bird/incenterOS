#pragma once
#include "macro_for.h"
#include <stdint.h>
#include "save_context.h"

void isr_common(int vec, void* const saved_registers);

#define DECLARE_ISR(vec) __attribute__((naked)) void isr_stub_##vec();
MACRO_FOR_256(DECLARE_ISR)
