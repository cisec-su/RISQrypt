#include "masked_gadgets.h"
#include "params.h"
#include "x2x.h"
#include "util.h"



void masked_gadgets_B2A_q(masked_poly *r, const masked_poly *a) { //DONE

    
    x2x_set_modulus(Q, 23, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_b2a(r->share[1].coeffs, r->share[0].coeffs, a->share[1].coeffs, a->share[0].coeffs, N);
    
    /*unsigned int i,j;
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
    }*/
}


void masked_gadgets_A2B_q_ptr(masked_poly *r, const poly *a[MASKING_N]) {
    
    x2x_set_modulus(Q, 23, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_a2b(r->share[1].coeffs, r->share[0].coeffs, a[1]->coeffs, a[0]->coeffs, N);

    /*unsigned int i,j;
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
    }*/
}


void masked_gadgets_A2B_q(masked_poly *r, const masked_poly *a) {//DONE
    /*unsigned int i;
    const poly *ptr[MASKING_N];
    for (i = 0; i < MASKING_N; i++) {
        ptr[i] = &a->share[i];
    }
    masked_gadgets_A2B_q_ptr(r, ptr);*/

    x2x_set_modulus(Q, 23, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_a2b(r->share[1].coeffs, r->share[0].coeffs, a->share[1].coeffs, a->share[0].coeffs, N);
}


void masked_gadgets_B2A_2k(masked_poly *r, const masked_poly *a, uint32_t p) {//DONE
    
    x2x_set_modulus(p, 0, X2X_MODULUS_POW2, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_b2a(r->share[1].coeffs, r->share[0].coeffs, a->share[1].coeffs, a->share[0].coeffs, N);
    
    /*unsigned int i,j;
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
    }*/
}


void masked_gadgets_A2B_2k(masked_poly *r, const masked_poly *a, uint32_t p) {//DONE
    
    x2x_set_modulus(p, 0, X2X_MODULUS_POW2, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_DIS);
    x2x_a2b(r->share[1].coeffs, r->share[0].coeffs, a->share[1].coeffs, a->share[0].coeffs, N);
    
    /*unsigned int i,j;
    uint32_t t, k;
    print_u32(p);print_string("\n");
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
    }*/
}