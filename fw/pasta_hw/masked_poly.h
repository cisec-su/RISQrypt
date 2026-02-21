#ifndef MASKED_POLY_H
#define MASKED_POLY_H

#include "poly.h"

typedef struct {
  poly share[MASKING_N];
} masked_poly;

#define masked_poly_mask PASTA_NAMESPACE(masked_poly_mask)
void masked_poly_mask(masked_poly *r, const poly *a);

#define masked_poly_unmask PASTA_NAMESPACE(masked_poly_unmask)
void masked_poly_unmask(poly *a, const masked_poly *r);

#define masked_poly_add_unmasked PASTA_NAMESPACE(masked_poly_add_unmasked)
void masked_poly_add_unmasked(masked_poly *r, poly *a, masked_poly *b);

#define masked_poly_mult_add_const PASTA_NAMESPACE(masked_poly_mult_add_const)
void masked_poly_mult_add_const(masked_poly *r, masked_poly *a, masked_poly *b);


#endif