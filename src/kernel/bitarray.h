#pragma once

#include <stdint.h>
#include <stddef.h>

typedef uint8_t* bit_array;

void set_bit(bit_array arr, size_t idx);

void clear_bit(bit_array arr, size_t idx);

int test_bit(bit_array arr, size_t idx);
