#include <stdint.h>
#include "ntt_lite.h"
#include "reduce.h"
#include "masked_gadgets.h"
#include "masked_symmetric.h"
#include "masked_cbd.h"
#include "masked_poly.h"


void masked_poly_frommsg(masked_poly *a, const masked_msg msg) {

    unsigned int i;
    uint32_t t[KYBER_N >> 1];
    const uint32_t q_half_ceil_dual = (((KYBER_Q >> 1) + 1) << 16) | ((KYBER_Q >> 1) + 1);
    const uint32_t q = KYBER_Q;
    const uint32_t mu = 0x13af; 
    const uint32_t inv2 = 0x681;
    
    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_decode((uint32_t*) &a->share[i], (uint32_t*) msg[i], 1);
    }

    masked_gadgets_B2A_q(a, a);

    for (i = 0; i < (KYBER_N >> 1); i++) {
        t[i] = q_half_ceil_dual;
    }

    ntt_lite_load_q(q, &mu, 7, 12, inv2, NTT_LITE_MODE_DUAL);  

    for (i = 0; i < MASKING_N; i++) {
        if (i == 0) {
            ntt_lite_pwm((uint32_t*) &a->share[i], (uint32_t*) &a->share[i], (uint32_t*) t);
        }
        else {
            ntt_lite_pwm((uint32_t*) &a->share[i], (uint32_t*) &a->share[i], NTT_LITE_INPUT_DIS);
        }
    }

}



/* https://eprint.iacr.org/2022/158: Algorithm 15
 * Distinctively, we use mod c+alpha+1 during decompress.
 */
static void masked_poly_compress_core(poly *r[MASKING_N], const poly *a[MASKING_N], uint32_t d) {

    unsigned int i;
    const uint32_t q = KYBER_Q << 1;
    const uint32_t mu = 0x275f;
    const uint32_t inv2 = 0x681;
    const uint32_t alpha = 12 + LOG_MASKING_N;
    const uint32_t alpha_dual = ((1 << (alpha - 1)) << 16) | (1 << (alpha - 1));
    const uint32_t d_ = alpha + 1 + KYBER_DV;
    uint32_t t[KYBER_N];
    masked_poly_u32 mpu32;
    uint32_t *dst;

    ntt_lite_load_q(q, &mu, 8, 13, inv2, NTT_LITE_MODE_SINGLE);

    for (i = 0; i < MASKING_N; i++) {
        if (i == MASKING_N - 1 && 0x0) {
            dst = NTT_LITE_OUTPUT_DIS;
        }
        else {
            dst = (uint32_t*) mpu32.share[i].coeffs;
        }
        ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) a[i]->coeffs, 16);
        ntt_lite_compress(dst, NTT_LITE_INPUT_DIS, d_);
    }

    for (i = 0; i < (KYBER_N); i++) {
        t[i] = alpha_dual;
    }
    ntt_lite_load_q((1 << d_), &mu, 8, 13, inv2, NTT_LITE_MODE_SINGLE);
    ntt_lite_add((uint32_t*) mpu32.share[MASKING_N - 1].coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) t);

    masked_gadgets_A2B_2k_u32(&mpu32, &mpu32, (1 << d_) - 1);

    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_load_q(1, &mu, 8, KYBER_DV, inv2, NTT_LITE_MODE_SINGLE);
        ntt_lite_encode((uint32_t*) r[i]->coeffs, mpu32.share[i].coeffs, 16);
    }
}


void masked_poly_compress(masked_poly *r, const masked_poly *a) {
    poly *r_[MASKING_N] = {&r->share[0], &r->share[1]};
    const poly *a_[MASKING_N] = {&a->share[0], &a->share[1]};
    masked_poly_compress_core(r_, a_, KYBER_DV);
}


void masked_poly_compress_du(poly *r[MASKING_N], const poly *a[MASKING_N]) {
    masked_poly_compress_core(r, a, KYBER_DU);
}


/* https://eprint.iacr.org/2022/158: Algorithm 15
 * Distinctively, we use mod c+alpha+1 during decompress.
 */
