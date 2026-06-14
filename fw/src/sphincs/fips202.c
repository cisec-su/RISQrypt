#include <stddef.h>
#include <stdint.h>
#include "fips202.h"
#include "keccak.h"

static void hw_absorb(const uint8_t *input, size_t inlen) {
    // transfer aligned words in bulk
    size_t words = inlen >> 2;
    if (words > 0) {
        keccak_absorb((const uint32_t*)input, NULL, words);
    }
}

static void hw_finalize(void) {
    volatile uint32_t t = SHAKE_PAD;
    keccak_finish((uint32_t*)&t);
}

static void hw_squeeze(uint8_t *output, size_t outlen) {
    size_t words = outlen >> 2;
    if (words > 0) {
        keccak_squeeze((uint32_t*)output, NULL, words);
    }
}

// SHAKE128
void shake128_inc_init(uint32_t *s_inc) {
    (void)s_inc;
    keccak_init(SHAKE128_RATE >> 3, KECCAK_MASK_DIS);
}

void shake128_inc_absorb(uint32_t *s_inc, const uint8_t *input, size_t inlen) {
    (void)s_inc;
    hw_absorb(input, inlen);
}

void shake128_inc_finalize(uint32_t *s_inc) {
    (void)s_inc;
    hw_finalize();
}

void shake128_inc_squeeze(uint8_t *output, size_t outlen, uint32_t *s_inc) {
    (void)s_inc;
    hw_squeeze(output, outlen);
}

void shake128_absorb(uint32_t *s, const uint8_t *input, size_t inlen) {
    shake128_inc_init(s);
    shake128_inc_absorb(s, input, inlen);
}

void shake128_squeezeblocks(uint8_t *output, size_t nblocks, uint32_t *s) {
    (void)s;
    hw_finalize();
    hw_squeeze(output, nblocks * SHAKE128_RATE);
}

void shake128(uint8_t *output, size_t outlen, const uint8_t *input, size_t inlen) {
    volatile uint32_t t;
    keccak_init(SHAKE128_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((const uint32_t*)input, NULL, inlen >> 2);
    t = SHAKE_PAD;
    keccak_finish((uint32_t*)&t);
    keccak_squeeze((uint32_t*)output, NULL, outlen >> 2);
    return;
}

// SHAKE256
void shake256_inc_init(uint32_t *s_inc) {
    (void)s_inc;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
}

void shake256_inc_absorb(uint32_t *s_inc, const uint8_t *input, size_t inlen) {
    (void)s_inc;
    hw_absorb(input, inlen);
}

void shake256_inc_finalize(uint32_t *s_inc) {
    (void)s_inc;
    hw_finalize();
}

void shake256_inc_squeeze(uint8_t *output, size_t outlen, uint32_t *s_inc) {
    (void)s_inc;
    hw_squeeze(output, outlen);
}

void shake256_absorb(uint32_t *s, const uint8_t *input, size_t inlen) {
    shake256_inc_init(s);
    shake256_inc_absorb(s, input, inlen);
}

void shake256_squeezeblocks(uint8_t *output, size_t nblocks, uint32_t *s) {
    (void)s;
    hw_finalize();
    hw_squeeze(output, nblocks * SHAKE256_RATE);
}

void shake256(uint8_t *output, size_t outlen, const uint8_t *input, size_t inlen) {
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((const uint32_t*)input, NULL, inlen >> 2);
    t = SHAKE_PAD;
    keccak_finish((uint32_t*)&t);
    keccak_squeeze((uint32_t*)output, NULL, outlen >> 2);
    return;
} 
