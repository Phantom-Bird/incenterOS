#pragma once

#include <stdint.h>

void memcpy(uint8_t *to, uint8_t *from, uint64_t bytes);

void memset(uint8_t *start, uint8_t val, uint64_t bytes);