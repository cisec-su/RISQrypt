#ifndef PASTA_H
#define PASTA_H

#include <stdint.h>
#include "params.h"

void pasta_soft_sbox_feistel(poly *B, const poly *A);
void pasta_soft_sbox_cube(poly *B, const poly *A);
void pasta_soft_poly_pointwise_mult(poly *C, const poly *A, const poly *B);
void pasta_soft_poly_pointwise_add(poly *C, const poly *A, const poly *B);
void pasta_soft_calculate_row(poly *C, const poly *B, const poly *A);
void pasta_soft_mix(poly *B_left, poly *B_right, const poly *A_left, const poly *A_right);
void pasta_soft_encrypt(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce);
void pasta_soft_poly_uniform(poly *p, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr, int allow_zero);
void pasta_soft_matmul(poly *new_state, const poly *state, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr);
void pasta_soft_round(poly *C, poly *D, const poly *A, const poly *B, uint64_t nonce, uint64_t block_ctr, int r);

void pasta_soft_key_gen(int32_t *key, uint64_t *nonce, uint64_t *block_ctr);

#endif
