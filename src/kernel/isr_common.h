#pragma once
#include "macro_for.h"
#include <stdint.h>

typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    // 自动压入
    uint64_t err_code;
    uint64_t rip, cs, rflags, rsp, ss;
} SavedRegistersWithError;

typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    // 自动压入
    uint64_t rip, cs, rflags, rsp, ss;
} SavedRegisters;

void isr_common(int vec, void* const saved_registers);

#define DECLARE_ISR(vec) __attribute__((naked)) void isr_stub_##vec();
MACRO_FOR_256(DECLARE_ISR)
