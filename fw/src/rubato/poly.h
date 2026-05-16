#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"
#include "symmetric.h"

typedef struct {
  int32_t coeffs[N];
} poly;

void poly_init_q(void);
void poly_add(poly *c, const poly *a, const poly *b);
void poly_sub(poly *c, const poly *a, const poly *b);
void poly_pointwise(poly *c, const poly *a, const poly *b);
void poly_mult_add(poly *D, const poly *A, const poly *B, const poly *C);
void poly_uniform(poly *p, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr);
#endif