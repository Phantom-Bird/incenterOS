#include "isr.h"
#include "isr_common.h"
#include "print.h"

ISR isr[256];

const char* exception_names[] = {
    "Division By Zero", "Debug", "Non Maskable Interrupt", "Breakpoint",
    "Overflow", "Bound Range Exceeded", "Invalid Opcode", "Device Not Available",
    "Double Fault", "Coprocessor Segment Overrun", "Invalid TSS", "Segment Not Present",
    "Stack-Segment Fault", "General Protection Fault", "Page Fault", "Reserved",
    "x87 Floating Point Exception", "Alignment Check", "Machine Check",
    "SIMD Floating Point Exception", "Virtualization Exception", "Control Protection Exception"
};

void handle_exception_with_err_code(uint8_t vec, SavedRegistersWithError* const regs){
    error_color();
    print("=== Exception ===\n");
    print(vec<22? exception_names[vec]: "Reserved");
    print("\nRIP: ");
    print_hex(regs->rip);
    print("\nRSP: ");
    print_hex(regs->rsp);
    print("\nERR: ");
    print_hex(regs->err_code);
    raise_err("");
}

void handle_exception_without_err_code(uint8_t vec, SavedRegisters* const regs){
    error_color();
    print("=== Exception ===\n");
    print(vec<22? exception_names[vec]: "Reserved");
    print("\nRIP: ");
    print_hex(regs->rip);
    print("\nRSP: ");
    print_hex(regs->rsp);
    raise_err("");
}

void init_isr(){
    for (int i=0; i<32; i++){
        isr[i] = (ISR)handle_exception_without_err_code;
    }
    isr[8] = isr[10] = isr[11] = isr[12] = 
             isr[13] = isr[14] = isr[17] = (ISR)handle_exception_with_err_code;
}