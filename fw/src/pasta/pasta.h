#ifndef PASTA_H
#define PASTA_H

#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "poly.h"

void pasta_sbox_feistel(poly *B, const poly *A);
void pasta_sbox_cube(poly *B, poly *A);
void pasta_calculate_row(uint32_t *C, const uint32_t *B, const poly *A);
void pasta_mix(poly *B_left, poly *B_right, const poly *A_left, const poly *A_right);
void pasta_matmul(poly *new_state, const poly *state, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr);
void pasta_round(poly *C, poly *D, const poly *A, const poly *B, uint64_t nonce, uint64_t block_ctr, size_t r);

void pasta_key_gen(int32_t *key, uint64_t *nonce, uint64_t *block_ctr);
void pasta_encrypt(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce);

#endif
