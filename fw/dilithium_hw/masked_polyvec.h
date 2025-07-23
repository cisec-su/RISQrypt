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



void masked_polyvecl_eta_unpack(masked_polyvecl *r, const uint8_t *a);

void masked_polyveck_eta_unpack(masked_polyveck *r, const uint8_t *a);

void masked_polyveck_t0_unpack(masked_polyveck *r, const uint8_t *a);

void masked_polyvecl_ntt(masked_polyvecl *r);

void masked_polyveck_ntt(masked_polyveck *r);

void masked_polyvecl_uniform_gamma1(masked_polyvecl *y, const masked_crh rhoprime, uint16_t nonce);

void masked_polyvec_matrix_pointwise(masked_polyveck *t, const polyvecl mat[K], const masked_polyvecl *v);




#endif