void masked_poly_sub_tomsg(masked_msg msg, const poly *a, masked_poly *b) {
#if (MASKING_N != 2)
#error "This implementation requires MASKING_N = 2"
#else
    unsigned int i;
    const uint32_t q = KYBER_Q << 1;
    const uint32_t mu = 0x275f;
    const uint32_t inv2 = 0x681;
    const uint32_t alpha = 12 + LOG_MASKING_N;
    const uint32_t alpha_dual = ((1 << (alpha - 1)) << 16) | (1 << (alpha - 1));
    const uint32_t d_ = alpha + 2; // alpha + d + 1
    uint32_t *src;
    uint32_t *dst;
    uint32_t t[KYBER_N >> 1];
    uint32_t zero[KYBER_N >> 1] = {0};

    for (i = 0; i < MASKING_N; i++) {
        if (i == 0) {
            src = (uint32_t*) a->coeffs;
        }
        else {
            src = zero;
        }
        if (i == MASKING_N - 1) {
            dst = NTT_LITE_OUTPUT_DIS;
        }
        else {
            dst = (uint32_t*) b->share[i].coeffs;
        }
        if (i != 0) {
            poly_init_q();
        }
        ntt_lite_sub(NTT_LITE_OUTPUT_DIS, src, (uint32_t*) b->share[i].coeffs);
        ntt_lite_load_q(q, &mu, 7, 13, inv2, NTT_LITE_MODE_POLY);
        ntt_lite_compress(dst, NTT_LITE_INPUT_DIS, d_);
    }

    for (i = 0; i < (KYBER_N >> 1); i++) {
        t[i] = alpha_dual;
    }
    ntt_lite_load_q((1 << d_), &mu, 7, 13, inv2, NTT_LITE_MODE_POLY);
    ntt_lite_add((uint32_t*) b->share[MASKING_N - 1].coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) t);

    masked_gadgets_A2B_2k(b, b, (1 << d_) - 1);

    ntt_lite_load_q(1, &mu, 7, 1, inv2, NTT_LITE_MODE_POLY);

    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_decompress_floor(NTT_LITE_OUTPUT_DIS, (uint32_t*) b->share[i].coeffs, alpha);
        ntt_lite_encode((uint32_t*) msg[i], NTT_LITE_INPUT_DIS, 1);
    }
#endif
}



void masked_poly_getnoise_eta2(masked_poly *r, const masked_sym seed, uint8_t *nonce) {
#if MASKING_N != 2
#error "This implementation requires MASKING_N = 2"
#endif    
    uint8_t buf[MASKING_N][KYBER_ETA1*KYBER_N/4];
    masked_ptr ptr = {buf[0], buf[1]};
    masked_prf(ptr, sizeof(buf) / MASKING_N, seed, (*nonce)++);
    masked_cbd_eta2(r, buf);
}


void masked_poly_add(masked_poly *r, const masked_poly *a, const masked_poly *b) {
    unsigned int i;
    for (i = 0; i < MASKING_N; i++) {
        poly_add(&(r->share[i]), &(a->share[i]), &(b->share[i]));
    }
}


void masked_poly_add_chain(masked_poly *r, const masked_poly *a, const masked_poly *b, const masked_poly *c) {
    unsigned int i;
    for (i = 0; i < MASKING_N; i++) {
        poly_add_chain(&(r->share[i]), &(a->share[i]), &(b->share[i]), &(c->share[i]));
    }
}


void masked_poly_sub_exp(masked_poly *r, const masked_poly *a, const poly *b) {
    unsigned int i;
    for (i = 0; i < MASKING_N; i++) {
        if (i == 0) {
            poly_sub_exp(&(r->share[i]), &(a->share[i]), b);
        }
        else {
            poly_exp(&(r->share[i]), &(a->share[i]));
        }
    }
}


void masked_poly_sub_x(masked_poly *r, const masked_poly *a) {
    unsigned int i;
    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_sub((uint32_t*) &(r->share[i].coeffs), (uint32_t*) &(a->share[i].coeffs), NTT_LITE_INPUT_DIS);
    }
}


void masked_poly_sum(masked_coeff *r, const masked_poly *a) {
    unsigned int i;
    for (i = 0; i < MASKING_N; i++) {
        poly_sum(&((*r)[i]), &(a->share[i]));
    }
}


void masked_poly_coeff_exp(masked_coeff *r, const masked_coeff *a) {
    const uint32_t q = KYBER_Q;
    const uint32_t mu = 0x13af; 
    const uint32_t inv2 = 0x681;
    unsigned int i;
    ntt_lite_load_q(q, &mu, 1, 12, inv2, NTT_LITE_MODE_SINGLE);
    for (i = 0; i < MASKING_N; i++) {
        poly_coeff_exp(&((*r)[i]), &((*a)[i]));
    }
}


void masked_poly_unmask_coeff_inp(masked_coeff *a) {
    unsigned int i;
    for (i = 1; i < MASKING_N; i++) {
        (*a)[0] += (*a)[i];
    }
    csubq((int16_t*) &((*a)[0]));
}
