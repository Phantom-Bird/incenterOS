#include "timer.h"
#include "apic.h"
#include "print.h"
#include "isr.h"

#define LAPIC_REG_TIMER        0x320
#define LAPIC_REG_TIMER_INIT   0x380
#define LAPIC_REG_TIMER_CURR   0x390
#define LAPIC_REG_TIMER_DIV    0x3E0

#define LAPIC_TIMER_MODE_PERIODIC (1 << 17)
#define LAPIC_TIMER_VECTOR 0x20

void init_lapic_timer() {
    apic_write(LAPIC_REG_TIMER_DIV, 0b0011); // Divide by 16
    apic_write(LAPIC_REG_TIMER, LAPIC_TIMER_VECTOR | LAPIC_TIMER_MODE_PERIODIC); // 中断向量: 0x20, 模式: 周期性
    apic_write(LAPIC_REG_TIMER_INIT, 64e4);  // // 设置初始计数器（在 QEMU 下刚好为 10ms）

    print("[KERNEL] Local APIC Timer initialized\n");
}

int64_t time=0;

void timer_handler(uint8_t vec, void *regs){
    // print("[TIMER] ");
    // print_dec(time);
    // print("\r");
    time += 10;
    apic_send_eoi();
}

void init_timer(){
    init_lapic_timer();
    isr[LAPIC_TIMER_VECTOR] = timer_handler;
}
