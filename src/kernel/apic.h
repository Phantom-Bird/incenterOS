#pragma once
#include <stdint.h>

#define APIC_DEFAULT_BASE 0xFEE00000
#define IOAPIC_DEFAULT_BASE  0xFEC00000

void apic_enable();
void ioapic_redirect_irq(uint8_t irq, uint8_t vector, uint8_t apic_id);

// 以下是 APIC 工具函数

#define APIC_EOI 0xB0
#define IOAPIC_REGSEL  0xFEC00000
#define IOAPIC_WINDOW  0xFEC00010

static inline void apic_write(uint32_t reg, uint32_t val) {
    ((volatile uint32_t*)APIC_DEFAULT_BASE)[reg / 4] = val;
}

static inline uint32_t apic_read(uint32_t reg) {
    return ((volatile uint32_t*)APIC_DEFAULT_BASE)[reg / 4];
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
