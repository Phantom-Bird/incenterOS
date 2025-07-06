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
    // 设置分频器（Divide by 16）
    apic_write(LAPIC_REG_TIMER_DIV, 0b0011); // Divide by 16

    // 设置中断向量和模式（周期性）
    apic_write(LAPIC_REG_TIMER, LAPIC_TIMER_VECTOR | LAPIC_TIMER_MODE_PERIODIC);

    // 设置初始计数器（越大越慢）
    apic_write(LAPIC_REG_TIMER_INIT, 1000000);  // 可调节此值

    print("[LAPIC] Local APIC Timer initialized (approximate 100Hz)\n");
}

int64_t time=0;

void timer_handler(uint8_t vec, void *regs){
    // print("[TIMER] t = ");
    // print_dec(time);
    // print("ms\r");
    time += 10;
    apic_send_eoi();
}

void init_timer(){
    init_lapic_timer();
    isr[LAPIC_TIMER_VECTOR] = timer_handler;
}