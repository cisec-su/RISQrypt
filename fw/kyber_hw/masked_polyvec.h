#ifndef MASKED_POLYVEC_H
#define MASKED_POLYVEC_H

#include "masked.h"
#include "polyvec.h"


typedef struct{
    polyvec share[MASKING_N];
} masked_polyvec;


#define masked_polyvec_ntt KYBER_NAMESPACE(_masked_polyvec_ntt)
void masked_polyvec_ntt(masked_polyvec *r);

#define masked_polyvec_pointwise_acc_invntt_i KYBER_NAMESPACE(_masked_polyvec_pointwise_acc_invntt_i)
void masked_polyvec_pointwise_acc_invntt_i(masked_polyvec *r, const masked_polyvec *a, const polyvec *b, unsigned int i);

#define masked_polyvec_pointwise_acc_invntt KYBER_NAMESPACE(_masked_polyvec_pointwise_acc_invntt)
void masked_polyvec_pointwise_acc_invntt(masked_poly *r, const masked_polyvec *a, const polyvec *b);

void masked_polyvec_mask(masked_polyvec *r, polyvec *a);

void masked_polyvec_unmask(polyvec *r, masked_polyvec *a);

#define masked_polyvec_getnoise_eta1 KYBER_NAMESPACE(_masked_polyvec_getnoise_eta1)
void masked_polyvec_getnoise_eta1(masked_polyvec *r, const masked_sym seed, uint8_t *nonce);

#define masked_polyvec_getnoise_eta2 KYBER_NAMESPACE(_masked_polyvec_getnoise_eta2)
void masked_polyvec_getnoise_eta2(masked_polyvec *r, const masked_sym seed, uint8_t *nonce);

#define masked_polyvec_add KYBER_NAMESPACE(_masked_polyvec_add)
void masked_polyvec_add(masked_polyvec *r, const masked_polyvec *a, const masked_polyvec *b);

#define masked_polyvec_sub_exp KYBER_NAMESPACE(_masked_polyvec_sub_exp)
void masked_polyvec_sub_exp(masked_polyvec *r, const masked_polyvec *a, const polyvec *b);

#define masked_polyvec_sub_one KYBER_NAMESPACE(_masked_polyvec_sub_one)
void masked_polyvec_sub_one(masked_polyvec *r, const masked_polyvec *a);

#define masked_polyvec_acc KYBER_NAMESPACE(_masked_polyvec_acc)
void masked_polyvec_acc(masked_poly *r, const masked_polyvec *a, const masked_poly *b);


#endif