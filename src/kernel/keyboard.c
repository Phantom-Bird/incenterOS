#include "keyboard.h"
#include "apic.h"
#include "stdint.h"
#include "io.h"
#include "print.h"
#include "isr.h"

#define KEYBOARD_PORT 0x60
#define KEYBOARD_VEC 0x21

char translate_scancode(uint8_t scancode);

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


#define LSHIFT_PRESS     0x2A
#define LSHIFT_RELEASE   0xAA
#define RSHIFT_PRESS     0x36
#define RSHIFT_RELEASE   0xB6
#define CAPSLOCK_PRESS   0x3A
#define CAPSLOCK_RELEASE 0xBA

const char scancode_table[2][128] = {{
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8,
    '	', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',  0, '*',
    0, ' ',
}, {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 8,
    '	', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',  0, '*',
    0, ' ',
}};

uint8_t lshift, rshift, capslock;

char translate_scancode(uint8_t scancode) {
    lshift = lshift && (scancode != LSHIFT_RELEASE) || (scancode == LSHIFT_PRESS);  // 注意运算优先级
    rshift = rshift && (scancode != RSHIFT_RELEASE) || (scancode == RSHIFT_PRESS);
    capslock = capslock ^ (scancode == CAPSLOCK_RELEASE);

    if (scancode >= 128) {
        return 0;  // 忽略松开按键
    }

    uint8_t shift = lshift || rshift;
    if ('a' <= scancode_table[0][scancode] && scancode_table[0][scancode] <= 'z'){
        shift ^= capslock;
    }

    return scancode_table[shift][scancode];
}
