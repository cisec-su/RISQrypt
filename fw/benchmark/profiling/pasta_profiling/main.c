#include <stdio.h>
#include <stdint.h>
#include "x2x.h"
#include "util.h"
#include "benchmark.h"
#include "timer.h"
#include "poly.h"
#include "params.h"
#include "masked_pasta.h"
#include "pasta.h"
#include "ntt_lite.h"

extern unsigned int keccak_cc;
extern unsigned int x2x_cc;
extern unsigned int ntt_lite_load_q_cc;
extern unsigned int ntt_lite_set_q_cc;
extern unsigned int ntt_lite_set_ctrl_cc;
extern unsigned int ntt_lite_set_inv2_cc;
extern unsigned int ntt_lite_set_mu_cc;
extern unsigned int ntt_lite_set_bound_cc;
extern unsigned int ntt_lite_set_clr_cc;
extern unsigned int ntt_lite_set_clr_with_twiddle_cc;
extern unsigned int ntt_lite_load_twiddle_cc;
extern unsigned int ntt_lite_load_zeta_cc;
extern unsigned int ntt_lite_read_twiddle_cc;
extern unsigned int ntt_lite_read_poly_cc;
extern unsigned int ntt_lite_forward_ntt_cc;
extern unsigned int ntt_lite_backward_ntt_cc;
extern unsigned int ntt_lite_pwm_cc;
extern unsigned int ntt_lite_mul_const_cc;
extern unsigned int ntt_lite_mac_cc;
extern unsigned int ntt_lite_add_cc;
extern unsigned int ntt_lite_add_const_cc;
extern unsigned int ntt_lite_sub_cc;
extern unsigned int ntt_lite_sub_const_cc;
extern unsigned int ntt_lite_sub_rev_cc;
extern unsigned int ntt_lite_sub_rev_const_cc;
extern unsigned int ntt_lite_sum_cc;
extern unsigned int ntt_lite_encode_cc;
extern unsigned int ntt_lite_decode_cc;
extern unsigned int ntt_lite_cbd_cc;
extern unsigned int ntt_lite_rejsamp_cc;
extern unsigned int ntt_lite_compress_cc;
extern unsigned int ntt_lite_decompress_cc;
extern unsigned int ntt_lite_decompress_floor_cc;
extern unsigned int ntt_lite_decompose_cc;
extern unsigned int ntt_lite_chknorm_cc;
extern unsigned int ntt_lite_make_hint_cc;
extern unsigned int ntt_lite_use_hint_cc;

/**
    secret_key = [1]*256
    plaintext  = [1]*128
    nonce = 0x123456789
*/
////////////////////////////////////////////////////////////////
/**
    secret_key = [1]*256
    plaintext  = [1]*128
    nonce = 0x123456789
*/

const uint32_t PASTA_KEY[] = {
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
};

const uint32_t PASTA_PLAINTEXT[] = {
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
};

const uint64_t PASTA_NONCE = 0x123456789;

const uint64_t PASTA_BLOCK_CTR = 0x0;

