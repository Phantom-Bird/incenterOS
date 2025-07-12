#pragma once

#define KB (1ull << 10)
#define MB (1ull << 20)
#define GB (1ull << 30)
#define TB (1ull << 40)

#define APIC_DEFAULT_BASE      0xFEE00000
#define IOAPIC_DEFAULT_BASE    0xFEC00000

#define APIC_VIRT_BASE      0xFFFFFFFFFEE00000
#define IOAPIC_VIRT_BASE    0xFFFFFFFFFEC00000
#define FB_VIRT_BASE        0xFFFFFFFFC0000000
#define STACK_VIRT_BASE     0xFFFFFFFF80000000
#define INITRAMFS_POOL_VIRT 0xFFFFFFF800000000

#define HIGH_ADDR           0xFFFF800000000000
#define PHYS_MAP_MAX        (8 * TB)
#define HIGH_ADDR_END       (HIGH_ADDR + PHYS_MAP_MAX)
