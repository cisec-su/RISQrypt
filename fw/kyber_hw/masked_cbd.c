#include "masked_gadgets.h"
#include "ntt_lite.h"
#include "masked_cbd.h"


static void masked_cbd_eta2_core(poly *r[MASKING_N], const uint8_t buf[MASKING_N][(KYBER_N*KYBER_ETA2*2)/8]) {
    unsigned int i, j;
    masked_poly t[4];
    for (i = 0; i < MASKING_N; i++) {
        for (j = 0; j < (KYBER_N*KYBER_ETA2*2)/8; j++) {
            t[0].share[i].coeffs[2*j] = buf[i][j] & 0x1;
            t[1].share[i].coeffs[2*j] = (buf[i][j] >> 1) & 0x1;
            t[2].share[i].coeffs[2*j] = (buf[i][j] >> 2) & 0x1;
            t[3].share[i].coeffs[2*j] = (buf[i][j] >> 3) & 0x1;
            t[0].share[i].coeffs[2*j + 1] = (buf[i][j] >> 4) & 0x1;
            t[1].share[i].coeffs[2*j + 1] = (buf[i][j] >> 5) & 0x1;
            t[2].share[i].coeffs[2*j + 1] = (buf[i][j] >> 6) & 0x1;
            t[3].share[i].coeffs[2*j + 1] = (buf[i][j] >> 7) & 0x1;                        
        }
    }
    for (i = 0; i < 4; i++) {
        masked_gadgets_B2A_q(&t[i], &t[i]);
    }
    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_add(NTT_LITE_OUTPUT_DIS, (uint32_t*) &t[1].share[i].coeffs, (uint32_t*) &t[0].share[i].coeffs);
        ntt_lite_sub(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) &t[2].share[i].coeffs);
        ntt_lite_sub((uint32_t*) r[i]->coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) &t[3].share[i].coeffs);    
    }
}
    


void masked_cbd_eta2(masked_poly *r, const uint8_t buf[MASKING_N][(KYBER_N*KYBER_ETA1*2)/8]) {
#if MASKING_N != 2
#error "This implementation requires MASKING_N = 2"
#endif    
    poly *r_[MASKING_N] = {&r->share[0], &r->share[1]};
    masked_cbd_eta2_core(r_, buf);
}



void masked_cbd_eta2_i(masked_polyvec *r, const uint8_t buf[MASKING_N][(KYBER_N*KYBER_ETA1*2)/8], unsigned int i) {
#if MASKING_N != 2
#error "This implementation requires MASKING_N = 2"
#endif    
    poly *r_[MASKING_N] = {&r->share[0].vec[i], &r->share[1].vec[i]};
    masked_cbd_eta2_core(r_, buf);
}


void masked_cbd_eta1_i(masked_polyvec *r, const uint8_t buf[MASKING_N][(KYBER_N*KYBER_ETA1*2)/8], unsigned int i) {
#if KYBER_ETA1 == 2
    masked_cbd_eta2_i(r, buf, i);
#else
#error "This implementation requires eta1 = 2"
#endif
}