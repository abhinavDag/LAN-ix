#include <stdint.h>
#include "../include/interrupts.h"
#include "../include/random.h"
#include "../include/timer.h"

static inline uint64_t interrupts_read_time(void) {
    uint32_t lo, hi;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

// called by ISR stubs with interrupt vector number
void interrupts_handle(uint8_t intno) {
    uint64_t t = interrupts_read_time();

    // mix minimal entropy
    rng_add_entropy((t << 8) ^ intno, 1);

    // later: dispatch to real handlers
}
void enable_interrupts() {
    __asm__ volatile("sti");
}

void disable_interrupts() {
    __asm__ volatile("cli");
}

