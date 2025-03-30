#include <stddef.h>
#include "rng.h"
#include "masked.h"
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
void masked_gadgets_mul_u32(volatile masked_u32 r, volatile masked_u32 a, volatile masked_u32 b, volatile uint32_t rand) {
#if MASKING_N != 2
#error "This implementation requires MASKING_N = 2"
#endif    
    unsigned int i,j;
    volatile uint32_t t;
    r[0] = a[0] * b[0];
    r[1] = a[1] * b[1];
    t = a[0]*b[1] + rand;
    t = t + a[1]*b[0];
    r[0] -= rand;
    r[1] += t;
}


// https://eprint.iacr.org/2021/1615.pdf alg.18
void masked_gadgets_exp_u32(masked_u32 r, masked_u32 a) {
#if MASKING_N != 2
#error "This implementation requires MASKING_N = 2"
#endif      
    unsigned int i,j;
    volatile uint32_t buf[32];
    volatile uint32_t t[MASKING_N];

    for (i = 0; i < MASKING_N; i++) {
        r[i] = a[i];
    }

    randombytes(buf, sizeof(buf));

    // mask refresh and square
    t[0] += buf[0];
    t[1] -= buf[0];
    masked_gadgets_mul_u32(r, r, t, buf[1]);
    // multiply
    masked_gadgets_mul_u32(r, r, a, buf[2]);

    // mask refresh and square
    t[0] += buf[3];
    t[1] -= buf[3];
    masked_gadgets_mul_u32(r, r, t, buf[4]);

    // mask refresh and square
    t[0] += buf[5];
    t[1] -= buf[5];
    masked_gadgets_mul_u32(r, r, t, buf[6]);
    // multiply
    masked_gadgets_mul_u32(r, r, a, buf[7]);

    for (i = 0; i < 8; i++) {
        // mask refresh and square
        t[0] += buf[8 + i];
        t[1] -= buf[8 + i];
        masked_gadgets_mul_u32(r, r, t, buf[16 + i]);
        // multiply
        masked_gadgets_mul_u32(r, r, a, buf[24 + i]);
    }

}


void masked_gadgets_unmask_u32(uint32_t *r, const masked_u32 a) {
    unsigned int i;
    *r = 0;
    for (i = 0; i < MASKING_N; i++) {
        *r += a[i];
    }
}