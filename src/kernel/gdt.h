#pragma once
#include <stdint.h>
#include "mem.h"

// GDT 项格式
typedef struct __attribute__((packed)) {
    uint16_t limit_low  : 16;
    uint16_t base_low   : 16;
    uint8_t  base_mid   : 8;

    uint8_t  type       : 4;
    uint8_t  system     : 1;
    uint8_t  dpl        : 2;
    uint8_t  present    : 1;

    uint8_t  limit_high : 4;
    uint8_t  avl        : 1;
    uint8_t  l          : 1;
    uint8_t  db         : 1;
    uint8_t  granularity: 1;

    uint8_t  base_high  : 8;
} GDTEntry;

// GDT 描述符（给 lgdt 用）
typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint64_t base;
} GDTPtr;

void init_gdt();
void reload_segments();
