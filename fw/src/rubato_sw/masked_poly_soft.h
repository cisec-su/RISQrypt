#ifndef MASKED_POLY_H
#define MASKED_POLY_H

#include <stddef.h>
#include "params.h"

typedef struct {
  poly share[MASKING_N];
} masked_poly;

void masked_poly_soft_mult(masked_poly *C, const masked_poly *A, const masked_poly *B);
void masked_poly_soft_mult_add(masked_poly *D, const poly *A, const masked_poly *B, const masked_poly *C);
void masked_poly_soft_mask(masked_poly *r, const poly *a);
void masked_poly_soft_unmask(poly *a, const masked_poly *r);
void masked_poly_soft_unmask_n(poly *a, const masked_poly *r, size_t count);

#endif