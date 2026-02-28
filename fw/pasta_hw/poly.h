#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"
#include "symmetric.h"

typedef struct {
  int32_t coeffs[N];
} poly;


void poly_add(poly *c, const poly *a, const poly *b);
void poly_sub(poly *c, const poly *a, const poly *b);
void poly_pointwise(poly *c, const poly *a, const poly *b);
void poly_pointwise_acc(poly *c, const poly *a, const poly *b);
void poly_init_q(void);
void poly_uniform(poly *a, uint64_t nonce, uint64_t block_ctr, uint8_t poly_ctr, int allow_zero, int to_hw);

#endif
