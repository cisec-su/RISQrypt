#include "swmasked_fips202.h"
#include <string.h>
#include "randombytes.h"

// Rotation constants for Keccak Rho step
static const uint8_t KeccakRhoOffsets[24] = {
    1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 2, 14, 27, 41, 56, 8, 25, 43, 62, 18, 39, 61, 20, 44
};

// Round constants for Iota step
static const uint64_t KeccakRoundConstants[24] = {
    0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL,
    0x8000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
    0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL,
    0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
    0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL,
    0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
    0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL,
    0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

#define ROL64(a, offset) ((((uint64_t)a) << offset) | (((uint64_t)a) >> (64 - offset)))
#define SHAKE256_RATE 136

// Helper to handle unaligned loads if your platform needs it
static uint64_t load64(const uint8_t *x) {
    uint64_t r = 0;
    for (int i = 0; i < 8; ++i) r |= (uint64_t)x[i] << (8 * i);
    return r;
}

// Helper to store output
static void store64(uint8_t *x, uint64_t u) {
    for (int i = 0; i < 8; ++i) x[i] = u >> (8 * i);
}

/*
 * Secure AND (ISW Gadget for 2 shares)
 * z = x & y
 * z0 ^ z1 = (x0 ^ x1) & (y0 ^ y1)
 */
static void sec_and(uint64_t *z0, uint64_t *z1, uint64_t x0, uint64_t x1, uint64_t y0, uint64_t y1) {
    uint64_t r;
    // We need 64 bits of randomness for the resharing
    // In a real embedded implementation, use a fast TRNG or PRNG reference
    randombytes((uint8_t*)&r, sizeof(r));

    // z0 = x0*y0 ^ r
    // z1 = x1*y1 ^ (r ^ x0*y1 ^ x1*y0) OR similar ISW formula
    
    // Standard ISW for 2 shares:
    // r is random.
    // z0 = (x0 & y0) ^ r;
    // z1 = (x1 & y1) ^ r ^ (x0 & y1) ^ (x1 & y0);
    
    *z0 = (x0 & y0) ^ r;
    *z1 = (x1 & y1) ^ r ^ (x0 & y1) ^ (x1 & y0);
}

static void theta(swmasked_state_t *state) {
    unsigned int x, y, s;
    uint64_t C[N_SHARES][5];
    uint64_t D[N_SHARES][5];

    // C[x] = A[x,0] ^ A[x,1] ^ A[x,2] ^ A[x,3] ^ A[x,4]
    for (s = 0; s < N_SHARES; ++s) {
        for (x = 0; x < 5; ++x) {
            C[s][x] = state->s[s][x] ^ state->s[s][x + 5] ^ state->s[s][x + 10] ^ state->s[s][x + 15] ^ state->s[s][x + 20];
        }
    }

    // D[x] = C[x-1] ^ ROL(C[x+1], 1)
    for (s = 0; s < N_SHARES; ++s) {
        for (x = 0; x < 5; ++x) {
            D[s][x] = C[s][(x + 4) % 5] ^ ROL64(C[s][(x + 1) % 5], 1);
        }
    }

    // A[x,y] ^= D[x]
    for (s = 0; s < N_SHARES; ++s) {
        for (x = 0; x < 5; ++x) {
            for (y = 0; y < 5; ++y) {
                state->s[s][x + 5 * y] ^= D[s][x];
            }
        }
    }
}

static void rho(swmasked_state_t *state) {
    unsigned int x = 1, y = 0;
    
    // Iterate through the 24 offsets (skipping 0,0 which stays 0)
    for (int t = 0; t < 24; t++) {
        unsigned int index = x + 5 * y;
        
        // Apply rotation to ALL shares at this index
        for (int s = 0; s < N_SHARES; ++s) {
            state->s[s][index] = ROL64(state->s[s][index], KeccakRhoOffsets[t]);
        }

        // Calculate the next coordinate in the sequence
        unsigned int old_x = x;
        x = y;
        y = (2 * old_x + 3 * y) % 5;
    }
}

static void pi(swmasked_state_t *state) {
    uint64_t temp[N_SHARES][25];
    unsigned int x, y, s;

    // 1. Calculate the permutation into 'temp'
    for (x = 0; x < 5; ++x) {
        for (y = 0; y < 5; ++y) {
            // Standard Keccak Pi formula:
            // A'[y, 2x+3y] = A[x, y]
            unsigned int src_idx = x + 5 * y;
            unsigned int dst_idx = y + 5 * ((2 * x + 3 * y) % 5);

            for (s = 0; s < N_SHARES; ++s) {
                temp[s][dst_idx] = state->s[s][src_idx];
            }
        }
    }

    // 2. Copy 'temp' back to the main state
    for (s = 0; s < N_SHARES; ++s) {
        memcpy(state->s[s], temp[s], 25 * sizeof(uint64_t));
    }
}

static void chi(swmasked_state_t *state) {
    unsigned int j;
    uint64_t C[N_SHARES][5]; // Slices

    for (int y = 0; y < 5; ++y) { 
        // For each row
        // Load row into C
        for (int s = 0; s < N_SHARES; ++s) {
            for (int x = 0; x < 5; ++x) {
                C[s][x] = state->s[s][x + 5 * y];
            }
        }

        // Apply Chi on the row
        // A[x] = A[x] ^ ( (~A[x+1]) & A[x+2] )
        // Let B = ~A[x+1] & A[x+2]
        // Note: ~A[x+1] in masked domain:
        // (~A_0, A_1, A_2...) - only negate ONE share.
        
        for (int x = 0; x < 5; ++x) {
            uint64_t not_next_0 = ~C[0][(x + 1) % 5];
            uint64_t not_next_1 =  C[1][(x + 1) % 5]; // Only negate 0-th share
            
            uint64_t next_next_0 = C[0][(x + 2) % 5];
            uint64_t next_next_1 = C[1][(x + 2) % 5];

            uint64_t z0, z1;
            sec_and(&z0, &z1, not_next_0, not_next_1, next_next_0, next_next_1);

            state->s[0][x + 5 * y] ^= z0;
            state->s[1][x + 5 * y] ^= z1;
        }
    }
}

static void iota(swmasked_state_t *state, int round) {
    // Only apply to first share
    state->s[0][0] ^= KeccakRoundConstants[round];
}

void swmasked_keccak_f1600(swmasked_state_t *state) {
    for (int i = 0; i < 24; ++i) {
        theta(state);
        rho(state); 
        pi(state);
        chi(state);
        iota(state, i);
    }
}

void swmasked_keccak_inc_init(swmasked_state_t *state) {
    memset(state, 0, sizeof(swmasked_state_t));
}


void swmasked_shake256(unsigned char *out1, unsigned char *out2, size_t outlen,
                     const unsigned char *in1, const unsigned char *in2, size_t inlen)
{
    swmasked_state_t state;
    swmasked_keccak_inc_init(&state);

    // 1. ABSORB PHASE
    while (inlen >= SHAKE256_RATE) {
        // XOR input blocks into the state (Rate part only)
        for (int i = 0; i < SHAKE256_RATE / 8; i++) {
            state.s[0][i] ^= load64(in1 + 8 * i);
            state.s[1][i] ^= load64(in2 + 8 * i);
        }
        
        // Permute
        swmasked_keccak_f1600(&state);
        
        in1 += SHAKE256_RATE;
        in2 += SHAKE256_RATE;
        inlen -= SHAKE256_RATE;
    }

    // 2. ABSORB REMAINDER (Partial Block)
    unsigned char t1[SHAKE256_RATE] = {0};
    unsigned char t2[SHAKE256_RATE] = {0};

    // Copy remaining input bytes
    for (size_t i = 0; i < inlen; i++) {
        t1[i] = in1[i];
        t2[i] = in2[i];
    }

    // 3. PADDING (0x1F)
    // We must apply padding to ONE share only (usually share 0) to maintain consistency.
    // The other share gets XORed with 0.
    t1[inlen] = 0x1F;
    t1[SHAKE256_RATE - 1] |= 0x80;
    // t2 remains 0 at these positions because Pad ^ 0 = Pad

    // XOR the padded block into state
    for (int i = 0; i < SHAKE256_RATE / 8; i++) {
        state.s[0][i] ^= load64(t1 + 8 * i);
        state.s[1][i] ^= load64(t2 + 8 * i);
    }

    // Final Permutation before squeezing
    swmasked_keccak_f1600(&state);

    // 4. SQUEEZE PHASE
    size_t out_offset = 0;
    while (out_offset < outlen) {
        size_t bytes_to_squeeze = outlen - out_offset;
        if (bytes_to_squeeze > SHAKE256_RATE) {
            bytes_to_squeeze = SHAKE256_RATE;
        }

        // Extract bytes from state
        for (size_t i = 0; i < bytes_to_squeeze; i++) {
            // Need to extract byte i from word (i/8)
            uint64_t w1 = state.s[0][i / 8];
            uint64_t w2 = state.s[1][i / 8];
            
            uint8_t b1 = (w1 >> (8 * (i % 8))) & 0xFF;
            uint8_t b2 = (w2 >> (8 * (i % 8))) & 0xFF;

            out1[out_offset + i] = b1;
            out2[out_offset + i] = b2;
        }

        out_offset += bytes_to_squeeze;

        // If we need more output, permute again
        if (out_offset < outlen) {
            swmasked_keccak_f1600(&state);
        }
    }
}
