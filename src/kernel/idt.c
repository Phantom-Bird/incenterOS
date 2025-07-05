#include "idt.h"
#include "isr_common.h"

IDTDescriptor idt[256];

void set_idt_entry(int vec, void (*handler)(), uint16_t selector, uint8_t type, uint8_t dpl) {
    uint64_t addr = (uint64_t)handler;
    idt[vec].offset_low  = addr & 0xFFFF;
    idt[vec].selector    = selector;
    idt[vec].ist         = 0;
    idt[vec].type        = type;
    idt[vec].dpl         = dpl;
    idt[vec].present     = 1;
    idt[vec].offset_mid  = (addr >> 16) & 0xFFFF;
    idt[vec].offset_high = (addr >> 32) & 0xFFFFFFFF;
    idt[vec].reserved    = 0;
}

void lidt(void* base, uint16_t size) {
    IDTPointer idtr = {
        .limit = size,
        .base = (uint64_t)base
    };
    __asm__ volatile ("lidt %0" : : "m"(idtr));
}

uint8_t get_type(int v){
    return (v==1 || v==3 || v==4 || v>=0x80)? 0xF: 0xE;
}

uint8_t get_dpl(int v){
    return (v >= 0x80)? 3: 0;
}

#define INIT_ISR(vec) set_idt_entry(vec, isr_stub_##vec, 0x08, get_type(vec), get_dpl(vec));
void init_idt() {
    MACRO_FOR_256(INIT_ISR)
    lidt(idt, sizeof(idt) - 1);
}
