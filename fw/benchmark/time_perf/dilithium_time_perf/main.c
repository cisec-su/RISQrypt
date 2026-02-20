#include <stdio.h>
#include <stdint.h>
#include "x2x.h"
#include "sign.h"
#include "masked_gadgets.h"
#include "poly.h"
#include "masked_poly.h"
#include "masked_sign.h"
#include "masked_symmetric.h"
#include "util.h"
#include "benchmark.h"
#include "timer.h"


#define LOG_TEST_NUM 10
#define TEST_NUM (1 << LOG_TEST_NUM)



void test_masked_poly_uniform() {
    static masked_poly_u mp;
    masked_poly_ptr mp_ptr;
    static const masked_crh rhoprime;
    static const uint32_t buf[MASKING_N][POLYZ_PACKEDBYTES >> 2];
    int i;

    mp_ptr.share[0] = &mp.share[0];
    mp_ptr.share[1] = &mp.share[1];

    BENCH_INIT();

    poly_init_q();
    masked_gadgets_init_q();

    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        masked_poly_ptr_uniform_gamma1_fromhw_inner(&mp_ptr, rhoprime, buf);
        (void) mp_ptr;
    }

    BENCH_END_SHIFT_COL(MASKED_POLY_UNIFORM, LOG_TEST_NUM);
}


void test_masked_poly_uniform_xof() {
    static masked_poly_u mp;
    masked_poly_ptr mp_ptr;
    static const masked_crh rhoprime;
    int i;

    mp_ptr.share[0] = &mp.share[0];
    mp_ptr.share[1] = &mp.share[1];

    BENCH_INIT();

    poly_init_q();
    masked_gadgets_init_q();


    BENCH_START();
    masked_stream256_init(rhoprime, -1);

    for (i = 0; i < TEST_NUM; i++) {
        masked_poly_ptr_uniform_gamma1_fromhw(&mp_ptr, rhoprime, i, 1);
        (void) mp_ptr;
    }

    BENCH_END_SHIFT_COL(MASKED_POLY_UNIFORM_XOF, LOG_TEST_NUM);
}


void test_masked_poly_chknorm() {
    static masked_poly_u mp;
    masked_poly_ptr mp_ptr;
    static masked_poly_u mp2;
    masked_poly_ptr mp2_ptr;
    int i;

    mp_ptr.share[0] = &mp.share[0];
    mp_ptr.share[1] = &mp.share[1];
    mp2_ptr.share[0] = &mp2.share[0];
    mp2_ptr.share[1] = &mp2.share[1];

    BENCH_INIT();

    poly_init_q();
    masked_gadgets_init_q();


    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        masked_poly_ptr_chknorm(&mp_ptr, &mp2_ptr, GAMMA1 - BETA);
        (void) mp_ptr;
    }

    BENCH_END_SHIFT_COL(MASKED_POLY_CHKNORM, LOG_TEST_NUM);
}


void test_masked_poly_decompose() {
    static masked_poly_u mp0;
    masked_poly_ptr mp0_ptr;
    static masked_poly_u mp;
    masked_poly_ptr_const mp_ptr;
    static poly p1;
    int i;

    mp0_ptr.share[0] = &mp0.share[0];
    mp0_ptr.share[1] = &mp0.share[1];
    mp_ptr.share[0] = &mp.share[0];
    mp_ptr.share[1] = &mp.share[1];

    BENCH_INIT();

    poly_init_q();
    masked_gadgets_init_q();

    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        masked_poly_ptr_decompose(&p1, &mp0_ptr, &mp_ptr);
        (void) p1;
        (void) mp_ptr;
    }

    BENCH_END_SHIFT_COL(MASKED_POLY_DECOMPOSE, LOG_TEST_NUM);
}


void test_api() {
    static uint8_t pk[CRYPTO_PUBLICKEYBYTES]  __attribute__((aligned(4)));
    static uint8_t sk[CRYPTO_SECRETKEYBYTES] __attribute__((aligned(4)));
    static uint8_t sig[CRYPTO_BYTES] __attribute__((aligned(4)));
    static uint8_t msg[32] __attribute__((aligned(4))) = {0x00};
    uint32_t msg_len = 32;
    unsigned int sig_len;
    int i, ret;

    BENCH_INIT();
    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        crypto_sign_keypair(pk, sk);
        (void) pk;
        (void) sk;
    }

    BENCH_END_SHIFT_COL(SIGN_KEYPAIR, LOG_TEST_NUM);

    msg[0] = 0;

    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        msg[0] += 1;
        crypto_sign_signature(sig, &sig_len, msg, msg_len, sk);
        (void) sig;
    }

    BENCH_END_SHIFT_COL(SIGN_SIGNATURE, LOG_TEST_NUM);

    msg[0] = 0;

    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        msg[0] += 1;
        masked_crypto_sign_signature(sig, &sig_len, msg, msg_len, sk);
        (void) sig;
    }

    BENCH_END_SHIFT_COL(MASKED_SIGN_SIGNATURE, LOG_TEST_NUM);

    
    BENCH_START();

    for (int i = 0; i < TEST_NUM; i++) {
        ret = crypto_sign_verify(sig, CRYPTO_BYTES, msg, msg_len, pk);
        (void) ret;
    }

    BENCH_END_SHIFT_COL(SIGN_VERIFY, LOG_TEST_NUM);
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
    test_masked_poly_uniform();
    test_masked_poly_uniform_xof();
    test_masked_poly_chknorm();
    test_masked_poly_decompose();

    BENCH_LINE();
    BENCH_LINE();
    print_string("Finished\n");
}