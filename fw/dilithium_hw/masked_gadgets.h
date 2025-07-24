#ifndef MASKED_GADGETS_H
#define MASKED_GADGETS_H

#include <stdint.h>
#include "masked_poly.h"


void masked_gadgets_B2A_q(masked_poly *r, const masked_poly *a);

void masked_gadgets_A2B_q_ptr(masked_poly *r, const poly *a[MASKING_N]);

void masked_gadgets_A2B_q(masked_poly *r, const masked_poly *a);

void masked_gadgets_B2A_2k(masked_poly *r, const masked_poly *a, uint32_t p);

void masked_gadgets_A2B_2k(masked_poly *r, const masked_poly *a, uint32_t p);


#endif