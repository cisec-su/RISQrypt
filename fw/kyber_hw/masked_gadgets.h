#ifndef MASKED_GADGETS_H
#define MASKED_GADGETS_H

#include "poly.h"
#include "masked_poly.h"
#include "polyvec.h"
#include "masked_polyvec.h"


void masked_gadgets_A2B_2k(masked_poly *r, const masked_poly *a, uint32_t p);

void masked_gadgets_A2B_2k_u32(masked_poly_u32 *r, const masked_poly_u32 *a, uint32_t p);

void masked_gadgets_B2A_q(masked_poly *r, const masked_poly *a);

void masked_gadgets_mask_poly(masked_poly *r, const poly *a);

void masked_gadgets_mask_polyvec(masked_polyvec *r, const polyvec *a);


#endif