#ifndef MASKED_GADGETS_H
#define MASKED_GADGETS_H

#include <stdint.h>
#include "masked_poly.h"

#define masked_gadgets_init_q DILITHIUM_NAMESPACE(masked_gadgets_init_q)
void masked_gadgets_init_q();

#define masked_gadgets_init_2k DILITHIUM_NAMESPACE(masked_gadgets_init_2k)
void masked_gadgets_init_2k(uint32_t p);

#define masked_gadgets_mask_poly_ptr DILITHIUM_NAMESPACE(masked_gadgets_mask_poly_ptr)
void masked_gadgets_mask_poly_ptr(const masked_poly_ptr *r, const poly_u *a);

#define masked_gadgets_A2B_q_ptr DILITHIUM_NAMESPACE(masked_gadgets_A2B_q_ptr)
void masked_gadgets_A2B_q_ptr(const masked_poly_ptr *r, const masked_poly_ptr_const *a);

#define masked_gadgets_A2B_q DILITHIUM_NAMESPACE(masked_gadgets_A2B_q)
void masked_gadgets_A2B_q(masked_poly *r, const masked_poly *a);

#define masked_gadgets_B2A_q_ptr DILITHIUM_NAMESPACE(masked_gadgets_B2A_q_ptr)
void masked_gadgets_B2A_q_ptr(const masked_poly_ptr *r, const masked_poly_ptr_const *a);

#define masked_gadgets_A2B_2k_ptr DILITHIUM_NAMESPACE(masked_gadgets_A2B_2k_ptr)
void masked_gadgets_A2B_2k_ptr(const masked_poly_ptr *r, const masked_poly_ptr_const *a);

#define masked_gadgets_B2A_2k_ptr DILITHIUM_NAMESPACE(masked_gadgets_B2A_2k_ptr)
void masked_gadgets_B2A_2k_ptr(const masked_poly_ptr *r, const masked_poly_ptr_const *a);

#endif