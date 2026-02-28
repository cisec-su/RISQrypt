#include "masked_poly.h"
#include "masked_gadgets.h"
#include "ntt_lite.h"
#include <stdio.h>
#include <stdint.h>
#include <util.h>

#include <stddef.h>
#include "ntt_lite.h"
#include "x2x.h"
#include "masked_gadgets.h"
#include "util.h"

/**
 * @brief Convert unmasked polynomial to masked form with two shares
 * @description Creates a two-share masking of polynomial a by calling the gadget function masked_gadgets_mask_poly. The result r = r0 + r1 where r0 + r1 ≡ a (mod Q) and r0, r1 are statistically independent
 * @param r pointer to output masked polynomial (two shares)
 * @param a pointer to input unmasked polynomial
 * @return void
 */
void masked_poly_mask(masked_poly *r, const poly *a) {
    masked_gadgets_mask_poly(r, a);
}

/**
 * @brief Add unmasked polynomial to masked polynomial
 * @description Computes r = a + b where a is unmasked and b is masked. The unmasked value is added only to the first share: r.share[0] = a + b.share[0]; r.share[i] = b.share[i] for i > 0
 * @param r pointer to output masked polynomial
 * @param a pointer to input unmasked polynomial
 * @param b pointer to input masked polynomial
 * @return void
 */
void masked_poly_add_unmasked(masked_poly *r, const poly *a, const masked_poly *b){
    ntt_lite_add(r->share[0].coeffs,a->coeffs,b->share[0].coeffs);
}

/**
 * @brief Add two masked polynomials
 * @description Performs masked addition: r = a + b. Works on masked shares independently: r.share[i] = a.share[i] + b.share[i] for all shares i. This preserves the masking structure since addition is linear
 * @param r pointer to output masked polynomial
 * @param a pointer to first input masked polynomial
 * @param b pointer to second input masked polynomial
 * @return void
 */
void masked_poly_add(masked_poly *r, const masked_poly *a, const masked_poly *b){
    /*
     a = a0 + a1
     b = b0 + b1
     r = r0 + r1 := (a0+b0) + (a1+b1)
    */
    size_t i;
    for(i = 0;i<MASKING_N; i++){
        ntt_lite_set_clr_with_twiddle();
        ntt_lite_add(r->share[i].coeffs,a->share[i].coeffs,b->share[i].coeffs);
    }
}

/**
 * @brief Masked multiplication by constant (2) with addition
 * @description Computes r = CONST * a + b where CONST is currently bound to 2. Applies the constant multiplication to each share independently, then adds b: r.share[i] = CONST * a.share[i] + b.share[i] for all shares
 * @param r pointer to output masked polynomial
 * @param a pointer to first input masked polynomial (to be multiplied by CONST)
 * @param b pointer to second input masked polynomial (addend)
 * @return void
 */
void masked_poly_mult_add_const(masked_poly *r, const masked_poly *a, const masked_poly *b){

    /*
        CONST
        a = a0 a1
        b = b0 b1
        r = r0 r1
        r0 = CONST*a0+b0
        r1 = CONST*a1+b1
        r = CONST*a+b
    */

    size_t i;
    for(i = 0;i<MASKING_N; i++){
        ntt_lite_set_clr_with_twiddle();
        ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, a->share[i].coeffs);
        ntt_lite_add(r->share[i].coeffs,NTT_LITE_INPUT_DIS,b->share[i].coeffs);
    }

}

/**
 * @brief Recover unmasked polynomial from masked form
 * @description Reconstructs the unmasked polynomial by summing all shares: a = sum(r.share[i]) for i in 0..MASKING_N-1. Uses the hardware accelerator for efficient addition
 * @param a pointer to output unmasked polynomial
 * @param r pointer to input masked polynomial
 * @return void
 */
void masked_poly_unmask(poly *a, const masked_poly *r) {

    size_t i;
    uint32_t *lhs;
    uint32_t *dst;


    for (i = 1; i < MASKING_N; i++) {
        if (i == 1) {
            lhs = (uint32_t*) &r->share[0].coeffs;
        }
        else {
            lhs = NTT_LITE_INPUT_DIS;
        }
        if (i != (MASKING_N - 1)) {
            dst = NTT_LITE_OUTPUT_DIS;
        }
        else {
            dst = (uint32_t*) &a->coeffs;
        }
        ntt_lite_add(dst, lhs, (uint32_t*) &r->share[i].coeffs);
    }
}

/**
 * @brief Masked two-share multiplication with refreshing
 * @description Computes C = A * B with two-share masking using random refreshing: c0 = a0*b0 - r; c1 = (a0*b1) + (a1*b0) + (a1*b1) + r. Where random r is generated from a PRNG for security refreshing. Uses hardware accelerator for pointwise multiplication (ntt_lite_pwm)
 * @param C pointer to output masked polynomial
 * @param A pointer to first input masked polynomial
 * @param B pointer to second input masked polynomial
 * @return void
 */
