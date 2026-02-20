#include "stdint.h"
#include "randombytes.h"


#ifdef TEST_RNG


// 48 byte seed
static const uint8_t seed[48] = {
    0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,
    0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0, 0x00,
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x01,
    0x13, 0x26, 0x39, 0x4C, 0x5F, 0x72, 0x85, 0x98,
    0xAB, 0xBE, 0xD1, 0xE4, 0xF7, 0x0A, 0x1D, 0x2F
};

int randombytes(uint8_t *x, unsigned int xlen) {
    for (int i = 0; i < xlen; i++) {
        x[i] = seed[i % 48];
    }
    return 0;
}


void rng_init() {
    // Do nothing for test RNG
}


#else

#include "x2x.h"


int randombytes(uint8_t *x, unsigned int xlen) {
    uint32_t temp[16];
    unsigned int i;
    uint32_t out_len = xlen >> 2;

    x2x_prng_read(temp, 16);

    for (i = 0; i < out_len; i++) {
        ((uint32_t*) x)[i] = temp[i];
    }
    return 0;
}


void rng_init() {
    x2x_set_modulus(0xffffffff, 0, X2X_MODULUS_POW2, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_DIS);
}


#endif