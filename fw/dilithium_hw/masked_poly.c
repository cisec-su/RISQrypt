#include "masked_poly.h"
#include "masked_gadgets.h"
#include "ntt_lite.h"




void masked_poly_mask(masked_poly *r, const poly *a) {
    int i, j;
    for (i = 0; i < MASKING_N; i++) {
        for (j = 0; j < N; j++) {
            if (i == 0) {
                r->share[i].coeffs[j] = a->coeffs[j] - j;
                if (r->share[i].coeffs[j] < 0) {
                    r->share[i].coeffs[j] += Q;
                }
            }
            else {
                r->share[i].coeffs[j] = j;
            }
        }
    }
}


void masked_poly_ntt(masked_poly *r) {
    unsigned int i;

    for(i = 0; i < MASKING_N; i++) {
        poly_ntt(&r->share[i]);
    }
}


void masked_poly_invntt(masked_poly *r) {
    unsigned int i;

    for(i = 0; i < MASKING_N; i++) {
        poly_invntt(&r->share[i]);
    }
}


void masked_poly_uniform_gamma1(masked_poly *y, const masked_crh rhoprime, uint16_t nonce) {
    unsigned int i;
    uint8_t buf[MASKING_N][POLYZ_PACKEDBYTES];
    uint32_t rhs = GAMMA1;
    uint32_t *rhs_ptr;

    dilithium_masked_shake256_absorb_nonce((masked_flat_ptr) buf, POLYZ_PACKEDBYTES, (masked_flat_ptr) rhoprime, CRHBYTES, nonce);

    for(i = 0; i < MASKING_N; ++i) {
        ntt_lite_decode(&y->share[i], (uint32_t*) buf[i], LOG_GAMMA1);
    }

    masked_gadgets_B2A_q(y, y);
    
    for(i = 0; i < MASKING_N; ++i) {
        if (i == 2) {
            rhs_ptr = NTT_LITE_INPUT_DIS;
        }
        else {
            rhs_ptr = &rhs;
        }
        if (i == 1) {
            rhs = 0;
        }
        ntt_lite_sub_rev_const(y->share[i].coeffs, y->share[i].coeffs, rhs_ptr);
    }

}


void masked_poly_pointwise_acc(masked_poly *c, const poly *a, const masked_poly *b) {
    unsigned int i;

    for (i = 0; i < MASKING_N; i++) {
        poly_pointwise_acc(&c->share[i], a, &b->share[i]);
    }
}


void masked_poly_pointwise(masked_poly *c, const poly *a, const masked_poly *b) {
    unsigned int i;
    uint32_t *rhs;

    for (i = 0; i < MASKING_N; i++) {
        if ((i == 0) || 1) {
            rhs = (uint32_t*) &a->coeffs;
        }
        else {
            rhs = NTT_LITE_INPUT_DIS;
        }
        ntt_lite_pwm((uint32_t*) &c->share[i].coeffs, (uint32_t*) &b->share[i].coeffs, rhs);
    }
}


int masked_poly_pointwise_add_invntt_chknorm(masked_poly *r, const masked_poly *v, const poly *c, const masked_poly *u, uint32_t B) {
    masked_poly temp;
    unsigned int i, j;
    int flag;
    poly *ptr[MASKING_N];
    uint32_t B2 = (B << 1) - 1;
    uint32_t *dst;

    ntt_lite_set_bound(B - 1);

    for (i = 0; i < MASKING_N; i++) {

        ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, (uint32_t*) &v->share[i].coeffs, (uint32_t*) &c->coeffs);
        ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) u->share[i].coeffs);
        poly_init_invntt();
        ntt_lite_backward_ntt((uint32_t*) &r->share[i].coeffs, NTT_LITE_INPUT_DIS);
        if (i == 0) {           
            ntt_lite_add_const((uint32_t*) &temp.share[0].coeffs, NTT_LITE_INPUT_DIS, NTT_LITE_INPUT_DIS);
            ptr[0] = &temp.share[0];
        }
        else {
            ptr[i] = &r->share[i];
        }
    }
    // unmask_and_print_coeffs(r, "z");
    for (i = 0; i< N; i++) {
        temp.share[1].coeffs[i] = r->share[1].coeffs[i];
    }
    // unmask_and_print_coeffs(&temp, "after add const");  

    // modulus switching from q to 2^32
    masked_gadgets_A2B_q_ptr(&temp, ptr);
    // unmask_and_print_coeffs_bool(&temp, "after A2B");  
    masked_gadgets_B2A_2k(&temp, &temp, 0xFFFFFFFF);
    // unmask_and_print_coeffs_2k(&temp, "B2A 2k");  

    ntt_lite_set_q(0);
    ntt_lite_set_bound(B2);

    ntt_lite_sub_const((uint32_t*) &temp.share[0].coeffs, (uint32_t*) &temp.share[0].coeffs, NTT_LITE_INPUT_DIS);
    

    // unmask_and_print_coeffs_2k(&temp, "after sub");  
    
    masked_gadgets_A2B_2k(&temp, &temp, 0xFFFFFFFF);
    // unmask_and_print_coeffs_bool(&temp, "after A2B");  


    ntt_lite_set_q(1);
    for (i = 0; i < MASKING_N; i++) {
        if (i == (MASKING_N - 1)) {
            dst = NTT_LITE_OUTPUT_DIS;
        }
        else {
            dst = (uint32_t*) &temp.share[i].coeffs;
        }
        ntt_lite_decompress_floor(dst, (uint32_t*) temp.share[i].coeffs, 31);
    }
    // unmask_and_print_coeffs_bool(&temp, "after sr31");  
    
    // unmasking
    ntt_lite_set_q(2);
    for (i = 0; i < (MASKING_N - 1); i++) {
        ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) temp.share[i].coeffs);
    }
    ntt_lite_set_bound(1);
    ntt_lite_add_const(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, NTT_LITE_INPUT_DIS);

    // print_all_coeffs(temp.share[0], "unmasked");

    ntt_lite_set_bound(0);
    flag = ntt_lite_chknorm(NTT_LITE_INPUT_DIS);
    print_string("flag is \n");
    print_u32(flag);
    print_string("\n");
    poly_set_q(); // reset the modulus to Dilithium's Q
    if (flag == NTT_LITE_CHKNORM_FAIL) {
        return 1;
    }
    else {
        return 0;
    }
}