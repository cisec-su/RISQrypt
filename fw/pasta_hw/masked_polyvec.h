#ifndef MASKED_POLYVEC_H
#define MASKED_POLYVEC_H


#include "masked.h"
#include "params.h"
#include "masked_poly.h"
#include "polyvec.h"


typedef struct {
  masked_poly vec[L];
} masked_polyvecl;


typedef struct {
  masked_poly vec[K];
} masked_polyveck;


#define masked_polyvecl_eta_unpack DILITHIUM_NAMESPACE(masked_polyvecl_eta_unpack)
void masked_polyvecl_eta_unpack(masked_polyvecl *r, const uint8_t *a);

#define masked_polyveck_eta_unpack DILITHIUM_NAMESPACE(masked_polyveck_eta_unpack)
void masked_polyveck_eta_unpack(masked_polyveck *r, const uint8_t *a);

#define masked_polyveck_t0_unpack DILITHIUM_NAMESPACE(masked_polyveck_t0_unpack)
void masked_polyveck_t0_unpack(masked_polyveck *r, const uint8_t *a);

#define masked_polyvecl_ntt DILITHIUM_NAMESPACE(masked_polyvecl_ntt)
void masked_polyvecl_ntt(masked_polyvecl *r);

#define masked_polyveck_ntt DILITHIUM_NAMESPACE(masked_polyveck_ntt)
void masked_polyveck_ntt(masked_polyveck *r);

#define masked_polyveck_invntt DILITHIUM_NAMESPACE(masked_polyveck_invntt)
void masked_polyveck_invntt(masked_polyveck *r);

#define masked_polyvecl_uniform_gamma1 DILITHIUM_NAMESPACE(masked_polyvecl_uniform_gamma1)
void masked_polyvecl_uniform_gamma1(masked_polyvecl *y, const masked_crh rhoprime, uint16_t nonce);

#define masked_polyvec_matrix_pointwise DILITHIUM_NAMESPACE(masked_polyvec_matrix_pointwise)
void masked_polyvec_matrix_pointwise(masked_polyveck *t, const polyvecl mat[K], const masked_polyvecl *v);

#define masked_polyvecl_unmask DILITHIUM_NAMESPACE(masked_polyvecl_unmask)
void masked_polyvecl_unmask(polyvecl *a, const masked_polyvecl *r);

#define masked_polyveck_unmask DILITHIUM_NAMESPACE(masked_polyveck_unmask)
void masked_polyveck_unmask(polyveck *a, const masked_polyveck *r);

#define masked_polyveck_decompose DILITHIUM_NAMESPACE(masked_polyveck_decompose)
void masked_polyveck_decompose(polyveck *v1, masked_polyveck *v0, const masked_polyveck *v);

#define masked_polyvecl_pointwise_add_invntt_chknorm DILITHIUM_NAMESPACE(masked_polyvecl_pointwise_add_invntt_chknorm)
int masked_polyvecl_pointwise_add_invntt_chknorm(masked_polyvecl *r, const masked_polyvecl *v, const poly *c, const masked_polyvecl *u, uint32_t B);

#define masked_polyveck_pointwise_invntt_sub_chknorm DILITHIUM_NAMESPACE(masked_polyveck_pointwise_invntt_sub_chknorm)
int masked_polyveck_pointwise_invntt_sub_chknorm(masked_polyveck *r, const masked_polyveck *v, const poly *c, const masked_polyveck *u, uint32_t B);

#endif