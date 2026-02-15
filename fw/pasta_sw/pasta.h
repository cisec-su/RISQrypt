#ifndef PASTA_H
#define PASTA_H

#include <stdint.h>
#include "params.h"

void sbox_feistel_soft(poly *B, const poly *A);
void sbox_cube_soft(poly *B, const poly *A);
void poly_pointwise_soft(poly *c, const poly *a, const poly *b);
void calculate_row_soft(poly *C, const poly *B, const poly *A);
void mix_soft(poly *B_left, poly *B_right, const poly *A_left, const poly *A_right);

void pasta_encrypt_one_block_soft(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce);
void poly_uniform_soft(poly *p, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr, int allow_zero);

void matmul_soft(poly *new_state, const poly *state, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr);
void pasta_round_soft(poly *C, poly *D, const poly *A, const poly *B, uint64_t nonce, uint64_t block_ctr, int r);

#endif
