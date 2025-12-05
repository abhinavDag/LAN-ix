#include <stdint.h>
#include "../include/timer.h"
#include "../include/lottery.h"
#include "../include/interrupts.h"

extern void timer_isr_stub();

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void init_timer() {
    uint32_t freq = 50; // 50 Hz
    uint32_t divisor = 1193180 / freq;

    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}

void timer_handler() {
    lottery_schedule_tick();
}

