#include "keyboard.h"
#include "apic.h"
#include "stdint.h"
#include "io.h"
#include "print.h"
#include "isr.h"

#define KEYBOARD_PORT 0x60
#define KEYBOARD_VEC 0x21

// 简化版 US 键盘扫描码表（Set 1）
const char scancode_table[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8,
    '	', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ', ', '.', '/',  0, '*',
    0, ' ',
};

char translate_scancode(uint8_t scancode) {
    if (scancode >= 128) {
        return 0;  // 忽略释放键
    }
    return scancode_table[scancode];
}

void keyboard_handler(uint8_t vec, void *regs) {
    uint8_t scancode = inb(0x60);
    char c = translate_scancode(scancode);
    if (c) {
        putchar(c);
    }
    apic_send_eoi();
}


void init_keyboard(){
    ioapic_redirect_irq(1, KEYBOARD_VEC, 0);
    isr[KEYBOARD_VEC] = keyboard_handler;
}
