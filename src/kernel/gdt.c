#include "gdt.h"

// GDT 数据
__attribute__((aligned(0x1000))) GDTEntry gdt[3];
GDTPtr gdt_descriptor;

// 初始化 GDT
void init_gdt() {
    // null descriptor
    memset(&gdt[0], 0, sizeof(GDTEntry));

    // 64位代码段描述符
    gdt[1] = (GDTEntry){
        .limit_low = 0xFFFF,
        .base_low = 0x0000,
        .base_mid = 0x00,

        .type = 0b1010,         // execute/read
        .system = 1,            // 数据/代码段
        .dpl = 0,               // 特权级
        .present = 1,

        .limit_high = 0xF,
        .avl = 0,
        .l = 1,                 // 64位
        .db = 0,                // 64位
        .granularity = 1,       // 粒度 4KB

        .base_high = 0x00,
    };

    // 64位数据段描述符
    gdt[2] = gdt[1];              // 偷懒复制
    gdt[2].type = 0b0010;         // read/write
    gdt[2].l = 0;                 // 数据段 L 位必须为 0

    gdt_descriptor.limit = sizeof(gdt) - 1;
    gdt_descriptor.base = (uint64_t)&gdt;

    asm volatile ("lgdt %0" : : "m"(gdt_descriptor));
}

// 用汇编刷新段寄存器
void reload_segments() {
    __asm__ volatile (
        "mov $0x10, %%ax;"
        "mov %%ax, %%ds;"
        "mov %%ax, %%es;"
        "mov %%ax, %%ss;"
        "mov %%ax, %%fs;"
        "mov %%ax, %%gs;"

        // 通过 far jump 刷新 CS
        "pushq $0x08;"
        "leaq 1f(%%rip), %%rax;"
        "pushq %%rax;"
        "lretq;"
        "1:"
        ::: "rax"
    );
}
