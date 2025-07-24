#ifndef MASKED_POLY_H
#define MASKED_POLY_H

#include "poly.h"
#include "masked.h"

typedef struct {
  poly share[MASKING_N];
} masked_poly;



void masked_poly_mask(masked_poly *r, const poly *a);

void masked_poly_unmask(poly *a, const masked_poly *r);

void masked_poly_ntt(masked_poly *r);

void masked_poly_uniform_gamma1(masked_poly *y, const masked_crh rhoprime, uint16_t nonce);

void masked_poly_pointwise(masked_poly *c, const poly *a, const masked_poly *b);

void masked_poly_pointwise_acc(masked_poly *c, const poly *a, const masked_poly *b);

void masked_poly_pointwise(masked_poly *c, const poly *a, const masked_poly *b);

void masked_poly_decompose(poly *v1, masked_poly *v0, const masked_poly *v);

#endif