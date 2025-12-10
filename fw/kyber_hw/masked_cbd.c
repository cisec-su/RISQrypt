#include "masked_gadgets.h"
#include "ntt_lite.h"
#include "masked_cbd.h"
#include "x2x.h"
#include "poly.h"



static void masked_cbd_eta2_core(poly *r[MASKING_N], const uint8_t buf[MASKING_N][(KYBER_N*KYBER_ETA2*2)/8]) {
    unsigned int i, j;
    poly t[MASKING_N][4];

    x2x_ref_b2a_1bit((uint32_t*) t[1][0].coeffs, (uint32_t*)  t[0][0].coeffs, (uint32_t*)  &buf[1][0], (uint32_t*)  &buf[0][0], 2, (KYBER_N*KYBER_ETA2*2));
    
    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_add(NTT_LITE_OUTPUT_DIS, (uint32_t*) t[i][1].coeffs, (uint32_t*) t[i][0].coeffs);
        ntt_lite_sub(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, (uint32_t*) t[i][2].coeffs);
        ntt_lite_set_clr_with_twiddle();
        ntt_lite_sub((uint32_t*) r[i]->coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) t[i][3].coeffs);    
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