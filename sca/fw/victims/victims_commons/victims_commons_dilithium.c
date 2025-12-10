#include "ntt_lite.h"
#include "x2x.h"
#include "keccak.h"
#include "masked_symmetric.h"
#include "masked_gadgets.h"
#ifdef VERBOSE
#include "util.h"
#endif
#include "victims_commons_dilithium.h"
#include "victims_commons_util.h"


#define POLY_SAMPLE_BYTES (N * sizeof(uint32_t))


void vcd_print_poly_shares(const masked_poly *mp, const char *label) {
#ifdef VERBOSE
    print_string(label);
    print_string(" Share 0 Coeffs: ");
    print_u32_arr((uint32_t*) mp->share[0].coeffs, 8);
    print_u32_arr((uint32_t*) mp->share[0].coeffs + N - 8, 8);
    print_string("\n");
    print_string(label);
    print_string(" Share 1 Coeffs: ");
    print_u32_arr((uint32_t*) mp->share[1].coeffs, 8);
    print_u32_arr((uint32_t*) mp->share[1].coeffs + N - 8, 8);
    print_string("\n");
#endif
}


void vcd_print_poly_unmasked(const masked_poly *mp, const char *label) {
#ifdef VERBOSE
    poly temp;
    unsigned int i;

    for (i = 0; i < N; i++) {
        temp.coeffs[i] = (mp->share[0].coeffs[i] + mp->share[1].coeffs[i]) % Q;
    }
    print_string(label);
    print_string(" Unmasked Coeffs: ");
    print_u32_arr((uint32_t*) temp.coeffs, 8);
    print_u32_arr((uint32_t*) temp.coeffs + N - 8, 8);
    print_string("\n");
#endif
}


static void vcd_masked_poly_from_seed_core(uint32_t *dst[MASKING_N], const uint8_t src[VCD_SEED_LEN], uint8_t nonce, uint32_t decode_len) {
    unsigned int i;
    volatile uint32_t pad;
    static uint32_t t[MASKING_N][POLY_SAMPLE_BYTES >> 2];

    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_EN);
    keccak_absorb((uint32_t*) src, (uint32_t*) (src + (VCD_SEED_LEN >> 1)), VCD_SEED_LEN >> 3);
    pad = (SHAKE_PAD << 8) | ((uint32_t) nonce);
    keccak_finish((uint32_t*) &pad);
    keccak_squeeze(t[0], t[1], POLY_SAMPLE_BYTES >> 2);

#ifdef VERBOSE
    print_string("Poly Bytes 0: ");
    print_hex((uint8_t*) t[0], POLY_SAMPLE_BYTES, 0);
    print_string("\n");
    print_string("Poly Bytes 1: ");
    print_hex((uint8_t*) t[1], POLY_SAMPLE_BYTES, 0);
    print_string("\n");
#endif

    poly_init_q();
    for(i = 0; i < MASKING_N; i++) {
        ntt_lite_decode(t[i], t[i], decode_len);
    }
    masked_gadgets_init_q();
    x2x_b2a(t[1], t[0], t[1], t[0], N);

#ifdef VERBOSE
    print_string("Poly t Share 0 Coeffs: ");
    print_u32_arr((uint32_t*) t[0], 8);
    print_u32_arr((uint32_t*) t[0] + N - 8, 8);
    print_string("\n");
    print_string("Poly t Share 1 Coeffs: ");
    print_u32_arr((uint32_t*) t[1], 8);
    print_u32_arr((uint32_t*) t[1] + N - 8, 8);
    print_string("\n");
#endif

    x2x_a_ref(dst[1], dst[0], t[1], t[0], N);
    vcu_ntt_lite_reset_state();
}


void vcd_masked_poly_from_seed(masked_poly *dst, const uint8_t src[VCD_SEED_LEN]) {
    uint32_t *dst_ptr[MASKING_N] = {(uint32_t*) dst->share[0].coeffs, (uint32_t*) dst->share[1].coeffs};
    vcd_masked_poly_from_seed_core(dst_ptr, src, 0x0, 23);
#ifdef VERBOSE
    vcd_print_poly_unmasked(dst, "Poly Masked");
#endif
}


void vcd_masked_poly_gamma2_from_seed(masked_poly *dst, const uint8_t src[VCD_SEED_LEN]) {
    uint32_t *dst_ptr[MASKING_N] = {(uint32_t*) dst->share[0].coeffs, (uint32_t*) dst->share[1].coeffs};
    vcd_masked_poly_from_seed_core(dst_ptr, src, 0x0, 17);
#ifdef VERBOSE
    vcd_print_poly_unmasked(dst, "Poly Masked");
#endif
}
