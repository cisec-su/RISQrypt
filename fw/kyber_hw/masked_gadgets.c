#include <stddef.h>
#include "rng.h"
#include "masked.h"
#include "ntt_lite.h"
#include "masked_gadgets.h"




void masked_gadgets_A2B_2k(masked_poly *r, const masked_poly *a, uint32_t p) {
    unsigned int i,j;
    uint16_t t, k;

    for(j = 0; j < KYBER_N; j++) {
        t = 0;
        k = 0;
        for(i = 0; i < MASKING_N - 1; i++) {
            r->share[i].coeffs[j] = a->share[i].coeffs[j];
            t = t + a->share[i].coeffs[j];
            k = k ^ a->share[i].coeffs[j];
            t = t & p;
        }
        t = t + a->share[MASKING_N - 1].coeffs[j];
        t = t & p;
        r->share[MASKING_N - 1].coeffs[j] = k ^ t;        
    }
}


void masked_gadgets_A2B_2k_u32(masked_poly_u32 *r, const masked_poly_u32 *a, uint32_t p) {
    unsigned int i,j;
    uint32_t t, k;

    for(j = 0; j < KYBER_N; j++) {
        t = 0;
        k = 0;
        for(i = 0; i < MASKING_N - 1; i++) {
            r->share[i].coeffs[j] = a->share[i].coeffs[j];
            t = t + a->share[i].coeffs[j];
            k = k ^ a->share[i].coeffs[j];
            t = t & p;
        }
        t = t + a->share[MASKING_N - 1].coeffs[j];
        t = t & p;
        r->share[MASKING_N - 1].coeffs[j] = k ^ t;        
    }
}


void masked_gadgets_B2A_q(masked_poly *r, const masked_poly *a) {
    unsigned int i,j;
    uint16_t t, k;

    for(j = 0; j < KYBER_N; j++) {
        t = 0;
        k = 0;
        for(i = 0; i < MASKING_N - 1; i++) {
            r->share[i].coeffs[j] = a->share[i].coeffs[j];
            t = t ^ a->share[i].coeffs[j];
            k = k + a->share[i].coeffs[j];
            if (k >= KYBER_Q) {
                k -= KYBER_Q;
            }
        }
        t = t ^ a->share[MASKING_N - 1].coeffs[j];
        r->share[MASKING_N - 1].coeffs[j] = t - k;
        if (r->share[MASKING_N - 1].coeffs[j] < 0) {
            r->share[MASKING_N - 1].coeffs[j] += KYBER_Q;
        }
    }
}


void masked_gadgets_B2A_qm_u32_core(poly_u32 *r[MASKING_N], const poly_u32 *a[MASKING_N]) {
    unsigned int i,j;
    uint32_t t, k;

    for(j = 0; j < KYBER_N; j++) {
        t = 0;
        k = 0;
        for(i = 0; i < MASKING_N - 1; i++) {
            r[i]->coeffs[j] = a[i]->coeffs[j];
            t = t ^ a[i]->coeffs[j];
            k = k + a[i]->coeffs[j];
            if (k >= Q_EXP) {
                k -= Q_EXP;
            }
        }
        t = t ^ a[MASKING_N - 1]->coeffs[j];
        r[MASKING_N - 1]->coeffs[j] = Q_EXP + t - k;
        if ((r[MASKING_N - 1]->coeffs[j]) >= Q_EXP) {
            r[MASKING_N - 1]->coeffs[j] -= Q_EXP;
        }
    }
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


void masked_gadgets_B2A_2k_u32_core(poly_u32 *r[MASKING_N], const poly_u32 *a[MASKING_N]) {
    unsigned int i,j;
    uint16_t t, k;

    for(j = 0; j < KYBER_N; j++) {
        t = 0;
        k = 0;
        for(i = 0; i < MASKING_N - 1; i++) {
            r[i]->coeffs[j] = a[i]->coeffs[j];
            t = t ^ a[i]->coeffs[j];
            k = k + a[i]->coeffs[j];
        }
        t = t ^ a[MASKING_N - 1]->coeffs[j];
        r[MASKING_N - 1]->coeffs[j] = t - k;
    }
}


void masked_gadgets_B2A_2k_u32(masked_poly_u32 *r, const masked_poly_u32 *a) {
#if (MASKING_N != 2)
#error "This implementation requires MASKING_N = 2"
#endif
    const poly_u32 *a_[MASKING_N];
    poly_u32 *r_[MASKING_N];
    a_[0] = &(a->share[0]);
    a_[1] = &(a->share[1]);
    r_[0] = &(r->share[0]);
    r_[1] = &(r->share[1]);    
    masked_gadgets_B2A_2k_u32_core(r_, a_);
}


void masked_gadgets_B2A_2k_u32_vec(masked_polyvec_u32 *r, const masked_polyvec_u32 *a) {
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
        masked_gadgets_B2A_2k_u32_core(r_, a_);
    }
}



