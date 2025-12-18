#ifndef POLYVEC_H
#define POLYVEC_H

#include <stdint.h>
#include "params.h"
#include "poly.h"

/* Vectors of polynomials of length L */
typedef struct {
  poly vec[L];
} polyvecl;

#define polyvecl_uniform_eta DILITHIUM_NAMESPACE(polyvecl_uniform_eta)
void polyvecl_uniform_eta(polyvecl *v, const uint8_t seed[CRHBYTES], uint16_t nonce);

#define polyvecl_uniform_gamma1 DILITHIUM_NAMESPACE(polyvecl_uniform_gamma1)
void polyvecl_uniform_gamma1(polyvecl *v, const uint8_t seed[CRHBYTES], uint16_t nonce);

#define polyvecl_add DILITHIUM_NAMESPACE(polyvecl_add)
void polyvecl_add(polyvecl *w, const polyvecl *u, const polyvecl *v);

#define polyvecl_ntt DILITHIUM_NAMESPACE(polyvecl_ntt)
void polyvecl_ntt(polyvecl *v);
#define polyvecl_invntt DILITHIUM_NAMESPACE(polyvecl_invntt)
void polyvecl_invntt(polyvecl *v);
#define polyvecl_pointwise_poly DILITHIUM_NAMESPACE(polyvecl_pointwise_poly)
void polyvecl_pointwise_poly(polyvecl *r, const poly *a, const polyvecl *v);
#define polyvecl_pointwise_acc \
        DILITHIUM_NAMESPACE(polyvecl_pointwise_acc)
void polyvecl_pointwise_acc(poly *w,
                                       const polyvecl *u,
                                       const polyvecl *v);


#define polyvecl_chknorm DILITHIUM_NAMESPACE(polyvecl_chknorm)
int polyvecl_chknorm(const polyvecl *v, int32_t B);



/* Vectors of polynomials of length K */
typedef struct {
  poly vec[K];
} polyveck;

#define polyveck_uniform_eta DILITHIUM_NAMESPACE(polyveck_uniform_eta)
void polyveck_uniform_eta(polyveck *v, const uint8_t seed[CRHBYTES], uint16_t nonce);

#define polyveck_reduce DILITHIUM_NAMESPACE(polyveck_reduce)
void polyveck_reduce(polyveck *v);
#define polyveck_freeze DILITHIUM_NAMESPACE(polyveck_freeze)
void polyveck_freeze(polyveck *v);

#define polyveck_add DILITHIUM_NAMESPACE(polyveck_add)
void polyveck_add(polyveck *w, const polyveck *u, const polyveck *v);
#define polyveck_sub DILITHIUM_NAMESPACE(polyveck_sub)
void polyveck_sub(polyveck *w, const polyveck *u, const polyveck *v);

#define polyveck_ntt DILITHIUM_NAMESPACE(polyveck_ntt)
void polyveck_ntt(polyveck *v);

#define polyveck_shiftl_ntt DILITHIUM_NAMESPACE(polyveck_shiftl_ntt)
void polyveck_shiftl_ntt(polyveck *v);

#define polyveck_invntt DILITHIUM_NAMESPACE(polyveck_invntt)
void polyveck_invntt(polyveck *v);

#define polyveck_pointwise_poly DILITHIUM_NAMESPACE(polyveck_pointwise_poly)
void polyveck_pointwise_poly(polyveck *r, const poly *a, const polyveck *v);

#define polyveck_pointwise_poly_sub DILITHIUM_NAMESPACE(polyveck_pointwise_poly_sub)
void polyveck_pointwise_poly_sub(polyveck *r, const poly *a, const polyveck *v, const polyveck *u);

#define polyveck_chknorm DILITHIUM_NAMESPACE(polyveck_chknorm)
int polyveck_chknorm(const polyveck *v, uint32_t B);

#define polyveck_power2round DILITHIUM_NAMESPACE(polyveck_power2round)
void polyveck_power2round(polyveck *v1, polyveck *v0, const polyveck *v);

#define polyveck_decompose DILITHIUM_NAMESPACE(polyveck_decompose)
void polyveck_decompose(polyveck *v1, polyveck *v0, const polyveck *v);

#define polyveck_use_hint_pack DILITHIUM_NAMESPACE(polyveck_use_hint_pack)
void polyveck_use_hint_pack(uint8_t *r, const polyveck *u, const polyveck *h);

#define polyveck_pack_w1 DILITHIUM_NAMESPACE(polyveck_pack_w1)
void polyveck_pack_w1(uint8_t r[K*POLYW1_PACKEDBYTES], const polyveck *w1);

#define polyvec_matrix_expand DILITHIUM_NAMESPACE(polyvec_matrix_expand)
void polyvec_matrix_expand(polyvecl mat[K], const uint8_t rho[SEEDBYTES]);

#define polyvec_matrix_pointwise DILITHIUM_NAMESPACE(polyvec_matrix_pointwise)
void polyvec_matrix_pointwise(polyveck *t, const polyvecl mat[K], const polyvecl *v);

#define polyveck_invntt_sub DILITHIUM_NAMESPACE(polyveck_invntt_sub)
void polyveck_invntt_sub(polyveck *r, polyveck *v, polyveck *u);

#define polyvecl_invntt_chknorm DILITHIUM_NAMESPACE(polyvecl_invntt_chknorm)
int polyvecl_invntt_chknorm(polyvecl *v, uint32_t B);

#define polyveck_invntt_chknorm DILITHIUM_NAMESPACE(polyveck_invntt_chknorm)
int polyveck_invntt_chknorm(polyveck *v, uint32_t B);

#define polyveck_pointwise_invntt_sub_chknorm DILITHIUM_NAMESPACE(polyveck_pointwise_invntt_sub_chknorm)
int polyveck_pointwise_invntt_sub_chknorm(polyveck *r, const polyveck *v, const poly *c, const polyveck *u, uint32_t B);

#define polyvecl_pointwise_add_invntt_chknorm DILITHIUM_NAMESPACE(polyvecl_pointwise_add_invntt_chknorm)
int polyvecl_pointwise_add_invntt_chknorm(polyvecl *r, const polyvecl *v, const poly *c, const polyvecl *u, uint32_t B);

#define polyveck_add_make_hint DILITHIUM_NAMESPACE(polyveck_add_make_hint)
unsigned int polyveck_add_make_hint(polyveck *h, const polyveck *v0, const polyveck *v1, const polyveck *u);

#endif
