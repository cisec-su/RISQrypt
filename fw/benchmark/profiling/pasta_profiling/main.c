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


#define LOG_TEST_NUM 0
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

    static poly plaintext;
    static poly ciphertext;
    static uint32_t pasta_key[2 * N];
    uint64_t nonce;
    uint64_t block_ctr;  // will be filled by pasta_key_gen
    uint32_t seed[2] = {1, 1};

    int i;

    // assign plaintext
    for (size_t i = 0; i < N; i++) {
        plaintext.coeffs[i] = 1 % Q;
    }

    BENCH_INIT()
    reset_modules_cc();
    BENCH_START()
    for (i = 0; i < TEST_NUM; i++) {
        pasta_key_gen(pasta_key, &nonce, &block_ctr);
        (void) pasta_key;
    }
    BENCH_END_SHIFT_COL(PASTA_KEY_GEN, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();

    reset_modules_cc(); 
    BENCH_START()
    for (i = 0; i < TEST_NUM; i++) {
        pasta_encrypt(&ciphertext, &plaintext, pasta_key, nonce);
        (void) ciphertext;
    }
    BENCH_END_SHIFT_COL(PASTA_ENCRYPT, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();

    reset_modules_cc();
    x2x_seed(seed);
    BENCH_START()
    for (i = 0; i < TEST_NUM; i++) {
        masked_pasta_encrypt(&ciphertext, &plaintext, pasta_key, nonce);
        (void) ciphertext;  
    }
    BENCH_END_SHIFT_COL(MASKED_PASTA_ENCRYPT, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();

}

int main() {

    uint32_t seed[2] = {1, 1};
    x2x_seed(seed);

    BENCH_LINE();
    BENCH_LINE();
    BENCH_TITLE("Pasta");
    BENCH_LINE();
    BENCH_LINE();
    BENCH_HEADER();
    BENCH_LINE();

    test_api();
    BENCH_LINE();

    print_string("Finished\n");

}