#include "print.h"
#include "isr_common.h"
#include "isr.h"

void isr_common(int vec, void* const saved_registers){
    if (isr[vec]){
        isr[vec](vec, saved_registers);
        return;
    }

    warning_color();
    print("[WARNING] int ");
    print_hex(vec);
    print(", saved registers at ");
    print_hex((uint64_t) saved_registers);
    putchar('\n');
    common_color();
}

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

#define ISR_STUB(vec)                                \
    __attribute__((naked)) void isr_stub_##vec() {    \
        __asm__ volatile (                      \
            PUSH_ALL_REGISTERS                  \
            "mov $" #vec ", %rdi;"              \
            "mov %rsp, %rsi;"                   \
            "call isr_common;"                  \
            POP_ALL_REGISTERS                   \
            "iretq;"                            \
        );                                      \
    }

MACRO_FOR_256(ISR_STUB)