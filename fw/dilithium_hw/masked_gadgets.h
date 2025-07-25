#ifndef MASKED_GADGETS_H
#define MASKED_GADGETS_H

#include <stdint.h>
#include "masked_poly.h"


#define masked_gadgets_B2A_q DILITHIUM_NAMESPACE(masked_gadgets_B2A_q)
void masked_gadgets_B2A_q(masked_poly *r, const masked_poly *a);

#define masked_gadgets_A2B_q_ptr DILITHIUM_NAMESPACE(masked_gadgets_A2B_q_ptr)
void masked_gadgets_A2B_q_ptr(masked_poly *r, const poly *a[MASKING_N]);

#define masked_gadgets_A2B_q DILITHIUM_NAMESPACE(masked_gadgets_A2B_q)
void masked_gadgets_A2B_q(masked_poly *r, const masked_poly *a);

#define masked_gadgets_B2A_2k DILITHIUM_NAMESPACE(masked_gadgets_B2A_2k)
void masked_gadgets_B2A_2k(masked_poly *r, const masked_poly *a, uint32_t p);

#define masked_gadgets_A2B_2k DILITHIUM_NAMESPACE(masked_gadgets_A2B_2k)
void masked_gadgets_A2B_2k(masked_poly *r, const masked_poly *a, uint32_t p);


#endif