#ifdef REJ_SAMP_DIS
static const uint32_t TEMP_TEST_EXP_CIPHERTEXT[] = {
0x0000ab80, 0x00005a10, 0x00000771, 0x00007933, 0x000020db, 0x0000d5b0, 0x0000b85d, 0x0000259d, 0x00007c65, 0x0000c4ef, 0x0000c660, 0x000098be, 0x0000bac0, 0x000032ae, 0x00007403, 0x00000bba, 0x000074d4, 0x000013d1, 0x00005588, 0x0000e68d, 0x00005ed8, 0x0000f65d, 0x0000e3b6, 0x00005da8, 0x00003d16, 0x0000dd5c, 0x00003b0e, 0x0000abba, 0x000003ea, 0x00000272, 0x0000b173, 0x00003c62, 0x00002420, 0x0000b9af, 0x00002996, 0x00001bdd, 0x0000305a, 0x000023f4, 0x0000ccd9, 0x00000ce7, 0x0000f341, 0x000009f0, 0x0000a142, 0x000066f3, 0x0000a5ef, 0x00000db6, 0x0000ec73, 0x00004932, 0x00008b5c, 0x00002c9c, 0x0000b6c1, 0x0000a3e2, 0x00005ab1, 0x0000e4b8, 0x000049c8, 0x0000f1bb, 0x00004d3d, 0x0000ab7c, 0x000051c3, 0x00007ca6, 0x0000348d, 0x0000a2ac, 0x0000685f, 0x00002447, 0x000040b3, 0x00000326, 0x000019fb, 0x0000c285, 0x0000e22d, 0x0000ca77, 0x0000febd, 0x00002c9d, 0x000050f9, 0x0000c460, 0x0000fdfd, 0x00004dec, 0x0000362a, 0x0000a562, 0x0000055c, 0x0000b7d8, 0x0000ca6c, 0x00006671, 0x0000860e, 0x00000f4e, 0x00001111, 0x0000f0b2, 0x0000ad29, 0x00005099, 0x0000f844, 0x00008021, 0x00005599, 0x00002d45, 0x000054d8, 0x0000a8aa, 0x0000a4ec, 0x0000c889, 0x00008406, 0x0000f8af, 0x0000e35d, 0x0000f01c, 0x000029b6, 0x0000182f, 0x0000da32, 0x0000d278, 0x00006524, 0x0000c824, 0x0000088e, 0x0000e67c, 0x00004123, 0x0000ad74, 0x00003946, 0x0000d344, 0x00008cda, 0x0000fce7, 0x00004f23, 0x0000950e, 0x0000de79, 0x0000f39b, 0x00008408, 0x00004637, 0x00000ed7, 0x0000a0db, 0x0000ffb5, 0x00001176, 0x0000bd19, 0x00007901, 0x00001ed8, 0x00000eac};
#else
static const uint32_t TEMP_TEST_EXP_CIPHERTEXT[] = {
0x0000e559, 0x00001d16, 0x0000df7f, 0x000044bb, 0x00006b22, 0x0000d17b, 0x00007d7a, 0x0000bb60, 0x0000962a, 0x0000521e, 0x00005846, 0x0000fd84, 0x00005924, 0x0000b070, 0x0000945d, 0x0000b942, 0x00003f99, 0x0000a5c5, 0x00004af4, 0x0000ae73, 0x00006004, 0x0000dc1a, 0x0000169d, 0x0000b5a5, 0x0000b5a3, 0x00007655, 0x00009b0c, 0x00007b4b, 0x0000892b, 0x00005811, 0x00009b4e, 0x00005cba, 0x00004b1b, 0x0000905a, 0x0000554d, 0x00009351, 0x0000981a, 0x000060b5, 0x00005ad2, 0x00007ce2, 0x00002bbb, 0x00000293, 0x0000e6f0, 0x0000b1b5, 0x0000c1b3, 0x00009051, 0x00003830, 0x000077ac, 0x000044ea, 0x00001369, 0x0000d3e6, 0x0000fa87, 0x000077c8, 0x00003eda, 0x0000f04e, 0x000058b0, 0x00007187, 0x0000fd8b, 0x00002929, 0x00006808, 0x000015a2, 0x0000d8d3, 0x0000f910, 0x0000c445, 0x00006ce7, 0x0000344f, 0x000015d8, 0x000061b2, 0x00001de2, 0x000042c1, 0x000030ce, 0x00005246, 0x000033a1, 0x00002b29, 0x0000c909, 0x00002437, 0x00003f46, 0x0000ba73, 0x0000de86, 0x0000833a, 0x0000cf8e, 0x0000f2ac, 0x00006ebb, 0x00001b92, 0x0000d9d1, 0x00005eb4, 0x00002786, 0x00003d0e, 0x00006e39, 0x00005ec1, 0x00007c21, 0x00000f15, 0x0000a980, 0x00000434, 0x0000701c, 0x000064ec, 0x0000f788, 0x0000e58d, 0x0000e071, 0x0000e90e, 0x000040fe, 0x00003ee2, 0x0000528b, 0x0000d09f, 0x000069b8, 0x0000afbc, 0x00002596, 0x00007c79, 0x0000753b, 0x00006610, 0x0000d994, 0x0000cc9c, 0x000000af, 0x00006a02, 0x000007c7, 0x00003316, 0x000024fb, 0x0000d59a, 0x00009886, 0x0000ab28, 0x000066a3, 0x000084e6, 0x00001ae9, 0x00001b68, 0x0000cb38, 0x000019bc, 0x0000df67, 0x0000c436};
#endif

