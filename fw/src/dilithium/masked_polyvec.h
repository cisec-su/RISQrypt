#ifndef MASKED_POLYVEC_H
#define MASKED_POLYVEC_H


#include "masked.h"
#include "params.h"
#include "masked_poly.h"
#include "polyvec.h"


typedef struct {
  poly_u vec[L];
} polyvecl_u;

typedef struct {
  polyvecl_u share[MASKING_N];
} masked_polyvecl;

typedef struct {
  poly_u vec[K];
} polyveck_u;

typedef struct {
  polyveck_u share[MASKING_N];
} masked_polyveck;


inline void masked_polyveck_to_poly_ptr_const(masked_poly_ptr_const *dst, const masked_polyveck *src, int i) {
    unsigned int j;
    for (j = 0; j < MASKING_N; j++) {
        dst->share[j] = &src->share[j].vec[i];
    }
}


inline void masked_polyveck_to_poly_ptr(masked_poly_ptr *dst, masked_polyveck *src, int i) {
    unsigned int j;
    for (j = 0; j < MASKING_N; j++) {
        dst->share[j] = &src->share[j].vec[i];
    }
}


inline void masked_polyvecl_to_poly_ptr_const(masked_poly_ptr_const *dst, const masked_polyvecl *src, int i) {
    unsigned int j;
    for (j = 0; j < MASKING_N; j++) {
        dst->share[j] = &src->share[j].vec[i];
    }
}


inline void masked_polyvecl_to_poly_ptr(masked_poly_ptr *dst, masked_polyvecl *src, int i) {
    unsigned int j;
    for (j = 0; j < MASKING_N; j++) {
        dst->share[j] = &src->share[j].vec[i];
    }
}


#define masked_polyvecl_eta_unpack DILITHIUM_NAMESPACE(masked_polyvecl_eta_unpack)
void masked_polyvecl_eta_unpack(masked_polyvecl *r, const uint8_t *a);

#define masked_polyveck_eta_unpack DILITHIUM_NAMESPACE(masked_polyveck_eta_unpack)
void masked_polyveck_eta_unpack(masked_polyveck *r, const uint8_t *a);

#define masked_polyvecl_ntt DILITHIUM_NAMESPACE(masked_polyvecl_t_ntt)
void masked_polyvecl_ntt(masked_polyvecl *r);

#define masked_polyveck_ntt DILITHIUM_NAMESPACE(masked_polyveck_t_ntt)
void masked_polyveck_ntt(masked_polyveck *r);

#define masked_polyveck_invntt DILITHIUM_NAMESPACE(masked_polyveck_invntt)
void masked_polyveck_invntt(masked_polyveck *r);

#define masked_polyvecl_uniform_gamma1 DILITHIUM_NAMESPACE(masked_polyvecl_uniform_gamma1)
void masked_polyvecl_uniform_gamma1(masked_polyvecl *y, const masked_crh rhoprime, uint16_t nonce);

#define masked_polyvec_matrix_pointwise DILITHIUM_NAMESPACE(masked_polyvec_matrix_pointwise)
void masked_polyvec_matrix_pointwise(masked_polyveck *t, const polyvecl mat[K], const masked_polyvecl *v);

#define masked_polyvec_matrix_pointwise_onthefly DILITHIUM_NAMESPACE(masked_polyvec_matrix_pointwise_onthefly)
void masked_polyvec_matrix_pointwise_onthefly(masked_polyveck *t, const uint8_t rho[SEEDBYTES], const masked_polyvecl *v);

#define masked_polyvecl_unmask DILITHIUM_NAMESPACE(masked_polyvecl_unmask)
void masked_polyvecl_unmask(polyvecl *a, const masked_polyvecl *r);

#define masked_polyveck_unmask DILITHIUM_NAMESPACE(masked_polyveck_unmask)
void masked_polyveck_unmask(polyveck *a, const masked_polyveck *r);

#define masked_polyveck_decompose DILITHIUM_NAMESPACE(masked_polyveck_decompose)
void masked_polyveck_decompose(polyveck *v1, masked_polyveck *v0, const masked_polyveck *v);

#define masked_polyvecl_pointwise_add_invntt_chknorm DILITHIUM_NAMESPACE(masked_polyvecl_pointwise_add_invntt_chknorm)
int masked_polyvecl_pointwise_add_invntt_chknorm(masked_polyvecl *r, const masked_polyvecl *v, const poly *c, masked_polyvecl *u, uint32_t B);

#define masked_polyveck_pointwise_invntt_sub_chknorm DILITHIUM_NAMESPACE(masked_polyveck_pointwise_invntt_sub_chknorm)
int masked_polyveck_pointwise_invntt_sub_chknorm(masked_polyveck *r, const masked_polyveck *v, const poly *c, const masked_polyveck *u, masked_poly_ptr *temp, uint32_t B);

#endif