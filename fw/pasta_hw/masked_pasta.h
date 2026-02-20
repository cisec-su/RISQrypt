#ifndef MASKED_PASTA_H
#define MASKED_PASTA_H

#include <stdint.h>
#include "params.h"
#include "poly.h"

void masked_pasta_encrypt_one_block(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce);

#endif