// #define MASKING_EN

void reset_modules_cc() {
    keccak_cc = 0;
    x2x_cc = 0;
}

static unsigned int ntt_lite_total_cc(void) {
    return ntt_lite_load_q_cc +
           ntt_lite_set_q_cc +
           ntt_lite_set_ctrl_cc +
           ntt_lite_set_inv2_cc +
           ntt_lite_set_mu_cc +
           ntt_lite_set_bound_cc +
           ntt_lite_set_clr_cc +
           ntt_lite_set_clr_with_twiddle_cc +
           ntt_lite_load_twiddle_cc +
           ntt_lite_load_zeta_cc +
           ntt_lite_read_twiddle_cc +
           ntt_lite_read_poly_cc +
           ntt_lite_forward_ntt_cc +
           ntt_lite_backward_ntt_cc +
           ntt_lite_pwm_cc +
           ntt_lite_mul_const_cc +
           ntt_lite_mac_cc +
           ntt_lite_add_cc +
           ntt_lite_add_const_cc +
           ntt_lite_sub_cc +
           ntt_lite_sub_const_cc +
           ntt_lite_sub_rev_cc +
           ntt_lite_sub_rev_const_cc +
           ntt_lite_sum_cc +
           ntt_lite_encode_cc +
           ntt_lite_decode_cc +
           ntt_lite_cbd_cc +
           ntt_lite_rejsamp_cc +
           ntt_lite_compress_cc +
           ntt_lite_decompress_cc +
           ntt_lite_decompress_floor_cc +
           ntt_lite_decompose_cc +
           ntt_lite_chknorm_cc +
           ntt_lite_make_hint_cc +
           ntt_lite_use_hint_cc;
}

void print_modules_cc(unsigned int time, unsigned int shift) {
    unsigned int ntt_total = ntt_lite_total_cc();

    print_string("NTT-Lite cycles:\t");
    print_u32_int(ntt_total >> shift);
    print_string("\n");
    print_string("Keccak cycles:\t");
    print_u32_int(keccak_cc >> shift);
    print_string("\n");
    print_string("X2X cycles:\t");
    print_u32_int(x2x_cc >> shift);
    print_string("\n");
    time = time - ntt_total - keccak_cc - x2x_cc;
    print_string("SW cycles:\t");
    print_u32_int(time >> shift);
    print_string("\n");
    reset_modules_cc();
}

static void ntt_lite_print_one(const char *name, unsigned int g) {
    print_string(name);
    print_string(":\t");
    print_u32_int(g);
    print_string("\n");
}

