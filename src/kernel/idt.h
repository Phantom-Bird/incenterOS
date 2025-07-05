#include <stdint.h>

// IDT 描述符结构（16 字节）
typedef struct __attribute__((packed)) {
    uint16_t offset_low;        // bits 0..15
    uint16_t selector;          // 代码段选择子
    uint8_t  ist;               // 低三位为中断栈表索引（0）

    uint8_t  type       : 4;    // 门类型 (0xE=中断门, 0xF=陷阱门)
    uint8_t  ignored    : 1;
    uint8_t  dpl        : 2;    // 描述符特权级
    uint8_t  present    : 1;

    uint16_t offset_mid;        // bits 16..31
    uint32_t offset_high;       // bits 32..63
    uint32_t reserved;
} IDTDescriptor;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint64_t base;
} IDTPointer;

void set_idt_entry(int vec, void (*handler)(), uint16_t selector, uint8_t type, uint8_t dpl);
void lidt(void* base, uint16_t size);
void init_idt();
