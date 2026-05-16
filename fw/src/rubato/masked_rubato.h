#ifndef MASKED_RUBATO_H
#define MASKED_RUBATO_H

#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "masked_poly.h"
#include "poly.h"

void masked_rubato_sbox_feistel(masked_poly *B, const masked_poly *A);
void masked_rubato_encrypt(poly *ciphertext, const poly *plaintext, poly *key, uint64_t nonce, uint64_t block_ctr);
#endif