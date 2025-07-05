#pragma once

#include <stdint.h>
#include "gop.h"

void set_scaling(int32_t w, int32_t h);

void putchar_at(char c, uint32_t x, uint32_t y, PIXEL fg, PIXEL bg);

void putchar(char c);

void print(char *s);

void raise_err(char *s);

void warning(char *s);

void common_color();

void warning_color();

void error_color();

void print_hex(uint64_t val);

void print_dec(int64_t val);