void masked_poly_mult_mm(masked_poly *C, const masked_poly *A, const masked_poly *B) {
    poly r;

    size_t buflen = 4*STREAM128_BLOCKBYTES;
    uint32_t rng_buffer[buflen>>2];
    masked_gadgets_init_q();
    masked_gadgets_x2x_prng_read(&r, N); 
    
    ntt_lite_set_clr_with_twiddle();

    // c0 = a0*b0 - r
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, A->share[0].coeffs, B->share[0].coeffs);
    ntt_lite_sub(C->share[0].coeffs, NTT_LITE_INPUT_DIS, r.coeffs);
    
    // r' = a0*b1 + r
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, A->share[0].coeffs, B->share[1].coeffs);
    ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*)r.coeffs);
    
    // r' = (a0*b1 + r) + (b0*a1)
    ntt_lite_mac(NTT_LITE_OUTPUT_DIS,B->share[0].coeffs, A->share[1].coeffs); 
    ntt_lite_set_clr_with_twiddle();
    // c1 = a1*b1 + r'
    ntt_lite_mac(C->share[1].coeffs,A->share[1].coeffs, B->share[1].coeffs); 

}

/**
 * @brief Masked polynomial squaring
 * @description Computes B = A^2 using masked multiplication with affine transformation from gadgets. Internally computes CC = x2x_a_ref(A) and then calls masked_poly_mult_mm(B, A, CC)
 * @param B pointer to output masked polynomial
 * @param A pointer to input masked polynomial
 * @return void
 */
void masked_poly_square(masked_poly *B, const masked_poly *A) {
    masked_poly CC;
    masked_gadgets_x2x_a_ref(&CC, A); //masked_gad'e al
    masked_poly_mult_mm(B,A,&CC);

}

/**
 * @brief Masked polynomial cubing
 * @description Computes B = A^3 by first computing BB = A^2 (via masked_poly_square), then computing B = A * BB (via masked_poly_mult_mm)
 * @param B pointer to output masked polynomial
 * @param A pointer to input masked polynomial
 * @return void
 */
void masked_poly_cube(masked_poly *B, const masked_poly *A) {
    masked_poly CC;
    masked_poly BB;
    masked_gadgets_x2x_a_ref(&CC, A); //masked_gad'e al
    masked_poly_mult_mm(&BB,A,&CC);
    masked_poly_mult_mm(B,A,&BB);

}

/**
 * @brief Masked multiply-accumulate: D = A*B + C
 * @description Performs masked multiply-accumulate operation. First applies affine transformation to C, then computes D = A*B + C using masked multiplication and addition operations. Used for efficient Feistel computation
 * @param D pointer to output masked polynomial
 * @param A pointer to first input masked polynomial
 * @param B pointer to second input masked polynomial
 * @param C pointer to third input masked polynomial (addend)
 * @return void
 */
void masked_poly_mac( masked_poly *D, const masked_poly *A, const masked_poly *B, const masked_poly *C){ //affine transform
    //d = a*b + c
    masked_poly CC;
    poly r;

    masked_gadgets_x2x_a_ref(&CC, C); //masked_gad'e al

    // c0 = a0*b0 + c
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, A->share[0].coeffs, B->share[0].coeffs);
    ntt_lite_add(D->share[0].coeffs, NTT_LITE_INPUT_DIS, CC.share[0].coeffs);

    // r' = a0*b1 + c
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, A->share[0].coeffs, B->share[1].coeffs);
    ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*)CC.share[1].coeffs);

    // r' = (a0*b1 + c) + (b0*a1)
    ntt_lite_mac(NTT_LITE_OUTPUT_DIS,B->share[0].coeffs, A->share[1].coeffs);
    // c1 = a1*b1 + r'
    ntt_lite_mac(D->share[1].coeffs,A->share[1].coeffs, B->share[1].coeffs);

}

/**
 * @brief Shift masked polynomial left by specified number of positions
 * @description Shifts each share of polynomial A left by shift_count positions, padding with zeros at the beginning: B = [0, 0, ..., 0, A[0], A[1], ..., A[N-shift_count-1]]. Used in Feistel and other operations requiring coefficient alignment
 * @param B pointer to output masked polynomial (shifted)
 * @param A pointer to input masked polynomial
 * @param shift_count number of positions to shift left (number of leading zeros)
 * @return void
 */
void masked_poly_right_shift(masked_poly *B, const masked_poly *A, size_t shift_count) {

    size_t j;
    size_t i;
    uint32_t c[MASKING_N][N+1];

    
    ntt_lite_set_bound(0);

    for(i = 0; i< MASKING_N; i++){
        for(j=0; j<shift_count; j++){
            c[i][j] = 0x00000;
        }
        ntt_lite_add_const(c[i] + shift_count, A->share[i].coeffs);
    }
    for(i = 0; i< MASKING_N; i++){
        ntt_lite_add_const(B->share[i].coeffs, c[i]);
    }
}

