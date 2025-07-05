#include "print.h"
#include "font.h"

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8
#define PADDING 16
#define LINE_SPACING 4

int32_t w_scaling=1, h_scaling=1;
int32_t w_scaled=8, h_scaled=8;

#define LINE_HEIGHT (h_scaled + LINE_SPACING)

void set_scaling(int32_t w, int32_t h){
    w_scaling = w;
    h_scaling = h;
    w_scaled = CHAR_WIDTH * w;
    h_scaled = CHAR_HEIGHT * h;
}

void putchar_at(char c, uint32_t x, uint32_t y, PIXEL fg, PIXEL bg) {
    if (c > 127){
        c = ' ';
    }

    uint64_t glyph = FONT[c];
    for (uint32_t row = 0; row < CHAR_HEIGHT; row++) {
        for (uint32_t col = 0; col < CHAR_WIDTH; col++) {
            BOOLEAN b = (glyph >> (row * 8 + col)) & 1;
            DrawRect(x+col*w_scaling, y+row*h_scaling,
                     x+(col+1)*w_scaling-1, y+(row+1)*h_scaling-1,
                     b? fg: bg);
            // DrawPixel(x+col, y+row, b? fg: bg);
        }
    }
}

uint32_t cursor_x = PADDING, cursor_y = PADDING;
PIXEL fg_color = {255, 255, 255, 255},
      bg_color = {0};

void putchar(char c) {
        if (c == '\n') {
        cursor_x = PADDING;
        cursor_y += LINE_HEIGHT;
        return;
    }

    putchar_at(c, cursor_x, cursor_y, fg_color, bg_color);
    cursor_x += w_scaled;

    if (cursor_y + LINE_HEIGHT > ScreenHeight - PADDING) {
        cursor_x = PADDING;
        cursor_y = PADDING;
        // TODO: 实现屏幕滚动
    } else if (cursor_x + w_scaled > ScreenWidth - PADDING) {
        cursor_x = PADDING;
        cursor_y += LINE_HEIGHT;
    }
}

void print(char *s) {
    for (int i=0; s[i]; i++){
        putchar(s[i]);
    }
}

inline void common_color(){
    fg_color = (PIXEL){255, 255, 255, 255};
}

inline void warning_color(){
    fg_color = (PIXEL){0, 255, 255, 255};
}

inline void error_color(){
    fg_color = (PIXEL){0, 0, 255, 255};
}

void raise_err(char *s) {
    error_color();
    print(s);
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void warning(char *s){
    warning_color();
    print(s);
    common_color();
}

void print_hex(uint64_t val){
    char buf[32];
    int len = 0;

    if (val == 0){
        len = 1;
        buf[0] = '0';
    }

    while (val){
        uint64_t nibble = val & 0xF;
        buf[len] = (nibble < 10)? ('0'+nibble) : ('A'+nibble-10);
        len++;
        val >>= 4;
    }

    print("0x");
    for (int i=len-1; i>=0; i--){
        putchar(buf[i]);
    }
}

void print_dec(int64_t val){
    if (val < 0){
        putchar('-');
        val = -val;
    }

    char buf[32];
    int len = 0;

    if (val == 0){
        len = 1;
        buf[0] = '0';
    }

    while (val){
        buf[len] = '0' + val % 10;
        val /= 10;
        len++;
    }

    for (int i=len-1; i>=0; i--){
        putchar(buf[i]);
    }
}
