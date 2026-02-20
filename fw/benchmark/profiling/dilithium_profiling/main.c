#include <stdio.h>
#include <stdint.h>
#include "x2x.h"
#include "sign.h"
#include "masked_sign.h"
#include "util.h"
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


void test_api() {
    static uint8_t pk[CRYPTO_PUBLICKEYBYTES]  __attribute__((aligned(4)));
    static uint8_t sk[CRYPTO_SECRETKEYBYTES] __attribute__((aligned(4)));
    static uint8_t sig[CRYPTO_BYTES] __attribute__((aligned(4)));
    static uint8_t msg[32] __attribute__((aligned(4)));
    uint32_t msg_len = 32;
    unsigned int sig_len;
    int i, ret;

    BENCH_INIT();
    reset_modules_cc();
    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        crypto_sign_keypair(pk, sk);
        (void) pk;
        (void) sk;
    }

    BENCH_END_SHIFT_COL(SIGN_KEYPAIR, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();

    msg[0] = 0;

    reset_modules_cc();
    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        msg[0] += 1;
        crypto_sign_signature(sig, &sig_len, msg, msg_len, sk);
        (void) sig;
    }

    BENCH_END_SHIFT_COL(SIGN_SIGNATURE, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();

    msg[0] = 0;

    reset_modules_cc();
    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        msg[0] += 1;
        masked_crypto_sign_signature(sig, &sig_len, msg, msg_len, sk);
        (void) sig;
    }

    BENCH_END_SHIFT_COL(MASKED_SIGN_SIGNATURE, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();

    reset_modules_cc();
    BENCH_START();

    for (int i = 0; i < TEST_NUM; i++) {
        ret = crypto_sign_verify(sig, CRYPTO_BYTES, msg, msg_len, pk);
        (void) ret;
    }

    BENCH_END_SHIFT_COL(SIGN_VERIFY, LOG_TEST_NUM);
    print_modules_cc(time);
}

int main() {
    uint32_t seed[2] = {2, 3};
    x2x_seed(seed);

    BENCH_LINE();
    BENCH_LINE();
    BENCH_TITLE("Dilithium");
    BENCH_LINE();
    BENCH_LINE();
    BENCH_HEADER();
    BENCH_LINE();

    test_api();
    BENCH_LINE();

    print_string("Finished\n");
}