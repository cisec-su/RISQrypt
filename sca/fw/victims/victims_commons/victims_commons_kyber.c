#include "ntt_lite.h"
#include "x2x.h"
#include "keccak.h"
#include "masked_symmetric.h"
#ifdef VERBOSE
#include "util.h"
#endif
#include "victims_commons_kyber.h"
#include "victims_commons_util.h"


void vck_print_shares(const masked_poly *mp, const char *label) {
#ifdef VERBOSE
    print_string(label);
    print_string(" Share 0 Coeffs: ");
    print_u32_arr((uint32_t*) mp->share[0].coeffs, 8);
    print_u32_arr((uint32_t*) mp->share[0].coeffs + KYBER_N/2 - 8, 8);
    print_string("\n");
    print_string(label);
    print_string("Share 1 Coeffs: ");
    print_u32_arr((uint32_t*) mp->share[1].coeffs, 8);
    print_u32_arr((uint32_t*) mp->share[1].coeffs + KYBER_N/2 - 8, 8);
    print_string("\n");
#endif
}


static void vck_poly_init_q_uniform32() {
  const uint32_t q = KYBER_Q;
  const uint32_t mu[2] = {0x680bb054, 0x13afb7};
  ntt_lite_load_q(q, mu, 8, 12, 1, NTT_LITE_MODE_SINGLE);
}

void vck_masked_poly_from_seed(masked_poly *dst, const uint8_t src[KYBER_SYMBYTES], uint8_t nonce) {

    unsigned int i;
    uint32_t pad;
    masked_ptr src_ptr = {(uint8_t*) src, (uint8_t*) (src + (KYBER_SYMBYTES >> 1))};
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
        ntt_lite_encode((uint32_t*) &dst->share[i].coeffs, NTT_LITE_INPUT_DIS, 16);        
    }
    vcu_ntt_lite_reset_state();
}