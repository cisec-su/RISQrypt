#include <stddef.h>
#include "rng.h"
#include "masked.h"
#include "ntt_lite.h"
#include "x2x.h"
#include "masked_gadgets.h"
#include "util.h"


void masked_gadgets_init_q() {
    x2x_set_modulus(KYBER_Q, 12, X2X_MODULUS_PRIME, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_EN);
}


void masked_gadgets_mask_polyvec(masked_polyvec *r, const polyvec *a) {
    unsigned int k;

    for(k = 0; k < KYBER_K; k++) {
        x2x_a_share((uint32_t*) r->share[1].vec[k].coeffs, (uint32_t*) r->share[0].vec[k].coeffs, (uint32_t*) a->vec[k].coeffs, KYBER_N >> 1);
    }
}


void masked_gadgets_mask_doublesym(uint8_t r[MASKING_N][KYBER_SYMBYTES * 2], const uint8_t a[KYBER_SYMBYTES * 2]) {
    x2x_b_share((uint32_t*) r[0], (uint32_t*) r[1], (uint32_t*) a, KYBER_SYMBYTES >> 1);
}


void masked_gadgets_init_2k(uint32_t p) {
    x2x_set_modulus(p, 0, X2X_MODULUS_POW2, X2X_DUAL_MODE_EN, X2X_REJ_SAMPLE_DIS);
}


void masked_gadgets_init_2k_u32(uint32_t p) {
    x2x_set_modulus(p, 0, X2X_MODULUS_POW2, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
}


void masked_gadgets_A2B_2k(masked_poly *r, const masked_poly *a) {
    x2x_a2b((uint32_t*) r->share[1].coeffs, (uint32_t*) r->share[0].coeffs, (uint32_t*) a->share[1].coeffs, (uint32_t*) a->share[0].coeffs, KYBER_N >> 1);
}


void masked_gadgets_A2B_2k_u32(masked_poly_u32 *r, const masked_poly_u32 *a) {
    x2x_a2b((uint32_t*) r->share[1].coeffs, (uint32_t*) r->share[0].coeffs, (uint32_t*) a->share[1].coeffs, (uint32_t*) a->share[0].coeffs, KYBER_N);
}


void masked_gadgets_B2A_q(masked_poly *r, const masked_poly *a) {       
    x2x_b2a((uint32_t*) r->share[1].coeffs, (uint32_t*) r->share[0].coeffs, (uint32_t*) a->share[1].coeffs, (uint32_t*) a->share[0].coeffs, KYBER_N >> 1);
}


void masked_gadgets_init_q_carrier() {
    x2x_set_modulus(Q_EXP, 31, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
}


void masked_gadgets_B2A_qm_u32_core(poly_u32 *r[MASKING_N], const poly_u32 *a[MASKING_N]) {
    x2x_b2a((uint32_t*) r[1]->coeffs, (uint32_t*) r[0]->coeffs, (uint32_t*) a[1]->coeffs, (uint32_t*) a[0]->coeffs, KYBER_N);
}


void masked_gadgets_B2A_qm_u32(masked_poly_u32 *r, const masked_poly_u32 *a) {
#if (MASKING_N != 2)
#error "This implementation requires MASKING_N = 2"
#endif
    const poly_u32 *a_[MASKING_N];
    poly_u32 *r_[MASKING_N];
    a_[0] = &(a->share[0]);
    a_[1] = &(a->share[1]);
    r_[0] = &(r->share[0]);
    r_[1] = &(r->share[1]);    
    masked_gadgets_B2A_qm_u32_core(r_, a_);
}


void masked_gadgets_B2A_qm_u32_vec(masked_polyvec_u32 *r, const masked_polyvec_u32 *a) {
#if (MASKING_N != 2)
#error "This implementation requires MASKING_N = 2"
#endif
    unsigned int i;
    const poly_u32 *a_[MASKING_N];
    poly_u32 *r_[MASKING_N];
    for (i = 0; i < KYBER_K; i++) {
        a_[0] = &(a->share[0].vec[i]);
        a_[1] = &(a->share[1].vec[i]);
        r_[0] = &(r->share[0].vec[i]);
        r_[1] = &(r->share[1].vec[i]);
        masked_gadgets_B2A_qm_u32_core(r_, a_);
    }
}


// https://eprint.iacr.org/2021/1615.pdf alg.1
int masked_gadgets_zero_test_mul(masked_u32 a) {
#if MASKING_N != 2
#error "This implementation requires MASKING_N = 2"
#endif      
    unsigned int i, j;
    const int buffer_len = 16;
    uint32_t rng_buffer[16];
    uint32_t t[MASKING_N][16];
    uint32_t *src;


    masked_gadgets_init_q_carrier();
    x2x_prng_read_nonzero(rng_buffer, buffer_len);
    // print_string("rng_buffer: ");
    // print_u32_arr(rng_buffer, buffer_len);
    // print_string("\n");

    ntt_lite_set_ctrl(1, 32, NTT_LITE_MODE_SINGLE);

    // print_string("a[0]: ");
    // print_u32_arr(a[0], 2);
    // print_string("\n");
    // print_string("a[1]: ");
    // print_u32_arr(a[1], 2);
    // print_string("\n");


    // TODO: process all shares in same cmd for ntt-lite. we need x2x length flexibility first.

    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_set_bound(0);
        ntt_lite_set_bound(rng_buffer[i << 1]);
        for (j = 0; j < MASKING_N; j++) {
            if (i == 0) {
                src = (uint32_t*) &a[j][0];
            }
            else {
                src = t[j];
            }
            ntt_lite_mul_const(t[j], src, NTT_LITE_INPUT_DIS);
        }
        // print_string("before ref. t[0]: ");
        // print_u32_arr(t[0], 16);
        // print_string("\n");        
        // print_string("t[1]: ");
        // print_u32_arr(t[1], 16);
        // print_string("\n");                
        x2x_b_ref(t[1], t[0], t[1], t[0], 16);
        // print_string("after ref. t[0]: ");
        // print_u32_arr(t[0], 16);
        // print_string("\n");        
        // print_string("t[1]: ");
        // print_u32_arr(t[1], 16);
        // print_string("\n");                
    }

    if (MASKING_N > 2) {
        ntt_lite_add(NTT_LITE_OUTPUT_DIS, t[0], t[1]);
        for (i = 2; i < MASKING_N - 1; i++) {
            ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, t[i]);
        }
        ntt_lite_add(t[0], NTT_LITE_INPUT_DIS, t[MASKING_N - 1]);
    }
    else {
        ntt_lite_add(t[0], t[0], t[1]);
    }

    return (t[0] != 0);
}




void masked_gadgets_unmask_u32(uint32_t *r, const masked_u32 a) {
    unsigned int i;
    uint32_t *src;
    uint32_t *dst;
    for (i = 1; i < MASKING_N; i++) {
        if (i != (MASKING_N - 1)) {
            dst = NTT_LITE_OUTPUT_DIS;
        }
        else {
            dst = r;
        }
        if (i == 1) {
            src = (uint32_t*) a[0];
        }
        else {
            src = NTT_LITE_INPUT_DIS;
        }
        ntt_lite_add(dst, src, a[i]);
    }
}
