#include "ntt_lite.h"
#include "x2x.h"
#include "keccak.h"
#include "masked_symmetric.h"
#ifdef VERBOSE
#include "util.h"
#endif
#include "victims_commons_kyber.h"
#include "victims_commons_util.h"


void vck_print_poly_shares(const masked_poly *mp, const char *label) {
#ifdef VERBOSE
    print_string(label);
    print_string(" Share 0 Coeffs: ");
    print_u32_arr((uint32_t*) mp->share[0].coeffs, 8);
    print_u32_arr((uint32_t*) mp->share[0].coeffs + KYBER_N/2 - 8, 8);
    print_string("\n");
    print_string(label);
    print_string(" Share 1 Coeffs: ");
    print_u32_arr((uint32_t*) mp->share[1].coeffs, 8);
    print_u32_arr((uint32_t*) mp->share[1].coeffs + KYBER_N/2 - 8, 8);
    print_string("\n");
#endif
}


void vck_print_polyvec_shares(const masked_polyvec *mpv, const char *label) {
#ifdef VERBOSE
    for (size_t i = 0; i < KYBER_K; i++) {
        print_string(label);
        print_string("[");
        print_u32_int(i);
        print_string("]");
        print_string(" Share 0 Coeffs: ");
        print_u32_arr((uint32_t*) mpv->share[0].vec[i].coeffs, 8);
        print_u32_arr((uint32_t*) mpv->share[0].vec[i].coeffs + KYBER_N/2 - 8, 8);
        print_string("\n");
        print_string(label);
        print_string("[");
        print_u32_int(i);
        print_string("]");
        print_string(" Share 1 Coeffs: ");
        print_u32_arr((uint32_t*) mpv->share[1].vec[i].coeffs, 8);
        print_u32_arr((uint32_t*) mpv->share[1].vec[i].coeffs + KYBER_N/2 - 8, 8);
        print_string("\n");
    }
#endif
}


static void vck_poly_init_q_uniform32() {
  const uint32_t q = KYBER_Q;
  const uint32_t mu[2] = {0x680bb054, 0x13afb7};
  ntt_lite_load_q(q, mu, 8, 12, 1, NTT_LITE_MODE_SINGLE);
}

static void vck_masked_poly_from_seed_core(uint32_t *dst[MASKING_N], const uint8_t src[KYBER_SYMBYTES], uint8_t nonce) {

    unsigned int i;
    uint32_t pad;
    uint32_t t[MASKING_N][POLY_SAMPLE_BYTES >> 2];

    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_EN);
    keccak_absorb((uint32_t*) src, (uint32_t*) (src + (KYBER_SYMBYTES >> 1)), KYBER_SYMBYTES >> 3);
    pad = (SHAKE_PAD << 8) | ((uint32_t) nonce);
    keccak_finish((uint32_t*) &pad);
    keccak_squeeze(t[0], t[1], POLY_SAMPLE_BYTES >> 2);

#ifdef VERBOSE
    print_string("Poly Bytes 0: ");
    print_hex((uint8_t*) t[0], KYBER_POLYBYTES, 0);
    print_string("\n");
    print_string("Poly Bytes 1: ");
    print_hex((uint8_t*) t[1], KYBER_POLYBYTES, 0);
    print_string("\n");
#endif

    x2x_set_modulus(3329, 12, X2X_MODULUS_PRIME, X2X_DUAL_MODE_DIS, X2X_REJ_SAMPLE_EN);
    x2x_b2a((uint32_t*) t[1], (uint32_t*) t[0], (uint32_t*) t[1], (uint32_t*) t[0], KYBER_N);

#ifdef VERBOSE
    print_string("Poly t Share 0 Coeffs: ");
    print_u32_arr((uint32_t*) t[0], 8);
    print_u32_arr((uint32_t*) t[0] + KYBER_N - 8, 8);
    print_string("\n");
    print_string("Poly t Share 1 Coeffs: ");
    print_u32_arr((uint32_t*) t[1], 8);
    print_u32_arr((uint32_t*) t[1] + KYBER_N - 8, 8);
    print_string("\n");
#endif

    vck_poly_init_q_uniform32();
    ntt_lite_set_bound(0x1);
    for (i = 0; i < MASKING_N; i++) {
#ifdef VERBOSE
        ntt_lite_mul_const(t[i], t[i], NTT_LITE_INPUT_DIS);
        print_string("Poly Reduced32 Share Coeffs: ");
        print_u32_arr((uint32_t*) t[i], 8);
        print_u32_arr((uint32_t*) t[i] + KYBER_N - 8, 8);
        print_string("\n");
#else
        ntt_lite_mul_const(NTT_LITE_OUTPUT_DIS, t[i], NTT_LITE_INPUT_DIS);
#endif
        ntt_lite_encode(dst[i], NTT_LITE_INPUT_DIS, 16);        
    }
    vcu_ntt_lite_reset_state();
}


void vck_masked_poly_from_seed(masked_poly *dst, const uint8_t src[KYBER_SYMBYTES]) {
    uint32_t *dst_ptr[MASKING_N] = {(uint32_t*) dst->share[0].coeffs, (uint32_t*) dst->share[1].coeffs};
    vck_masked_poly_from_seed_core(dst_ptr, src, 0x0);
}


void vck_masked_polyvec_from_seed(masked_polyvec *dst, const uint8_t src[KYBER_SYMBYTES]) {
    size_t i, j;
    uint32_t *dst_ptr[MASKING_N];
    for (i = 0; i < KYBER_K; i++) {
        for (j = 0; j < MASKING_N; j++) {
            dst_ptr[j] = (uint32_t*) dst->share[j].vec[i].coeffs;
        }
        vck_masked_poly_from_seed_core(dst_ptr, src, i);
    }
}


void vck_masked_msg_from_seed(masked_msg dst, const uint8_t src[KYBER_SYMBYTES]) {
    unsigned int i;
    uint32_t pad;

    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_EN);
    keccak_absorb((uint32_t*) src, (uint32_t*) (src + (KYBER_SYMBYTES >> 1)), KYBER_SYMBYTES >> 3);
    pad = SHAKE_PAD;
    keccak_finish((uint32_t*) &pad);
    keccak_squeeze((uint32_t*) (dst[0]), (uint32_t*) (dst[1]), KYBER_SYMBYTES >> 2);
}


void vck_print_msg_shares(const masked_msg mm, const char *label) {
#ifdef VERBOSE
    print_string(label);
    print_string(" Share 0: ");
    print_hex(mm[0], KYBER_INDCPA_MSGBYTES, 0);
    print_string("\n");
    print_string(label);
    print_string(" Share 1: ");
    print_hex(mm[1], KYBER_INDCPA_MSGBYTES, 0);
    print_string("\n");
#endif
}