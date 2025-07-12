#pragma once
#include <stdint.h>
#include "../shared/addr.h"

void apic_enable();
void ioapic_redirect_irq(uint8_t irq, uint8_t vector, uint8_t apic_id);

// 以下是 APIC 工具函数

#define APIC_EOI 0xB0
#define IOAPIC_REGSEL  IOAPIC_VIRT_BASE
#define IOAPIC_WINDOW  (IOAPIC_VIRT_BASE + 0x10)

// IMPORTANT TODO: virt
static inline void apic_write(uint32_t reg, uint32_t val) {
    ((volatile uint32_t*)APIC_VIRT_BASE)[reg / 4] = val;
}

static inline uint32_t apic_read(uint32_t reg) {
    return ((volatile uint32_t*)APIC_VIRT_BASE)[reg / 4];
}

static inline void apic_send_eoi() {
    apic_write(APIC_EOI, 0);
}

static inline void ioapic_write(uint8_t reg, uint32_t val) {
    *(volatile uint32_t*)IOAPIC_REGSEL = reg;
    *(volatile uint32_t*)IOAPIC_WINDOW = val;
}

static inline uint32_t ioapic_read(uint8_t reg) {
    *(volatile uint32_t*)IOAPIC_REGSEL = reg;
    return *(volatile uint32_t*)IOAPIC_WINDOW;
}
