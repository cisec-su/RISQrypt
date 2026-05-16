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

void rubato_soft_sbox_feistel(poly *B, const poly *A);
void rubato_soft_linear_layer(poly *A, const poly *B);
void rubato_soft_poly_uniform(poly *p, uint64_t nonce, uint64_t block_ctr);
void rubato_soft_sampling(poly coeffs[RUBATO_R + 1], uint64_t nonce, uint64_t block_ctr);
void rubato_soft_encrypt(poly *ciphertext, const poly *plaintext, const poly *key, uint64_t nonce, uint64_t block_ctr);
#endif
