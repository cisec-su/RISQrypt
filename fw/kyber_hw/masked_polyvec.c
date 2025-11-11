#include "rng.h"
#include "masked_gadgets.h"
#include "ntt_lite.h"
#include "masked_cbd.h"
#include "masked_symmetric.h"
#include "masked_poly.h"
#include "masked_polyvec.h"
#include "x2x.h"


void masked_polyvec_ntt(masked_polyvec *r) {
    unsigned int i, j;
    for (i = 0; i < MASKING_N; i++) {
        for (j = 0; j < KYBER_K; j++) {
            ntt_lite_forward_ntt((uint32_t*) r->share[i].vec[j].coeffs, (uint32_t*) r->share[i].vec[j].coeffs);
        }
    }
}


void masked_polyvec_pointwise_acc_invntt_i(masked_polyvec *r, const masked_polyvec *a, const polyvec *b, unsigned int i) {
    unsigned int j;
    for (j = 0; j < MASKING_N; j++) {
        polyvec_pointwise_acc_invntt(&(r->share[j].vec[i]), &(a->share[j]), b);
    }
}


void masked_polyvec_pointwise_acc_invntt_add_i(masked_polyvec *r, const masked_polyvec *a, const polyvec *b, const masked_polyvec *c, unsigned int i) {
    unsigned int j;
    for (j = 0; j < MASKING_N; j++) {
        polyvec_pointwise_acc_core(&(r->share[j].vec[i]), &(a->share[j]), b, 1, 1, 0);
        ntt_lite_set_clr_with_twiddle();
        ntt_lite_add((uint32_t*) &(r->share[j].vec[i]), NTT_LITE_INPUT_DIS, (uint32_t*) &(c->share[j].vec[i].coeffs));
    }
}



void masked_polyvec_pointwise_acc_invntt(masked_poly *r, const masked_polyvec *a, const polyvec *b) {
    unsigned int i;
    for (i = 0; i < MASKING_N; i++) {
        polyvec_pointwise_acc_core(&(r->share[i]), &(a->share[i]), b, 1, 0, 1);
    }
}


void masked_polyvec_pointwise_acc_invntt_addchain(masked_poly *r, const masked_polyvec *a, const polyvec *b, const masked_poly *c, const masked_poly *d) {
    unsigned int i;
    for (i = 0; i < MASKING_N; i++) {
        polyvec_pointwise_acc_core(&(r->share[i]), &(a->share[i]), b, 1, 1, 0);
        ntt_lite_add((uint32_t*) &(r->share[i]), NTT_LITE_INPUT_DIS, (uint32_t*) &(c->share[i].coeffs));
        ntt_lite_set_clr_with_twiddle();
        ntt_lite_add((uint32_t*) &(r->share[i]), NTT_LITE_INPUT_DIS, (uint32_t*) &(d->share[i].coeffs));
    }
}


void masked_polyvec_pointwise_acc_invntt_tohw(masked_poly *r, const masked_polyvec *a, const polyvec *b) {
    int i;
    for (i = (MASKING_N - 1); i >= 0; i--) {
        polyvec_pointwise_acc_core(&(r->share[i]), &(a->share[i]), b, 1, (i == 0), 1);
    }
}


void masked_polyvec_add(masked_polyvec *r, const masked_polyvec *a, const masked_polyvec *b) {
    unsigned int i, j;
    for (i = 0; i < MASKING_N; i++) {
        for (j = 0; j < KYBER_K; j++) {
            poly_add(&(r->share[i].vec[j]), &(a->share[i].vec[j]), &(b->share[i].vec[j]));
        }
    }
}


void masked_polyvec_sub_compress(masked_polyvec_u32 *r, const masked_polyvec *a, const uint8_t *b) {
    unsigned int i;
    const poly *a_[MASKING_N];
    poly_u32 *r_[MASKING_N];
    for (i = 0; i < KYBER_K; i++) {
        a_[0] = &(a->share[0].vec[i]);
        a_[1] = &(a->share[1].vec[i]);
        r_[0] = &(r->share[0].vec[i]);
        r_[1] = &(r->share[1].vec[i]);
        masked_poly_sub_compress_du(r_, a_, b + (i * (KYBER_POLYVECCOMPRESSEDBYTES / 3)), i == 0);
    }
}



void masked_polyvec_mask(masked_polyvec *r, const polyvec *a) {
    masked_gadgets_mask_polyvec(r, a);
}


void masked_polyvec_getnoise_eta1(masked_polyvec *r, const masked_sym seed, uint8_t *nonce) {
#if MASKING_N != 2
#error "This implementation requires MASKING_N = 2"
#endif
    uint8_t buf[MASKING_N][(KYBER_N*KYBER_ETA1*2)/8];
    masked_ptr ptr = {buf[0], buf[1]};
    unsigned int i;
    for (i = 0; i < KYBER_K; i++) {
        masked_prf(ptr, sizeof(buf) / MASKING_N, seed, (*nonce)++);
        masked_cbd_eta1_i(r, buf, i);
    }    
}


void masked_polyvec_getnoise_eta2(masked_polyvec *r, const masked_sym seed, uint8_t *nonce) {
#if KYBER_ETA2 == KYBER_ETA1
    masked_polyvec_getnoise_eta1(r, seed, nonce);
#else
#if MASKING_N != 2
#error "This implementation requires MASKING_N = 2"
#endif
    uint8_t buf[MASKING_N][(KYBER_N*KYBER_ETA2*2)/8];
    masked_ptr ptr = {buf[0], buf[1]};
    unsigned int i;
    for (i = 0; i < KYBER_K; i++) {
        masked_prf(ptr, sizeof(buf) / MASKING_N, seed, (*nonce)++);
        masked_cbd_eta2_i(r, buf, i);
    }
#endif    
}


void masked_polyvec_u32_acc(masked_u32 r, const masked_polyvec_u32 *a, const masked_poly_u32 *b) {
    unsigned int i, j;
    const uint32_t mu[2] = {MU_EXP_L, MU_EXP_H};
    uint32_t *src;

    ntt_lite_load_q(Q_EXP, mu, 8, 32, 0, NTT_LITE_MODE_SINGLE);

    for (j = 0; j < MASKING_N; j++) {
        for (i = 0; i < KYBER_K; i++) {
            if (i == 0) {
                src = (uint32_t*) &(b->share[j].coeffs);
            }
            else {
                src = NTT_LITE_INPUT_DIS;
            }
            ntt_lite_add(NTT_LITE_OUTPUT_DIS, src, (uint32_t*) &(a->share[j].vec[i].coeffs));
        }
        ntt_lite_sum(&(r[j][0]), NTT_LITE_INPUT_DIS);
    }

}
