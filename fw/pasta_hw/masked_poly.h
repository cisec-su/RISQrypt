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
void masked_poly_add_unmasked(masked_poly *r, const poly *a, const masked_poly *b);

#define masked_poly_add PASTA_NAMESPACE(masked_poly_add)
void masked_poly_add(masked_poly *r, const masked_poly *a, const masked_poly *b);

#define masked_poly_mult_add_const PASTA_NAMESPACE(masked_poly_mult_add_const)
void masked_poly_mult_add_const(masked_poly *r,const masked_poly *a, const masked_poly *b);

#define masked_poly_mult_mm PASTA_NAMESPACE(masked_poly_mult_mm)
void masked_poly_mult_mm(masked_poly *C, const masked_poly *A, const masked_poly *B);

#define masked_poly_sec_exp PASTA_NAMESPACE(masked_poly_sec_exp)
void masked_poly_sec_exp(masked_poly *B, const masked_poly *A, uint32_t e) ;

#define masked_poly_cube PASTA_NAMESPACE(masked_poly_cube)
void masked_poly_cube(masked_poly *B, const masked_poly *A);

#define masked_poly_square PASTA_NAMESPACE(masked_poly_square)
void masked_poly_square(masked_poly *B, const masked_poly *A);

#define masked_poly_mac PASTA_NAMESPACE(masked_poly_mac)
void masked_poly_mac( masked_poly *D, const masked_poly *A, const masked_poly *B, const masked_poly *C);

#define masked_poly_right_shift PASTA_NAMESPACE(masked_poly_right_shift)
void masked_poly_right_shift(masked_poly *B, const masked_poly *A, size_t shift_count);

#endif