#ifndef MASKED_POLY_H
#define MASKED_POLY_H

#include <stdint.h>
#include "masked.h"
#include "poly.h"


typedef struct{
    poly share[MASKING_N];
} masked_poly;

typedef struct{
    poly_u32 share[MASKING_N];
} masked_poly_u32;



#define masked_poly_sub_tomsg KYBER_NAMESPACE(_masked_poly_tomsg)
void masked_poly_sub_tomsg(masked_msg msg, const poly *b, masked_poly *a);

#define masked_poly_frommsg KYBER_NAMESPACE(_masked_poly_frommsg)
void masked_poly_frommsg(masked_poly *a, const masked_msg msg);

#define masked_poly_sub_compress KYBER_NAMESPACE(_masked_poly_sub_compress)
void masked_poly_sub_compress(masked_poly_u32 *r, const masked_poly *a, const uint8_t *b);

#define masked_poly_sub_compress_du KYBER_NAMESPACE(_masked_poly_sub_compress_du)
void masked_poly_sub_compress_du(poly_u32 *r[MASKING_N], const poly *a[MASKING_N], const uint8_t *b);

#define masked_poly_getnoise_eta2 KYBER_NAMESPACE(_masked_poly_getnoise_eta2)
void masked_poly_getnoise_eta2(masked_poly *r, const masked_sym seed, uint8_t *nonce);

#define masked_poly_add KYBER_NAMESPACE(_masked_poly_add)
void masked_poly_add(masked_poly *r, const masked_poly *a, const masked_poly *b);

#define masked_poly_add_chain KYBER_NAMESPACE(_masked_poly_add_chain)
void masked_poly_add_chain(masked_poly *r, const masked_poly *a, const masked_poly *b, const masked_poly *c);


#endif