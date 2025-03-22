#ifndef MASKED_POLY_H
#define MASKED_POLY_H

#include <stdint.h>
#include "masked.h"
#include "poly.h"


typedef struct{
    poly share[MASKING_N];
} masked_poly;


typedef struct{
    int32_t coeffs[KYBER_N];
} poly_u32;


typedef struct{
    poly_u32 share[MASKING_N];
} masked_poly_u32;


typedef coeff masked_coeff[MASKING_N];


#define masked_poly_sub_tomsg KYBER_NAMESPACE(_masked_poly_tomsg)
void masked_poly_sub_tomsg(masked_msg msg, const poly *b, masked_poly *a);

#define masked_poly_frommsg KYBER_NAMESPACE(_masked_poly_frommsg)
void masked_poly_frommsg(masked_poly *a, const masked_msg msg);

#define masked_poly_compress KYBER_NAMESPACE(_masked_poly_compress)
void masked_poly_compress(masked_poly *r, const masked_poly *a);

#define masked_poly_compress_du KYBER_NAMESPACE(_masked_poly_compress_du)
void masked_poly_compress_du(poly *r[MASKING_N], const poly *a[MASKING_N]);

#define masked_poly_getnoise_eta2 KYBER_NAMESPACE(_masked_poly_getnoise_eta2)
void masked_poly_getnoise_eta2(masked_poly *r, const masked_sym seed, uint8_t *nonce);

#define masked_poly_add KYBER_NAMESPACE(_masked_poly_add)
void masked_poly_add(masked_poly *r, const masked_poly *a, const masked_poly *b);

#define masked_poly_add_chain KYBER_NAMESPACE(_masked_poly_add_chain)
void masked_poly_add_chain(masked_poly *r, const masked_poly *a, const masked_poly *b, const masked_poly *c);

#define masked_poly_sub KYBER_NAMESPACE(_masked_poly_sub)
void masked_poly_sub(masked_poly *r, const masked_poly *a, const poly *b);

#define masked_poly_sub_exp KYBER_NAMESPACE(_masked_poly_sub_exp)
void masked_poly_sub_exp(masked_poly *r, const masked_poly *a, const poly *b);

#define masked_poly_sub_x KYBER_NAMESPACE(masked_poly_sub_x)
void masked_poly_sub_x(masked_poly *r, const masked_poly *a);

#define masked_poly_sum KYBER_NAMESPACE(_masked_poly_sum)
void masked_poly_sum(masked_coeff *r, const masked_poly *a);

#define masked_poly_coeff_exp KYBER_NAMESPACE(_masked_poly_coeff_exp)
void masked_poly_coeff_exp(masked_coeff *r, const masked_coeff *a);

#define masked_poly_unmask_coeff_inp KYBER_NAMESPACE(_masked_poly_unmask_coeff_inp)
void masked_poly_unmask_coeff_inp(masked_coeff *a);


#endif