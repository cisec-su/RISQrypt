#ifndef MASKED_RUBATO_SOFT_H
#define MASKED_RUBATO_SOFT_H

#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "masked_poly_soft.h"

/* Masked RUBATO cipher (software) */
void masked_rubato_soft_sbox_feistel(masked_poly *B, const masked_poly *A);
void masked_rubato_soft_encrypt(poly *ciphertext, const poly *plaintext, const poly *key, uint64_t nonce, uint64_t block_ctr);

#endif
