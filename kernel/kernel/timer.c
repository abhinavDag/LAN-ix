#include <stdint.h>
#include "../include/timer.h"
#include "../include/lottery.h"
#include "../include/interrupts.h"
#include "../include/random.h"    // ← add this
#include "../include/pic.h"

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

static inline uint64_t read_tsc() {
    uint32_t lo, hi;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

void timer_handler() {
    uint64_t t = read_tsc();
    rng_add_entropy((t << 8) ^ 32, 1);

    lottery_schedule_tick();

    /* send EOI for IRQ0 */
    pic_send_eoi(0);
}
