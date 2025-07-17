#include "print.h"
#include "isr_common.h"
#include "isr.h"
#include "save_context.h"

void isr_common(int vec, void* const saved_registers){
    if (isr[vec]){
        isr[vec](vec, saved_registers);
        return;
    }

    warning_color();
    print("[WARNING] Unhandled int ");
    print_hex(vec);
    print(", saved registers at ");
    print_hex((uint64_t) saved_registers);
    putchar('\n');
    common_color();
}

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