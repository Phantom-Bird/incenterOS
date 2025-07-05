#pragma once
#include <stdint.h>
#include "isr_common.h"

typedef void (*ISR)(uint8_t, void* const);
extern ISR isr[256];

void init_isr();
