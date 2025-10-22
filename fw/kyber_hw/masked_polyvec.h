#ifndef MASKED_POLYVEC_H
#define MASKED_POLYVEC_H

#include "masked.h"
#include "masked_poly.h"
#include "polyvec.h"


typedef struct{
    polyvec share[MASKING_N];
} masked_polyvec;

typedef struct{
    polyvec_u32 share[MASKING_N];
} masked_polyvec_u32;


#define masked_polyvec_ntt KYBER_NAMESPACE(_masked_polyvec_ntt)
void masked_polyvec_ntt(masked_polyvec *r);

#define masked_polyvec_pointwise_acc_invntt_i KYBER_NAMESPACE(_masked_polyvec_pointwise_acc_invntt_i)
void masked_polyvec_pointwise_acc_invntt_i(masked_polyvec *r, const masked_polyvec *a, const polyvec *b, unsigned int i);

#define masked_polyvec_pointwise_acc_invntt KYBER_NAMESPACE(_masked_polyvec_pointwise_acc_invntt)
void masked_polyvec_pointwise_acc_invntt(masked_poly *r, const masked_polyvec *a, const polyvec *b);

#define masked_polyvec_pointwise_acc_invntt_tohw KYBER_NAMESPACE(_masked_polyvec_pointwise_acc_invntt_tohw)
void masked_polyvec_pointwise_acc_invntt_tohw(masked_poly *r, const masked_polyvec *a, const polyvec *b);

#define masked_polyvec_sub_compress KYBER_NAMESPACE(_masked_polyvec_sub_compress)
void masked_polyvec_sub_compress(masked_polyvec_u32 *r, const masked_polyvec *a, const uint8_t *b);

void masked_polyvec_mask(masked_polyvec *r, const polyvec *a);

#define masked_polyvec_getnoise_eta1 KYBER_NAMESPACE(_masked_polyvec_getnoise_eta1)
void masked_polyvec_getnoise_eta1(masked_polyvec *r, const masked_sym seed, uint8_t *nonce);

#define masked_polyvec_getnoise_eta2 KYBER_NAMESPACE(_masked_polyvec_getnoise_eta2)
void masked_polyvec_getnoise_eta2(masked_polyvec *r, const masked_sym seed, uint8_t *nonce);

#define masked_polyvec_add KYBER_NAMESPACE(_masked_polyvec_add)
void masked_polyvec_add(masked_polyvec *r, const masked_polyvec *a, const masked_polyvec *b);

#define masked_polyvec_u32_acc KYBER_NAMESPACE(_masked_polyvec_u32_acc)
void masked_polyvec_u32_acc(masked_u32 r, const masked_polyvec_u32 *a, const masked_poly_u32 *b);


#endif