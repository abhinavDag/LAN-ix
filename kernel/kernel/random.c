/* kernel/kernel/random.c */
#include "../include/random.h"
#include <stdint.h>
#include "../include/timer.h"
#include "../include/interrupts.h"
#include "../include/pic.h" /* optional */

/* Simple 64-bit pool + SplitMix64 mixer. Good enough for scheduler randomness. */

struct {
    uint64_t pool;
} rng_state;

/* SplitMix64 mixer */
static inline uint64_t splitmix64(uint64_t *x) {
    uint64_t z = (*x += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

void rng_init(void) {
    uint32_t lo, hi;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    rng_state.pool = ((uint64_t)hi << 32) | lo;
    rng_state.pool ^= (uint64_t)(uintptr_t)&rng_state;
    rng_state.pool = splitmix64(&rng_state.pool);
}

/* silence unused parameter warning */
void rng_add_entropy(uint64_t sample, int est_bits __attribute__((unused))) {
    rng_state.pool ^= sample;
    uint64_t tmp = rng_state.pool;
    tmp = splitmix64(&tmp);
    rng_state.pool ^= tmp;
    rng_state.pool = splitmix64(&rng_state.pool);
}

uint64_t rng_get_u64(void) {
    uint64_t out = splitmix64(&rng_state.pool);
    rng_state.pool ^= out;
    rng_state.pool = splitmix64(&rng_state.pool);
    return out;
}

/* Multiply-high: compute high 64 bits of a * b without using __int128.
   Implemented using 32-bit halves. */
static uint64_t mul_high_u64(uint64_t a, uint64_t b) {
    uint32_t a_lo = (uint32_t)a;
    uint32_t a_hi = (uint32_t)(a >> 32);
    uint32_t b_lo = (uint32_t)b;
    uint32_t b_hi = (uint32_t)(b >> 32);

    uint64_t p0 = (uint64_t)a_lo * b_lo;     // 64-bit
    uint64_t p1 = (uint64_t)a_lo * b_hi;     // 64-bit
    uint64_t p2 = (uint64_t)a_hi * b_lo;     // 64-bit
    uint64_t p3 = (uint64_t)a_hi * b_hi;     // 64-bit

    // Combine the cross terms
    uint64_t mid = (p0 >> 32) + (p1 & 0xFFFFFFFFULL) + (p2 & 0xFFFFFFFFULL);
    uint64_t carry = mid >> 32;

    uint64_t hi = p3 + (p1 >> 32) + (p2 >> 32) + carry;
    return hi;
}

/* rng_get_range: map 64-bit RNG to [0, n) without division/modulo.
   Uses multiply-high trick: floor( (x * n) / 2^64 ). */
uint64_t rng_get_range(uint64_t n) {
    if (n == 0) return 0;
    uint64_t x = rng_get_u64();
    uint64_t result = mul_high_u64(x, n);
    return result;
}

