#include "masked_poly.h"
#include "masked_gadgets.h"
#include "ntt_lite.h"

void masked_poly_mask(masked_poly *r, const poly *a) {
    masked_gadgets_mask_poly(r, a);
}

void masked_poly_add_unmasked(masked_poly *r, poly *a, masked_poly *b){
    ntt_lite_add(r->share[0].coeffs,a->coeffs,b->share[0].coeffs);
}

void masked_poly_add(masked_poly *r, masked_poly *a, masked_poly *b){

    /*
     a = a0 + a1
     b = b0 + b1
     r = r0 + r1 := (a0+b0) + (a1+b1)
    */
    unsigned int i;
    for(i = 0;i<MASKING_N; i++){
        ntt_lite_set_clr_with_twiddle();
        ntt_lite_add(r->share[i].coeffs,a->share[i].coeffs,b->share[i].coeffs);
    }
}


/*
    ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, temp3.coeffs);
    ntt_lite_add(temp5.coeffs, NTT_LITE_INPUT_DIS, temp4.coeffs);
*/
void masked_poly_mult_add_const(masked_poly *r, masked_poly *a, masked_poly *b){
    
    /*
        CONST
        a = a0 a1 
        b = b0 b1
        r = r0 r1 
        r0 = CONST*a0+b0  
        r1 = CONST*a1+b1
        r = CONST*a+b
    */
   
    unsigned int i;
    for(i = 0;i<MASKING_N; i++){
        ntt_lite_set_clr_with_twiddle();
        ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, a->share[i].coeffs);
        ntt_lite_add(r->share[i].coeffs,NTT_LITE_INPUT_DIS,b->share[i].coeffs);
    }

}

void masked_poly_unmask(poly *a, const masked_poly *r) {
    unsigned int i;
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