#pragma once
#include <stdint.h>

void rng_init(void);
void rng_add_entropy(uint64_t sample, int est_bits);
uint64_t rng_get_u64(void);
uint64_t rng_get_range(uint64_t n);

