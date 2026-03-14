#ifndef MASKED_PASTA_SOFT_H
#define MASKED_PASTA_SOFT_H

#include <stdint.h>
#include "params.h"
#include "masked_poly.h"

void masked_pasta_soft_encrypt(poly *ciphertext, const poly *plaintext, const int32_t *key, uint64_t nonce);

#endif
