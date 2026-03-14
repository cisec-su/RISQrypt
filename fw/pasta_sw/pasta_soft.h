#ifndef PASTA_H
#define PASTA_H

#include <stdint.h>
#include "params.h"

#ifdef USE_BARRETT
static inline uint32_t barrett_reduce(uint64_t x) {
    int32_t r = (int32_t)(x & 0xFFFF) - (int32_t)(x >> 16);
    if (r < 0) r += Q;
    return (uint32_t)r;
}
#define MOD_Q(x) barrett_reduce((uint64_t)(x))

/* Reduce accumulator of up to N unreduced products, each < (Q-1)^2.
 * Valid when x < 2^40 (e.g., N=128, Q=65537: 128*(Q-1)^2 < 2^39).
 * Uses 2^16 ≡ -1 (mod Q)  =>  2^32 ≡ 1 (mod Q). */
static inline uint32_t barrett_reduce_acc(uint64_t x) {
    int32_t r;
    r  = (int32_t)(x & 0xFFFF);
    r -= (int32_t)((x >> 16) & 0xFFFF);
    r += (int32_t)(x >> 32);
    if (r < 0) r += Q;
    if (r >= Q) r -= Q;
    return (uint32_t)r;
}
#define MOD_Q_ACC(x) barrett_reduce_acc((uint64_t)(x))

/* Lightweight modular addition: reduce sum of two coefficients each < Q.
 * Result is always < 2*Q, so single conditional subtraction suffices. */
static inline uint32_t mod_q_add(uint32_t a, uint32_t b) {
    uint32_t sum = a + b;
    return (sum >= Q) ? (sum - Q) : sum;
}
#define MOD_Q_ADD(a, b) mod_q_add((a), (b))
#else
#define MOD_Q(x)       ((x) % Q)
#define MOD_Q_ACC(x)   ((x) % Q)
#define MOD_Q_ADD(a, b) (((a) + (b)) % Q)
#endif

extern const uint32_t PASTA_KEY[];
extern const uint32_t PASTA_PLAINTEXT[];
extern const uint64_t PASTA_NONCE;
extern const uint64_t PASTA_BLOCK_CTR;

void pasta_soft_sbox_feistel(poly *B, const poly *A);
void pasta_soft_sbox_cube(poly *B, const poly *A);
void pasta_soft_poly_pointwise_mult(poly *C, const poly *A, const poly *B);
void pasta_soft_poly_pointwise_add(poly *C, const poly *A, const poly *B);
void pasta_soft_poly_pointwise_sub(poly *C, const poly *A, const poly *B);
void pasta_soft_calculate_row(int32_t *C, const int32_t *B, const poly *A);
void pasta_soft_mix(poly *B_left, poly *B_right, const poly *A_left, const poly *A_right);
void pasta_soft_encrypt(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce);
void pasta_soft_poly_uniform(poly *p, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr, int allow_zero);
void pasta_soft_matmul(poly *new_state, const poly *state, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr);
void pasta_soft_round(poly *C, poly *D, const poly *A, const poly *B, uint64_t nonce, uint64_t block_ctr, int r);
void pasta_soft_key_gen(int32_t *key, uint64_t *nonce, uint64_t *block_ctr);

#endif
