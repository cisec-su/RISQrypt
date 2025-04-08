#ifndef MASKED_GADGETS_H
#define MASKED_GADGETS_H

#include <stdint.h>
#include "poly.h"
#include "masked_poly.h"
#include "polyvec.h"
#include "masked_polyvec.h"


#define Q_EXP    ((uint32_t) 0x80000081)  // 2**31 + 2**7 + 1
#define MU_EXP_L ((uint32_t) 0xfffffdfc)
#define MU_EXP_H ((uint32_t) 0x1       )

void masked_gadgets_A2B_2k(masked_poly *r, const masked_poly *a, uint32_t p);

void masked_gadgets_A2B_2k_u32(masked_poly_u32 *r, const masked_poly_u32 *a, uint32_t p);

void masked_gadgets_B2A_2k_u32(masked_poly_u32 *r, const masked_poly_u32 *a);

void masked_gadgets_B2A_2k_u32_vec(masked_polyvec_u32 *r, const masked_polyvec_u32 *a);

void masked_gadgets_B2A_qm_u32(masked_poly_u32 *r, const masked_poly_u32 *a);

void masked_gadgets_B2A_qm_u32_vec(masked_polyvec_u32 *r, const masked_polyvec_u32 *a);

void masked_gadgets_B2A_q(masked_poly *r, const masked_poly *a);

void masked_gadgets_mask_poly(masked_poly *r, const poly *a);

void masked_gadgets_mask_polyvec(masked_polyvec *r, const polyvec *a);

void masked_gadgets_exp_u32(masked_u32 r, masked_u32 a);

void masked_gadgets_unmask_u32(uint32_t *r, const masked_u32 a);

#endif