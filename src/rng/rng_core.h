#ifndef XMC_RNG_CORE_H
#define XMC_RNG_CORE_H

#include <stdint.h>

/* xoshiro256 PRNG. Public domain (CC0) by Blackman & Vigna.
 * Reference: http://prng.di.unimi.it/xoshiro256starstar.c
 * State = 4 x uint64_t.
 * Period = 2^256 - 1.
 */

typedef struct {
    uint64_t s[4];
} xmc_xoshiro_state_t;

/* Seed via SplitMix64 (recommended by authors). */
void xmc_xoshiro_seed(xmc_xoshiro_state_t *state, uint64_t seed);

/* Next 64-bit output. */
uint64_t xmc_xoshiro_next(xmc_xoshiro_state_t *state);

/* Uniform double in [0, 1) using top 53 bits. */
double xmc_xoshiro_next_double(xmc_xoshiro_state_t *state);

/* Uniform uint64 in [0, bound) via Lemire's method. */
uint64_t xmc_xoshiro_next_bounded(xmc_xoshiro_state_t *state, uint64_t bound);

#endif /* XMC_RNG_CORE_H */
