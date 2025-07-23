#ifndef MASKED_PACKING_H
#define MASKED_PACKING_H

#include "packing.h"
#include "masked.h"
#include "masked_polyvec.h"


void masked_unpack_sk(uint8_t *rho, uint8_t *tr, masked_seed key, masked_polyveck *t0, masked_polyvecl *s1,  masked_polyveck *s2, const uint8_t sk[CRYPTO_SECRETKEYBYTES]);


#endif