#include <stdint.h>
#include "ntt_lite.h"
#include "masked_gadgets.h"
#include "masked_symmetric.h"
#include "masked_cbd.h"
#include "masked_poly.h"


void masked_poly_frommsg(masked_poly *a, const masked_msg msg) {

    unsigned int i;
    const uint32_t q_half_ceil_dual = (((KYBER_Q >> 1) + 1) << 16) | ((KYBER_Q >> 1) + 1);
    const uint32_t q = (KYBER_Q << 16) | KYBER_Q;
    const uint32_t mu[2] = {0x13afb7, 0x13afb7};
    
    masked_gadgets_B2A_onebit_frommsg(a, msg);

    ntt_lite_set_ctrl(7, 12, NTT_LITE_MODE_DUAL);

    ntt_lite_set_bound(q_half_ceil_dual);

    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_set_clr();
        ntt_lite_mul_const((uint32_t*) &a->share[i], (uint32_t*) &a->share[i]);
    }

}



/* https://eprint.iacr.org/2022/158: Algorithm 15
 * Distinctively, we use mod c+alpha+1 during decompress.
 */
static void masked_poly_sub_compress_core(poly_u32 *r[MASKING_N], const poly *a[MASKING_N], const uint8_t *b, uint32_t d, int init_a2b) {

    unsigned int i;
    const uint32_t q = KYBER_Q << 1;
    const uint32_t mu[2] = {0xb405d82a, 0x9d7db};
    const uint32_t inv2 = 0x6810681;
    const uint32_t alpha = 12 + LOG_MASKING_N;
    const uint32_t alpha_shift = (1 << (alpha));
    const uint32_t alpha_m1_shift = (1 << (alpha - 1));
    const uint32_t d_ = alpha + 1 + d;
    masked_poly_u32 mpu32;
    uint32_t *dst;

    ntt_lite_load_q(q, mu, 8, 13, inv2, NTT_LITE_MODE_SINGLE);


    for (i = 0; i < MASKING_N; i++) {
        if (i == MASKING_N - 1) {
            dst = NTT_LITE_OUTPUT_DIS;
        }
        else {
            dst = (uint32_t*) mpu32.share[i].coeffs;
        }
        ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) a[i]->coeffs, 16);
        if (i != (MASKING_N - 1)) {
            ntt_lite_set_clr_with_twiddle();
        }
        ntt_lite_compress(dst, NTT_LITE_INPUT_DIS, d_);
    }

    ntt_lite_set_bound(alpha_m1_shift);

    ntt_lite_set_q(1 << d_);
    ntt_lite_add_const((uint32_t*) mpu32.share[MASKING_N - 1].coeffs, NTT_LITE_INPUT_DIS);

    ntt_lite_set_q(alpha_shift);
    ntt_lite_set_ctrl(8, 32, NTT_LITE_MODE_SINGLE);
    ntt_lite_decode(NTT_LITE_OUTPUT_DIS, (uint32_t*) b, d);
    ntt_lite_decompress_floor(NTT_LITE_OUTPUT_DIS, NTT_LITE_INPUT_DIS, 0);
    ntt_lite_set_q(1 << d_);
    ntt_lite_set_clr_with_twiddle();
    ntt_lite_sub_rev((uint32_t*) mpu32.share[MASKING_N - 1].coeffs, NTT_LITE_INPUT_DIS, (uint32_t*) mpu32.share[MASKING_N - 1].coeffs);

    if (init_a2b) {
        masked_gadgets_init_2k_u32(0xFFFFFFFF);
    }
    masked_gadgets_A2B_2k_u32(&mpu32, &mpu32);

    ntt_lite_set_q(1);
    ntt_lite_set_ctrl(8, d, NTT_LITE_MODE_SINGLE);
    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_decompress_floor(r[i]->coeffs, (uint32_t*) mpu32.share[i].coeffs, alpha);
    }

}


void masked_poly_sub_compress(masked_poly_u32 *r, const masked_poly *a, const uint8_t *b) {
    poly_u32 *r_[MASKING_N] = {&r->share[0], &r->share[1]};
    const poly *a_[MASKING_N] = {&a->share[0], &a->share[1]};
    masked_poly_sub_compress_core(r_, a_, b, KYBER_DV, 1);
}


void masked_poly_sub_compress_du(poly_u32 *r[MASKING_N], const poly *a[MASKING_N], const uint8_t *b, int init_a2b) {
    masked_poly_sub_compress_core(r, a, b, KYBER_DU, init_a2b);
}


/* https://eprint.iacr.org/2022/158: Algorithm 15
 * Distinctively, we use mod c+alpha+1 during decompress.
 */
void masked_poly_tomsg(masked_msg msg, masked_poly *b) {
#if (MASKING_N != 2)
#error "This implementation requires MASKING_N = 2"
#endif
    unsigned int i;
    const uint32_t q = ((KYBER_Q << 1) << 16) | (KYBER_Q << 1);
    const uint32_t mu[2] = {0x9d7db, 0x9d7db};
    const uint32_t alpha = 12 + LOG_MASKING_N;
    const uint32_t alpha_dual = ((1 << (alpha - 1)) << 16) | (1 << (alpha - 1));
    const uint32_t d_ = alpha + 2;
    const uint32_t d_dual = ((1 << d_) << 16) | (1 << d_);
    uint32_t *dst, *src;
    

    ntt_lite_set_q(q);
    ntt_lite_set_mu(mu, NTT_LITE_MODE_POLY);

    for (i = 0; i < MASKING_N; i++) {
        if (i == MASKING_N - 1) {
            dst = NTT_LITE_OUTPUT_DIS;
        }
        else {
            dst = (uint32_t*) b->share[i].coeffs;
        }
        if (i != (MASKING_N - 1))
            ntt_lite_set_clr();
        ntt_lite_compress(dst, (uint32_t*) b->share[i].coeffs, d_);
    }

    ntt_lite_set_bound(alpha_dual);
    ntt_lite_set_q(d_dual);
    ntt_lite_set_clr();
    ntt_lite_add_const((uint32_t*) b->share[MASKING_N - 1].coeffs, NTT_LITE_INPUT_DIS);

    masked_gadgets_init_2k(0xFFFF);
    masked_gadgets_A2B_2k(b, b);

    ntt_lite_set_q(0x10001);
    ntt_lite_set_ctrl(7, 1, NTT_LITE_MODE_POLY);

    for (i = 0; i < MASKING_N; i++) {
        ntt_lite_decompress_floor(NTT_LITE_OUTPUT_DIS, (uint32_t*) b->share[i].coeffs, alpha);
        ntt_lite_encode((uint32_t*) msg[i], NTT_LITE_INPUT_DIS, 1);
    }
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