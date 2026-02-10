#ifndef PASTA_H
#define PASTA_H

#include <stdint.h>
#include "params.h"
#include "poly.h"

void sbox_feistel_soft(poly *B, const poly *A);
void sbox_feistel(poly *B, const poly *A);
void sbox_cube_soft(poly *B, const poly *A);
void sbox_cube(poly *B, const poly *A);
void poly_pointwise_soft(poly *c, const poly *a, const poly *b);
void calculate_row_soft(poly *C, const poly *B, const poly *A);
void calculate_row(uint32_t *C, const uint32_t *B, const poly *A);
void mix_soft(poly *B_left, poly *B_right, const poly *A_left, const poly *A_right);
void mix(poly *B_left, poly *B_right, const poly *A_left, const poly *A_right);

// Random field element generation
uint64_t squeeze_u64(void);
uint32_t generate_random_field_element(int allow_zero);
void get_random_vector(poly *p, int allow_zero);
void reset_random_state(void);
void add_rc(poly *B, const poly *A);
void add_rc_soft(poly *B, const poly *A);
void pasta_round(poly *C, poly *D, const poly *A, const poly *B, int r);
void pasta_round_soft(poly *C, poly *D, const poly *A, const poly *B, int r);
void gen_keystream(poly *keystream, const int32_t *key);
void gen_keystream_soft(poly *keystream, const int32_t *key, uint64_t nonce, uint64_t block_counter);
void pasta_encrypt_one_block(poly *ciphertext, const poly *plaintext, const int32_t *key);
void pasta_encrypt_one_block_soft(poly *ciphertext, const poly *plaintext, const int32_t *key);

// Matrix multiplication
void matmul_soft(poly *new_state, const poly *state);
void matmul(poly *new_state, const poly *state);

#endif
