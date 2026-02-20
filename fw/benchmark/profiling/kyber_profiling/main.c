#include <stdint.h>
#include "string.h"
#include "timer.h"
#include "uart.h"
#include "util.h"
#include "indcpa.h"
#include "kem.h"
#include "masked_indcpa.h"
#include "masked_kem.h"
#include "x2x.h"
#include "benchmark.h"
#include "timer.h"


#define LOG_TEST_NUM 10
#define TEST_NUM (1 << LOG_TEST_NUM)


extern unsigned int ntt_lite_cc;
extern unsigned int keccak_cc;
extern unsigned int x2x_cc;




void reset_modules_cc() {
    ntt_lite_cc = 0;
    keccak_cc = 0;
    x2x_cc = 0;
}


void print_modules_cc(unsigned int time) {
    BENCH_END_SHIFT_COL_T(NTT-Lite, LOG_TEST_NUM, ntt_lite_cc);
    BENCH_END_SHIFT_COL_T(Keccak, LOG_TEST_NUM, keccak_cc);
    BENCH_END_SHIFT_COL_T(X2X, LOG_TEST_NUM, x2x_cc);
    time = time - ntt_lite_cc - keccak_cc - x2x_cc;
    BENCH_END_SHIFT_COL_T(SW, LOG_TEST_NUM, time);
}


void test_indcpa_api() {
    static uint8_t pk_cpa[KYBER_INDCPA_PUBLICKEYBYTES];
    static uint8_t sk_cpa[KYBER_INDCPA_SECRETKEYBYTES];
    static uint8_t coins[KYBER_SYMBYTES];
    static uint8_t m[KYBER_SYMBYTES];
    static uint8_t c_cpa[KYBER_INDCPA_BYTES];
    static uint8_t m_[KYBER_SYMBYTES];
    static masked_msg mm;
    static masked_sym masked_coins;
    int i, ret;

    BENCH_INIT();
    reset_modules_cc();
    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        indcpa_keypair(pk_cpa, sk_cpa);
        (void) pk_cpa;
        (void) sk_cpa;
    }

    BENCH_END_SHIFT_COL(INDCPA_KEYPAIR, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();

    reset_modules_cc();
    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        indcpa_enc(c_cpa, m, pk_cpa, coins);
        (void) c_cpa;
        (void) m;
    }

    BENCH_END_SHIFT_COL(INDCPA_ENC, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();

    reset_modules_cc();
    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        indcpa_dec(m, c_cpa, sk_cpa);
        (void) m;
    }

    BENCH_END_SHIFT_COL(INDCPA_DEC, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();

    reset_modules_cc();
    BENCH_START();

    for (int i = 0; i < TEST_NUM; i++) {
        masked_indcpa_dec(mm, c_cpa, sk_cpa);
        (void) mm;
    }

    BENCH_END_SHIFT_COL(MASKED_INDCPA_DEC, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();

    reset_modules_cc();
    BENCH_START();

    for (int i = 0; i < TEST_NUM; i++) {
        ret = masked_indcpa_enc_cmp(c_cpa, mm, pk_cpa, masked_coins);
        (void) ret;
    }

    BENCH_END_SHIFT_COL(MASKED_INDCPA_ENCCMP, LOG_TEST_NUM);
    print_modules_cc(time);
}


void test_indcca_api() {
    static uint8_t pk_cca[KYBER_PUBLICKEYBYTES];
    static uint8_t sk_cca[KYBER_SECRETKEYBYTES];
    static uint8_t K[KYBER_SSBYTES];
    static uint8_t c_cca[KYBER_CIPHERTEXTBYTES];
    static uint8_t KK[KYBER_SSBYTES];
    int i;

    BENCH_INIT();
    reset_modules_cc();
    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        crypto_kem_keypair(pk_cca, sk_cca);
        (void) pk_cca;
        (void) sk_cca;
    }

    BENCH_END_SHIFT_COL(KEM_KEYPAIR, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();


    reset_modules_cc();
    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        crypto_kem_enc(c_cca, K, pk_cca);
        (void) c_cca;
        (void) K;
    }

    BENCH_END_SHIFT_COL(KEM_ENC, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();

    reset_modules_cc();
    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        crypto_kem_dec(KK, c_cca, sk_cca);
        (void) KK;
    }

    BENCH_END_SHIFT_COL(KEM_DEC, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();

    reset_modules_cc();
    BENCH_START();

    for (int i = 0; i < TEST_NUM; i++) {
        masked_crypto_kem_dec(KK, c_cca, sk_cca);
        (void) KK;
    }

    BENCH_END_SHIFT_COL(MASKED_KEM_DEC, LOG_TEST_NUM);
    print_modules_cc(time);
}



int main() {
    uint32_t seed[2] = {1, 1};
    x2x_seed(seed);

    BENCH_LINE();
    BENCH_LINE();
    BENCH_TITLE("Kyber");
    BENCH_LINE();
    BENCH_LINE();
    BENCH_HEADER();
    BENCH_LINE();

    test_indcpa_api();
    BENCH_LINE();
    test_indcca_api();
    BENCH_LINE();

    print_string("Finished\n");
}