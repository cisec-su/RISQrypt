#include "masked_gadgets.h"
#include "params.h"





void masked_gadgets_B2A_q(masked_poly *r, const masked_poly *a) {
    unsigned int i,j;
    int32_t t, k;

    for(j = 0; j < N; j++) {
        t = 0;
        k = 0;
        for(i = 0; i < MASKING_N - 1; i++) {
            r->share[i].coeffs[j] = a->share[i].coeffs[j];
            t = t ^ a->share[i].coeffs[j];
            k = k + a->share[i].coeffs[j];
            if (k >= Q) {
                k -= Q;
            }
        }
        t = t ^ a->share[MASKING_N - 1].coeffs[j];
        r->share[MASKING_N - 1].coeffs[j] = t - k;
        if (r->share[MASKING_N - 1].coeffs[j] < 0) {
            r->share[MASKING_N - 1].coeffs[j] += Q;
        }
    }
}


void masked_gadgets_A2B_q_ptr(masked_poly *r, const poly *a[MASKING_N]) {
    unsigned int i,j;
    uint32_t t, k;

    for(j = 0; j < N; j++) {
        t = 0;
        k = 0;
        for(i = 0; i < MASKING_N - 1; i++) {
            r->share[i].coeffs[j] = a[i]->coeffs[j];
            t = (t + a[i]->coeffs[j]) % Q;
            k = k ^ a[i]->coeffs[j];
        }
        t = (t + a[MASKING_N - 1]->coeffs[j]) % Q;
        r->share[MASKING_N - 1].coeffs[j] = k ^ t;        
    }
}


void masked_gadgets_B2A_2k(masked_poly *r, const masked_poly *a, uint32_t p) {
    unsigned int i,j;
    int32_t t, k;

    for(j = 0; j < N; j++) {
        t = 0;
        k = 0;
        for(i = 0; i < MASKING_N - 1; i++) {
            r->share[i].coeffs[j] = a->share[i].coeffs[j];
            t = t ^ a->share[i].coeffs[j];
            k = (k + a->share[i].coeffs[j]) & p;
        }
        t = t ^ a->share[MASKING_N - 1].coeffs[j];
        r->share[MASKING_N - 1].coeffs[j] = (t - k) & p;
    }
}


void masked_gadgets_A2B_2k(masked_poly *r, const masked_poly *a, uint32_t p) {
    unsigned int i,j;
    uint32_t t, k;

    for(j = 0; j < N; j++) {
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