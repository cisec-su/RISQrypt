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


typedef struct {
  poly_u *share[MASKING_N];
} masked_poly_ptr;


typedef struct {
  const poly_u *share[MASKING_N];
} masked_poly_ptr_const;



#define masked_poly_ptr_mask DILITHIUM_NAMESPACE(masked_poly_ptr_mask)
void masked_poly_ptr_mask(const masked_poly_ptr *r, const poly_u *a);

#define masked_poly_ptr_unmask DILITHIUM_NAMESPACE(masked_poly_ptr_unmask)
void masked_poly_ptr_unmask(poly *a, masked_poly_ptr_const *r);

#define masked_poly_ptr_unpack DILITHIUM_NAMESPACE(masked_poly_ptr_unpack)
void masked_poly_ptr_unpack(const masked_poly_ptr *r, const uint8_t *a, unsigned int len, const uint32_t d, const uint32_t c);

#define masked_poly_ntt DILITHIUM_NAMESPACE(masked_poly_ntt)
void masked_poly_ntt(masked_poly *r);

#define masked_poly_invntt DILITHIUM_NAMESPACE(masked_poly_invntt)
void masked_poly_invntt(masked_poly *r);

#define masked_poly_ptr_uniform_gamma1_fromhw DILITHIUM_NAMESPACE(masked_poly_ptr_uniform_gamma1_fromhw)
void masked_poly_ptr_uniform_gamma1_fromhw(const masked_poly_ptr *y, const masked_crh rhoprime, uint16_t nonce_next, int init_next);

#define masked_poly_ptr_uniform_gamma1_fromhw_inner DILITHIUM_NAMESPACE(masked_poly_ptr_uniform_gamma1_fromhw_inner)
void masked_poly_ptr_uniform_gamma1_fromhw_inner(const masked_poly_ptr *y, const masked_crh rhoprime, uint32_t buf[MASKING_N][POLYZ_PACKEDBYTES >> 2]);

#define masked_poly_pointwise DILITHIUM_NAMESPACE(masked_poly_pointwise)
void masked_poly_pointwise(masked_poly *c, const poly *a, const masked_poly *b);

#define masked_poly_pointwise_acc DILITHIUM_NAMESPACE(masked_poly_pointwise_acc)
void masked_poly_pointwise_acc(masked_poly *c, const poly *a, const masked_poly *b);

#define masked_poly_decompose DILITHIUM_NAMESPACE(masked_poly_decompose)
void masked_poly_decompose(poly *v1, masked_poly *v0, const masked_poly *v);

#define masked_poly_ptr_decompose DILITHIUM_NAMESPACE(masked_poly_ptr_decompose)
void masked_poly_ptr_decompose(poly *v1, const masked_poly_ptr *v0, const masked_poly_ptr_const *v);

#define masked_poly_ptr_pointwise_add_invntt_chknorm DILITHIUM_NAMESPACE(masked_poly_ptr_pointwise_add_invntt_chknorm)
int masked_poly_ptr_pointwise_add_invntt_chknorm(const masked_poly_ptr *r_ptr, const masked_poly_ptr_const *v_ptr, const poly *c, const masked_poly_ptr *u_ptr, uint32_t B);

#define masked_poly_ptr_pointwise_invntt_sub_chknorm DILITHIUM_NAMESPACE(masked_poly_ptr_pointwise_invntt_sub_chknorm)
int masked_poly_ptr_pointwise_invntt_sub_chknorm(const masked_poly_ptr *r, const masked_poly_ptr_const *v, const poly *c, const masked_poly_ptr_const *u, const masked_poly_ptr *temp, uint32_t B);

#define masked_poly_ptr_chknorm DILITHIUM_NAMESPACE(masked_poly_ptr_chknorm)
int masked_poly_ptr_chknorm(const masked_poly_ptr *r, const masked_poly_ptr *temp, uint32_t B);

#endif