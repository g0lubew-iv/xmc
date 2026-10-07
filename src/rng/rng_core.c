#include "rng/rng_core.h"

/* SplitMix64: only used for seeding. */
static uint64_t
splitmix64(uint64_t *x) {
    uint64_t z = (*x += 0x9E3779B97F4A7C15ULL);

    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;

    return z ^ (z >> 31);
}

static uint64_t
rotl(uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

void
xmc_xoshiro_seed(xmc_xoshiro_state_t *state, uint64_t seed) {
    uint64_t x = seed;
    state->s[0] = splitmix64(&x);
    state->s[1] = splitmix64(&x);
    state->s[2] = splitmix64(&x);
    state->s[3] = splitmix64(&x);
}

uint64_t
xmc_xoshiro_next(xmc_xoshiro_state_t *state) {
    uint64_t *s = state->s;
    uint64_t const result = rotl(s[1] * 5, 7) * 9;
    uint64_t const t = s[1] << 17;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];
    s[2] ^= t;

    s[3]  = rotl(s[3], 45);

    return result;
}

double
xmc_xoshiro_next_double(xmc_xoshiro_state_t *state) {
    /* Top 53 bits -> [0, 1). 2^-53 = 1 / 9007199254740992. */
    uint64_t x = xmc_xoshiro_next(state);
    return (double) (x >> 11) * (1.0 / 9007199254740992.0);
}

uint64_t
xmc_xoshiro_next_bounded(xmc_xoshiro_state_t *state, uint64_t bound) {
    /* Lemire's nearly divisionless method. */
    if (bound == 0) {
        return 0;
    }

    uint64_t    x = xmc_xoshiro_next(state);
    __uint128_t m = (__uint128_t) x * (__uint128_t) bound;
    uint64_t    l = (uint64_t) m;

    if (l < bound) {
        uint64_t t = (uint64_t)(-bound) % bound;
        while (l < t) {
            x = xmc_xoshiro_next(state);
            m = (__uint128_t) x * (__uint128_t) bound;
            l = (uint64_t) m;
        }
    }
    
    return (uint64_t)(m >> 64);
}
