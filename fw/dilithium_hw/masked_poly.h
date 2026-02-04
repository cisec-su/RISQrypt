#ifndef MASKED_POLY_H
#define MASKED_POLY_H

#include "poly.h"
#include "masked.h"

#define U 1 // UNALIGNMENT_OFFSET

typedef struct {
  poly share[MASKING_N];
} masked_poly;


typedef struct {
  uint32_t coeffs[N + U];
} poly_u;


typedef struct {
  poly_u share[MASKING_N];
} masked_poly_u;


// typedef struct {
//   poly *share[MASKING_N];
// } masked_poly_ptr;

typedef struct {
  poly_u *share[MASKING_N];
} masked_poly_ptr;



#define masked_poly_mask DILITHIUM_NAMESPACE(masked_poly_mask)
void masked_poly_mask(masked_poly *r, const poly *a);

#define masked_poly_unmask DILITHIUM_NAMESPACE(masked_poly_unmask)
void masked_poly_unmask(poly *a, const masked_poly *r);

#define masked_poly_ntt DILITHIUM_NAMESPACE(masked_poly_ntt)
void masked_poly_ntt(masked_poly *r);

#define masked_poly_invntt DILITHIUM_NAMESPACE(masked_poly_invntt)
void masked_poly_invntt(masked_poly *r);

#define masked_poly_uniform_gamma1_fromhw DILITHIUM_NAMESPACE(masked_poly_uniform_gamma1_fromhw)
void masked_poly_uniform_gamma1_fromhw(masked_poly *y, const masked_crh rhoprime, uint16_t nonce_next, int init_next);

#define masked_poly_pointwise DILITHIUM_NAMESPACE(masked_poly_pointwise)
void masked_poly_pointwise(masked_poly *c, const poly *a, const masked_poly *b);

#define masked_poly_pointwise_acc DILITHIUM_NAMESPACE(masked_poly_pointwise_acc)
void masked_poly_pointwise_acc(masked_poly *c, const poly *a, const masked_poly *b);

#define masked_poly_decompose DILITHIUM_NAMESPACE(masked_poly_decompose)
void masked_poly_decompose(poly *v1, masked_poly *v0, const masked_poly *v);

#define masked_poly_pointwise_add_invntt_chknorm DILITHIUM_NAMESPACE(masked_poly_pointwise_add_invntt_chknorm)
int masked_poly_pointwise_add_invntt_chknorm(masked_poly *r, const masked_poly *v, const poly *c, const masked_poly *u, uint32_t B);

#define masked_poly_pointwise_invntt_sub_chknorm DILITHIUM_NAMESPACE(masked_poly_pointwise_invntt_sub_chknorm)
int masked_poly_pointwise_invntt_sub_chknorm(masked_poly *r, const masked_poly *v, const poly *c, const masked_poly *u, uint32_t B);


#endif