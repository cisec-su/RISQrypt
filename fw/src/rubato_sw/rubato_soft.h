#ifndef RUBATO_H
#define RUBATO_H

#include <stdint.h>
#include "params.h"

/**
 * @brief Modular multiplication
 * @description Multiply two coefficients and reduce modulo Q.
 * @param a First coefficient (< Q)
 * @param b Second coefficient (< Q)
 * @return Product reduced modulo Q
 */
static inline uint32_t mod_q_mult(uint32_t a, uint32_t b) {
    return (uint32_t)(((uint64_t)a * (uint64_t)b) % (uint64_t)Q);
}
#define MOD_Q_MULT(a, b) mod_q_mult((a), (b))

/**
 * @brief Branch-free conditional subtraction of Q.
 * @description Maps x in [0, 2Q) to x mod Q in [0, Q). Computes d = x - Q and
 *              adds Q back when d is negative, using the sign mask d >> 31.
 *              The comparison form (0 - (x >= Q)) is not used on purpose: GCC
 *              turns it back into a branch (bleu) on rv32im at -O2/-O3.
 * @param x Value in [0, 2Q).
 * @return x mod Q, in [0, Q).
 */
static inline uint32_t csub_q(uint32_t x) {
    int32_t d = (int32_t)(x - (uint32_t)Q);
    d += Q & (d >> 31);
    return (uint32_t)d;
}

void rubato_soft_sbox_feistel(poly *B, const poly *A);
void rubato_soft_linear_layer(poly *A, const poly *B);
void rubato_soft_poly_uniform(poly *p, uint64_t nonce, uint64_t block_ctr);
void rubato_soft_sampling(poly coeffs[RUBATO_R + 1], uint64_t nonce, uint64_t block_ctr);
void rubato_soft_getnoise(poly *e, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr);
void rubato_soft_encrypt(poly *ciphertext, const poly *plaintext, const poly *key, uint64_t nonce, uint64_t block_ctr);
#endif
