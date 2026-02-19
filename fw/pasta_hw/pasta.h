#ifndef PASTA_H
#define PASTA_H

#include <stdint.h>
#include "params.h"
#include "poly.h"

void sbox_feistel(poly *B, const poly *A);
void sbox_cube(poly *B, const poly *A);
void calculate_row(uint32_t *C, const uint32_t *B, const poly *A);
void mix(poly *B_left, poly *B_right, const poly *A_left, const poly *A_right);
void matmul(poly *new_state, const poly *state, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr);
void pasta_round(poly *C, poly *D, const poly *A, const poly *B, uint64_t nonce, uint64_t block_ctr, int r);
void pasta_encrypt_one_block(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce);

#endif
