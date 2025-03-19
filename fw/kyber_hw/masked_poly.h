#ifndef MASKED_POLY_H
#define MASKED_POLY_H

#include "masked.h"
#include "poly.h"


typedef struct{
    poly share[MASKING_N];
} masked_poly;


#define masked_poly_tomsg KYBER_NAMESPACE(_masked_poly_tomsg)
void masked_poly_tomsg(masked_msg msg, masked_poly *a);

#define masked_poly_frommsg KYBER_NAMESPACE(_masked_poly_frommsg)
void masked_poly_frommsg(masked_poly *a, masked_msg msg);


#endif