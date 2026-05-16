#include <stdio.h>
#include <stdint.h>
#include "x2x.h"
#include "util.h"
#include "benchmark.h"
#include "timer.h"
#include "poly.h"
#include "params.h"
#include "rubato.h"
#include "masked_rubato.h"


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
    static poly rubato_key;
    uint64_t nonce;
    uint64_t block_ctr;  // will be filled by rubato_key_gen
    uint32_t seed[2] = {1, 1};

    int i;

    // assign plaintext
    for (size_t i = 0; i < N; i++) {
        plaintext.coeffs[i] = 1 % Q;
        rubato_key.coeffs[i] = 1 % Q;
    }

    BENCH_INIT()
    BENCH_LINE();
    reset_modules_cc(); 
    BENCH_START()
    for (i = 0; i < TEST_NUM; i++) {
        rubato_encrypt(&ciphertext, &plaintext, &rubato_key, nonce, 0);
        (void) ciphertext;
    }
    BENCH_END_SHIFT_COL(RUBATO_ENCRYPT, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();

    // assign plaintext
    for (size_t i = 0; i < N; i++) {
        plaintext.coeffs[i] = 1 % Q;
        rubato_key.coeffs[i] = 1 % Q;
    }

    BENCH_LINE();
    reset_modules_cc(); 
    BENCH_START()
    for (i = 0; i < TEST_NUM; i++) {
        masked_rubato_encrypt(&ciphertext, &plaintext, &rubato_key, nonce, 0);
        (void) ciphertext;
    }
    BENCH_END_SHIFT_COL(MASKED_RUBATO_ENCRYPT, LOG_TEST_NUM);
    print_modules_cc(time);
    BENCH_LINE();

}

int main() {

    uint32_t seed[2] = {1, 1};
    x2x_seed(seed);

    BENCH_LINE();
    BENCH_LINE();
    BENCH_TITLE("Rubato");
    BENCH_LINE();
    BENCH_LINE();
    BENCH_HEADER();
    BENCH_LINE();

    test_api();
    BENCH_LINE();

    print_string("Finished\n");

}