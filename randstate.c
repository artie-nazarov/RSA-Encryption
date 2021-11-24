#include "randstate.h"

gmp_randstate_t state;

// Initialize random state
void randstate_init(uint64_t seed) {
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
    return;
}

// Clear random state memory
void randstate_clear(void) {
    gmp_randclear(state);
    return;
}
