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



// void masked_gadgets_mask_poly(masked_poly *r, poly *a) {
//     unsigned int i,j;
//     uint16_t t, k;

//     for(j = 0; j < KYBER_N; j++) {
//         k = 0;
//         for(i = 0; i < MASKING_N - 1; i++) {
//             r->share[i].coeffs[j] = rand16();
//             k = k + r->share[i].coeffs[j];
//             if (k >= KYBER_Q) {
//                 k -= KYBER_Q;
//             }
//         }
//         r->share[MASKING_N - 1].coeffs[j] = a->coeffs[j] - k;
//         if (r->share[MASKING_N - 1].coeffs[j] < 0) {
//             r->share[MASKING_N - 1].coeffs[j] += KYBER_Q;
//         }
//     }
// }