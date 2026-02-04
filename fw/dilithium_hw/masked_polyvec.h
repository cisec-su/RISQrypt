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
  poly_u vec[L];
} polyvecl_u;

typedef struct {
  polyvecl_u share[MASKING_N];
} masked_polyvecl_t;


typedef struct {
  masked_poly vec[K];
} masked_polyveck;

typedef struct {
  poly_u vec[K];
} polyveck_u;


typedef struct {
  polyveck_u share[MASKING_N];
} masked_polyveck_t;


inline void masked_polyveck_t_to_poly_ptr_const(masked_poly_ptr_const *dst, const masked_polyveck_t *src, int i) {
    unsigned int j;
    for (j = 0; j < MASKING_N; j++) {
        dst->share[j] = &src->share[j].vec[i];
    }
}


inline void masked_polyveck_t_to_poly_ptr(masked_poly_ptr *dst, masked_polyveck_t *src, int i) {
    unsigned int j;
    for (j = 0; j < MASKING_N; j++) {
        dst->share[j] = &src->share[j].vec[i];
    }
}


inline void masked_polyvecl_t_to_poly_ptr_const(masked_poly_ptr_const *dst, const masked_polyvecl_t *src, int i) {
    unsigned int j;
    for (j = 0; j < MASKING_N; j++) {
        dst->share[j] = &src->share[j].vec[i];
    }
}


inline void masked_polyvecl_t_to_poly_ptr(masked_poly_ptr *dst, masked_polyvecl_t *src, int i) {
    unsigned int j;
    for (j = 0; j < MASKING_N; j++) {
        dst->share[j] = &src->share[j].vec[i];
    }
}


#define masked_polyvecl_eta_unpack DILITHIUM_NAMESPACE(masked_polyvecl_eta_unpack)
void masked_polyvecl_eta_unpack(masked_polyvecl_t *r, const uint8_t *a);

#define masked_polyveck_eta_unpack DILITHIUM_NAMESPACE(masked_polyveck_eta_unpack)
void masked_polyveck_eta_unpack(masked_polyveck_t *r, const uint8_t *a);

// #define masked_polyvecl_ntt DILITHIUM_NAMESPACE(masked_polyvecl_t_ntt)
// void masked_polyvecl_ntt(masked_polyvecl_t *r);

// #define masked_polyveck_ntt DILITHIUM_NAMESPACE(masked_polyveck_t_ntt)
// void masked_polyveck_ntt(masked_polyveck_t *r);

#define masked_polyveck_invntt DILITHIUM_NAMESPACE(masked_polyveck_invntt)
void masked_polyveck_invntt(masked_polyveck_t *r);

// #define masked_polyvecl_uniform_gamma1 DILITHIUM_NAMESPACE(masked_polyvecl_uniform_gamma1)
// void masked_polyvecl_uniform_gamma1(masked_polyvecl_t *y, const masked_crh rhoprime, uint16_t nonce);

#define masked_polyvec_matrix_pointwise DILITHIUM_NAMESPACE(masked_polyvec_matrix_pointwise)
void masked_polyvec_matrix_pointwise(masked_polyveck *t, const polyvecl mat[K], const masked_polyvecl *v);

#define masked_polyvecl_unmask DILITHIUM_NAMESPACE(masked_polyvecl_unmask)
void masked_polyvecl_unmask(polyvecl *a, const masked_polyvecl_t *r);

#define masked_polyveck_unmask DILITHIUM_NAMESPACE(masked_polyveck_unmask)
void masked_polyveck_unmask(polyveck *a, const masked_polyveck_t *r);

// #define masked_polyveck_t_decompose DILITHIUM_NAMESPACE(masked_polyveck_t_decompose)
// void masked_polyveck_t_decompose(polyveck *v1, masked_polyveck_t *v0, const masked_polyveck_t *v);

#define masked_polyvecl_pointwise_add_invntt_chknorm DILITHIUM_NAMESPACE(masked_polyvecl_pointwise_add_invntt_chknorm)
int masked_polyvecl_pointwise_add_invntt_chknorm(masked_polyvecl_t *r, const masked_polyvecl_t *v, const poly *c, masked_polyvecl_t *u, uint32_t B);

#define masked_polyveck_pointwise_invntt_sub_chknorm DILITHIUM_NAMESPACE(masked_polyveck_pointwise_invntt_sub_chknorm)
int masked_polyveck_pointwise_invntt_sub_chknorm(masked_polyveck_t *r, const masked_polyveck_t *v, const poly *c, const masked_polyveck_t *u, masked_poly_ptr *temp, uint32_t B);

#endif