#pragma once

#include <stdint.h>

typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    // 自动压入
    uint64_t rip, cs, rflags, rsp, ss;
} SavedRegisters;

typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    // 自动压入
    uint64_t err_code;
    uint64_t rip, cs, rflags, rsp, ss;
} SavedRegistersWithError;

#define PUSH_ALL_REGISTERS \
    "push %rax; push %rbx; push %rcx; push %rdx;" \
    "push %rsi; push %rdi; push %rbp; push %r8;" \
    "push %r9; push %r10; push %r11; push %r12;" \
    "push %r13; push %r14; push %r15;"

#define POP_ALL_REGISTERS \
    "pop %r15; pop %r14; pop %r13; pop %r12;" \
    "pop %r11; pop %r10; pop %r9; pop %r8;" \
    "pop %rbp; pop %rdi; pop %rsi; pop %rdx;" \
    "pop %rcx; pop %rbx; pop %rax;"
