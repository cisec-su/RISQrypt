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

void masked_gadgets_init_q();

void masked_gadgets_mask_polyvec(masked_polyvec *r, const polyvec *a);

void masked_gadgets_mask_doublesym(uint8_t r[MASKING_N][KYBER_SYMBYTES * 2], const uint8_t a[KYBER_SYMBYTES * 2]);

void masked_gadgets_init_2k(uint32_t p);

void masked_gadgets_A2B_2k(masked_poly *r, const masked_poly *a);

void masked_gadgets_init_2k_u32(uint32_t p);

void masked_gadgets_A2B_2k_u32(masked_poly_u32 *r, const masked_poly_u32 *a);

void masked_gadgets_init_q_carrier();

void masked_gadgets_B2A_qm_u32(masked_poly_u32 *r, const masked_poly_u32 *a);

void masked_gadgets_B2A_qm_u32_vec(masked_polyvec_u32 *r, const masked_polyvec_u32 *a);

void masked_gadgets_B2A_q(masked_poly *r, const masked_poly *a);

void masked_gadgets_B2A_onebit_frommsg(masked_poly *r, const masked_msg msg);

void masked_gadgets_exp_u32(masked_u32 r, masked_u32 a);

int masked_gadgets_zero_test_mul(masked_u32 a);

int masked_gadgets_zero_test_vec(masked_u32 a, masked_polyvec_u32 *mpvu32, masked_poly_u32 *mpu32);

void masked_gadgets_unmask_u32(uint32_t *r, const masked_u32 a);

#endif