// https://eprint.iacr.org/2021/1615.pdf alg.17
void masked_gadgets_mul_u32(masked_u32 r, masked_u32 a, masked_u32 b, uint32_t rand) {
#if MASKING_N != 2
#error "This implementation requires MASKING_N = 2"
#endif    
    unsigned int i,j;
    uint32_t t;
    // t = a[0]*b[1] + rand;
    // t = t + a[1]*b[0];
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, &a[0], &b[1]);
    ntt_lite_add(&t, NTT_LITE_INPUT_DIS, &rand);
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, &a[1], &b[0]);
    ntt_lite_add(&t, NTT_LITE_INPUT_DIS, &t);
    // r[0] = (a[0] * b[0]) - rand;
    // r[1] = (a[1] * b[1]) + t;
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, &a[0], &b[0]);
    ntt_lite_sub(&r[0], NTT_LITE_INPUT_DIS, &rand);
    ntt_lite_pwm(NTT_LITE_OUTPUT_DIS, &a[1], &b[1]);
    ntt_lite_add(&r[1], NTT_LITE_INPUT_DIS, &t);

}



// https://eprint.iacr.org/2021/1615.pdf alg.18
void masked_gadgets_exp_u32(masked_u32 r, masked_u32 a) {
#if MASKING_N != 2
#error "This implementation requires MASKING_N = 2"
#endif      
    unsigned int i,j;
    uint32_t buf[63];
    uint32_t t[MASKING_N];
    const uint32_t mu[2] = {MU_EXP_L, MU_EXP_H};
    const uint32_t mask = 0x7FFFFFFF; // Q_EXP is very close to 2**31

    ntt_lite_load_q(Q_EXP, mu, 0, 32, 1, NTT_LITE_MODE_SINGLE);

    for (i = 0; i < MASKING_N; i++) {
        r[i] = a[i];
    }

    randombytes((uint8_t*) buf, sizeof(buf));

    for (i = 0; i < (sizeof(buf) / sizeof(uint32_t)); i++) {
        buf[i] = buf[i] & mask;
    }

    // uses 48 random words
    for (i = 0; i < 24; i++) {
        // mask refresh and square
        ntt_lite_add(&t[0], &r[0], &buf[i]);
        ntt_lite_sub(&t[1], &r[1], &buf[i]);
        masked_gadgets_mul_u32(r, r, t, buf[24 + i]);
    }

    // multiply
    masked_gadgets_mul_u32(r, r, a, buf[48]);

    // uses 14 random words
    for (i = 0; i < 7; i++) {
        // mask refresh and square
        ntt_lite_add(&t[0], &r[0], &buf[49 + i]);
        ntt_lite_sub(&t[1], &r[1], &buf[49 + i]);
        masked_gadgets_mul_u32(r, r, t, buf[56 + i]);
    }

}


static void masked_zero_double(volatile masked_u32 r) {
    unsigned int i;
    for (i = 0; i < MASKING_N; i++) {
        r[(i << 1)] = 0;
    }
}


// https://eprint.iacr.org/2021/1615.pdf alg.1
int masked_gadgets_zero_test_mul(masked_u32 a) {
#if MASKING_N != 2
#error "This implementation requires MASKING_N = 2"
#endif      
    unsigned int i, j;
    volatile uint32_t t[MASKING_N << 1];
    uint32_t buf[MASKING_N];
    uint32_t *src_ptr, *dst_ptr;

    randombytes((uint8_t*) buf, sizeof(buf));

    ntt_lite_set_ctrl(LOG_MASKING_N + 1, 32, NTT_LITE_MODE_SINGLE);


    for (i = 0; i < MASKING_N; i++) {
        t[ i << 1     ] = a[i]; // to avoid transitional leakage
        t[(i << 1) + 1] = 0;
    }

    for (i = 0; i < MASKING_N; i++) {
        if (i == 0) {
            src_ptr = (uint32_t*) &t[0];
        }
        else {
            src_ptr = NTT_LITE_INPUT_DIS;
        }

        ntt_lite_set_bound(buf[i]);
        ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, src_ptr, NTT_LITE_INPUT_DIS);
        masked_zero_double(t);
        ntt_lite_add(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) t);
    }

    ntt_lite_sum((uint32_t*) &t[0], NTT_LITE_INPUT_DIS);

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
            src = (uint32_t*) &a[0];
        }
        else {
            src = NTT_LITE_INPUT_DIS;;
        }
        ntt_lite_add(dst, src, &a[i]);
    }
}