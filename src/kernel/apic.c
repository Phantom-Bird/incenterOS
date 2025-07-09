#include <stdint.h>
#include "print.h"
#include "apic.h"

#define APIC_BASE_MSR 0x1B
#define APIC_SVR 0xF0
#define APIC_ENABLE 0x100
#define APIC_LVT_LINT0 (APIC_DEFAULT_BASE + 0x350)

void apic_enable() {
    // Enable APIC in MSR
    uint32_t low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(APIC_BASE_MSR));
    low |= (1 << 11); // APIC global enable
    __asm__ volatile ("wrmsr" : : "c"(APIC_BASE_MSR), "a"(low), "d"(high));

    // Enable APIC in SVR
    apic_write(APIC_SVR, 0xFF | APIC_ENABLE);

    print("[KERNEL] Local APIC enabled at 0xFEE00000\n");
}


void ioapic_redirect_irq(uint8_t irq, uint8_t vector, uint8_t apic_id/*CPU*/) {
    uint8_t index = 0x10 + irq * 2;

    uint32_t low = vector;
    uint32_t high = ((uint32_t)apic_id) << 24;

    ioapic_write(index + 1, high); // high dword: APIC ID
    ioapic_write(index, low);      // low dword: vector & flags(0)

    print("[KERNEL] IRQ "); print_dec(irq); print(" -> vector "); print_dec(vector); print(" mapped to APIC ID "); print_dec(apic_id); print("\n");
}
