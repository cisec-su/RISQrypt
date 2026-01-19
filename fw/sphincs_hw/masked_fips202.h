#ifndef MASKED_FIPS202_H
#define MASKED_FIPS202_H

#include <stdint.h>
#include <stddef.h>

#define N_SHARES 2

/*
 * State structure for Masked Keccak
 * The 1600-bit state (25 uint64_t lanes) is split into N_SHARES.
 */
typedef struct {
    uint64_t s[N_SHARES][25];
} masked_state_t;

static uint64_t load64(const uint8_t *x);
static void store64(uint8_t *x, uint64_t u);
static void sec_and(uint64_t *z0, uint64_t *z1, uint64_t x0, uint64_t x1, uint64_t y0, uint64_t y1);
static void theta(masked_state_t *state);
static void rho(masked_state_t *state);
static void pi(masked_state_t *state);
static void chi(masked_state_t *state);
static void iota(masked_state_t *state, int round);
void masked_keccak_f1600(masked_state_t *state);
void masked_keccak_inc_init(masked_state_t *state);
void masked_shake256(unsigned char *out1, unsigned char *out2, size_t outlen,
                     const unsigned char *in1, const unsigned char *in2, size_t inlen);
#endif
