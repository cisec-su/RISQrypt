#ifndef RUBATO_H
#define RUBATO_H

#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "poly.h"

void rubato_sbox_feistel(poly *B, const poly *A);
void rubato_linear_layer(poly *A, const poly *B);
void rubato_encrypt(poly *ciphertext, const poly *plaintext, poly *key, uint64_t nonce, uint64_t block_ctr);
void rubato_key_gen(poly *key, uint64_t *nonce, uint64_t *block_ctr);
#endif