void ntt_lite_print_profile(void) {
    print_string("NTT-Lite function profile:\n");
    ntt_lite_print_one("ntt_lite_load_q", ntt_lite_load_q_cc);
    ntt_lite_print_one("ntt_lite_set_q", ntt_lite_set_q_cc);
    ntt_lite_print_one("ntt_lite_set_ctrl", ntt_lite_set_ctrl_cc);
    ntt_lite_print_one("ntt_lite_set_inv2", ntt_lite_set_inv2_cc);
    ntt_lite_print_one("ntt_lite_set_mu", ntt_lite_set_mu_cc);
    ntt_lite_print_one("ntt_lite_set_bound", ntt_lite_set_bound_cc);
    ntt_lite_print_one("ntt_lite_set_clr", ntt_lite_set_clr_cc);
    ntt_lite_print_one("ntt_lite_set_clr_with_twiddle", ntt_lite_set_clr_with_twiddle_cc);
    ntt_lite_print_one("ntt_lite_load_twiddle", ntt_lite_load_twiddle_cc);
    ntt_lite_print_one("ntt_lite_load_zeta", ntt_lite_load_zeta_cc);
    ntt_lite_print_one("ntt_lite_read_twiddle", ntt_lite_read_twiddle_cc);
    ntt_lite_print_one("ntt_lite_read_poly", ntt_lite_read_poly_cc);
    ntt_lite_print_one("ntt_lite_forward_ntt", ntt_lite_forward_ntt_cc);
    ntt_lite_print_one("ntt_lite_backward_ntt", ntt_lite_backward_ntt_cc);
    ntt_lite_print_one("ntt_lite_pwm", ntt_lite_pwm_cc);
    ntt_lite_print_one("ntt_lite_mul_const", ntt_lite_mul_const_cc);
    ntt_lite_print_one("ntt_lite_mac", ntt_lite_mac_cc);
    ntt_lite_print_one("ntt_lite_add", ntt_lite_add_cc);
    ntt_lite_print_one("ntt_lite_add_const", ntt_lite_add_const_cc);
    ntt_lite_print_one("ntt_lite_sub", ntt_lite_sub_cc);
    ntt_lite_print_one("ntt_lite_sub_const", ntt_lite_sub_const_cc);
    ntt_lite_print_one("ntt_lite_sub_rev", ntt_lite_sub_rev_cc);
    ntt_lite_print_one("ntt_lite_sub_rev_const", ntt_lite_sub_rev_const_cc);
    ntt_lite_print_one("ntt_lite_sum", ntt_lite_sum_cc);
    ntt_lite_print_one("ntt_lite_encode", ntt_lite_encode_cc);
    ntt_lite_print_one("ntt_lite_decode", ntt_lite_decode_cc);
    ntt_lite_print_one("ntt_lite_cbd", ntt_lite_cbd_cc);
    ntt_lite_print_one("ntt_lite_rejsamp", ntt_lite_rejsamp_cc);
    ntt_lite_print_one("ntt_lite_compress", ntt_lite_compress_cc);
    ntt_lite_print_one("ntt_lite_decompress", ntt_lite_decompress_cc);
    ntt_lite_print_one("ntt_lite_decompress_floor", ntt_lite_decompress_floor_cc);
    ntt_lite_print_one("ntt_lite_decompose", ntt_lite_decompose_cc);
    ntt_lite_print_one("ntt_lite_chknorm", ntt_lite_chknorm_cc);
    ntt_lite_print_one("ntt_lite_make_hint", ntt_lite_make_hint_cc);
    ntt_lite_print_one("ntt_lite_use_hint", ntt_lite_use_hint_cc);
}

static void profile_pasta_encrypt_ntt_functions(poly *ciphertext,
                                                const poly *plaintext,
                                                const uint32_t *pasta_key,
                                                uint64_t nonce) {
    unsigned int start_cc;
    unsigned int total_cc;

    start_cc = timer_read();
    ntt_lite_reset_profile();
#ifdef MASKING_EN
    masked_pasta_encrypt(ciphertext, plaintext, pasta_key, nonce);
#else
    pasta_encrypt(ciphertext, plaintext, pasta_key, nonce);
#endif
    total_cc = timer_read() - start_cc;

    print_modules_cc(total_cc, 0);
    ntt_lite_print_profile();
}

void test() {

    static poly plaintext;
    static poly ciphertext;
    static uint32_t pasta_key[2 * N];
    uint64_t nonce;
    uint64_t block_ctr;  // will be filled by pasta_key_gen

#ifdef MASKING_EN
    print_string("\n -- masked pasta profiling -- \n");
#else
    print_string("\n -- pasta profiling -- \n");
#endif

    // assign plaintext
    for (size_t i = 0; i < N; i++) {
        plaintext.coeffs[i] = 1 % Q;
    }

    BENCH_INIT()
    reset_modules_cc();

    BENCH_START()
    // generate constant parameters for the test (key + nonce + ctr)
    pasta_key_gen(pasta_key, &nonce, &block_ctr);
    BENCH_END(PASTA_KEY_GEN)

    // print_modules_cc(time, 0);
    reset_modules_cc();

    profile_pasta_encrypt_ntt_functions(&ciphertext, &plaintext, pasta_key, nonce);
}

int main() {

    test();
}