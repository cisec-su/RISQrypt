#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "params.h"
#include "poly.h"
#include "symmetric.h"
#include "ntt_lite.h"
#include "x2x.h"
#include "util.h"
#include "timer.h"
#include "unity.h"
#include "unity_internals.h"
#include "benchmark.h"
#include "pasta.h"
#include "masked_pasta.h"

#define LOG_TEST_NUM 0
#define TEST_NUM (1 << LOG_TEST_NUM)

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

const uint32_t TEST_KEY[] = {
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
};

const uint64_t TEST_NONCE = 0x123456789;

#ifdef REJ_SAMP_DIS

static const uint32_t TEMP_TEST_EXP_CIPHERTEXT[] = {
0x0000ab80, 0x00005a10, 0x00000771, 0x00007933, 0x000020db, 0x0000d5b0, 0x0000b85d, 0x0000259d, 0x00007c65, 0x0000c4ef, 0x0000c660, 0x000098be, 0x0000bac0, 0x000032ae, 0x00007403, 0x00000bba, 0x000074d4, 0x000013d1, 0x00005588, 0x0000e68d, 0x00005ed8, 0x0000f65d, 0x0000e3b6, 0x00005da8, 0x00003d16, 0x0000dd5c, 0x00003b0e, 0x0000abba, 0x000003ea, 0x00000272, 0x0000b173, 0x00003c62, 0x00002420, 0x0000b9af, 0x00002996, 0x00001bdd, 0x0000305a, 0x000023f4, 0x0000ccd9, 0x00000ce7, 0x0000f341, 0x000009f0, 0x0000a142, 0x000066f3, 0x0000a5ef, 0x00000db6, 0x0000ec73, 0x00004932, 0x00008b5c, 0x00002c9c, 0x0000b6c1, 0x0000a3e2, 0x00005ab1, 0x0000e4b8, 0x000049c8, 0x0000f1bb, 0x00004d3d, 0x0000ab7c, 0x000051c3, 0x00007ca6, 0x0000348d, 0x0000a2ac, 0x0000685f, 0x00002447, 0x000040b3, 0x00000326, 0x000019fb, 0x0000c285, 0x0000e22d, 0x0000ca77, 0x0000febd, 0x00002c9d, 0x000050f9, 0x0000c460, 0x0000fdfd, 0x00004dec, 0x0000362a, 0x0000a562, 0x0000055c, 0x0000b7d8, 0x0000ca6c, 0x00006671, 0x0000860e, 0x00000f4e, 0x00001111, 0x0000f0b2, 0x0000ad29, 0x00005099, 0x0000f844, 0x00008021, 0x00005599, 0x00002d45, 0x000054d8, 0x0000a8aa, 0x0000a4ec, 0x0000c889, 0x00008406, 0x0000f8af, 0x0000e35d, 0x0000f01c, 0x000029b6, 0x0000182f, 0x0000da32, 0x0000d278, 0x00006524, 0x0000c824, 0x0000088e, 0x0000e67c, 0x00004123, 0x0000ad74, 0x00003946, 0x0000d344, 0x00008cda, 0x0000fce7, 0x00004f23, 0x0000950e, 0x0000de79, 0x0000f39b, 0x00008408, 0x00004637, 0x00000ed7, 0x0000a0db, 0x0000ffb5, 0x00001176, 0x0000bd19, 0x00007901, 0x00001ed8, 0x00000eac};
#else
static const uint32_t TEMP_TEST_EXP_CIPHERTEXT[] = {
0x0000e559, 0x00001d16, 0x0000df7f, 0x000044bb, 0x00006b22, 0x0000d17b, 0x00007d7a, 0x0000bb60, 0x0000962a, 0x0000521e, 0x00005846, 0x0000fd84, 0x00005924, 0x0000b070, 0x0000945d, 0x0000b942, 0x00003f99, 0x0000a5c5, 0x00004af4, 0x0000ae73, 0x00006004, 0x0000dc1a, 0x0000169d, 0x0000b5a5, 0x0000b5a3, 0x00007655, 0x00009b0c, 0x00007b4b, 0x0000892b, 0x00005811, 0x00009b4e, 0x00005cba, 0x00004b1b, 0x0000905a, 0x0000554d, 0x00009351, 0x0000981a, 0x000060b5, 0x00005ad2, 0x00007ce2, 0x00002bbb, 0x00000293, 0x0000e6f0, 0x0000b1b5, 0x0000c1b3, 0x00009051, 0x00003830, 0x000077ac, 0x000044ea, 0x00001369, 0x0000d3e6, 0x0000fa87, 0x000077c8, 0x00003eda, 0x0000f04e, 0x000058b0, 0x00007187, 0x0000fd8b, 0x00002929, 0x00006808, 0x000015a2, 0x0000d8d3, 0x0000f910, 0x0000c445, 0x00006ce7, 0x0000344f, 0x000015d8, 0x000061b2, 0x00001de2, 0x000042c1, 0x000030ce, 0x00005246, 0x000033a1, 0x00002b29, 0x0000c909, 0x00002437, 0x00003f46, 0x0000ba73, 0x0000de86, 0x0000833a, 0x0000cf8e, 0x0000f2ac, 0x00006ebb, 0x00001b92, 0x0000d9d1, 0x00005eb4, 0x00002786, 0x00003d0e, 0x00006e39, 0x00005ec1, 0x00007c21, 0x00000f15, 0x0000a980, 0x00000434, 0x0000701c, 0x000064ec, 0x0000f788, 0x0000e58d, 0x0000e071, 0x0000e90e, 0x000040fe, 0x00003ee2, 0x0000528b, 0x0000d09f, 0x000069b8, 0x0000afbc, 0x00002596, 0x00007c79, 0x0000753b, 0x00006610, 0x0000d994, 0x0000cc9c, 0x000000af, 0x00006a02, 0x000007c7, 0x00003316, 0x000024fb, 0x0000d59a, 0x00009886, 0x0000ab28, 0x000066a3, 0x000084e6, 0x00001ae9, 0x00001b68, 0x0000cb38, 0x000019bc, 0x0000df67, 0x0000c436};


#endif


void setUp(void)
{
}

void tearDown(void)
{
}


/**
 * @brief Prints all coefficients of a polynomial
 * 
 * @param p Pointer to the poly structure
 */
void print_poly(const poly *p, size_t len) {
    size_t i;
    for (i = 0; i < len; i++) {
        print_u32(i);
        print_string(":\t0x");
        print_u32((uint32_t)p->coeffs[i]);
        print_string("\n");
    }
}

void masked_pasta_test() {
    size_t i;
    int ret;
    poly plaintext;
    poly ciphertext;

    for (i = 0; i < N; i++) {
        plaintext.coeffs[i] = PASTA_PLAINTEXT[i];
    }

    BENCH_INIT()

    BENCH_START()
    masked_pasta_encrypt(&ciphertext, &plaintext, (int32_t *)PASTA_KEY, PASTA_NONCE);
    BENCH_END(MASKED_PASTA_ENCRYPT)

}


void pasta_test() {
    size_t i;
    int ret;
    poly plaintext;
    poly ciphertext;

    for (i = 0; i < N; i++) {
        plaintext.coeffs[i] = PASTA_PLAINTEXT[i];
    }
    BENCH_INIT()
    BENCH_START()
    pasta_encrypt(&ciphertext, &plaintext, (int32_t *)PASTA_KEY, PASTA_NONCE);
    BENCH_END(PASTA_ENCRYPT)

}

void pasta_poly_mul_test() {
    size_t i;
    int ret;
    poly A,B,C;

    for (size_t i = 0; i < N; i++) {
        A.coeffs[i] = 1 % Q;
        B.coeffs[i] = 1 % Q;
    }

    BENCH_INIT()
    BENCH_START()
    poly_pointwise(&C, &A, &B);
    BENCH_END(PASTA_POLY_POINTWISE_MUL)
}

void pasta_poly_add_test() {
    size_t i;
    int ret;
    poly A,B,C;

    for (size_t i = 0; i < N; i++) {
        A.coeffs[i] = 1 % Q;
        B.coeffs[i] = 1 % Q;
    }
    BENCH_INIT()
    BENCH_START()
    poly_add(&C, &A, &B);
    BENCH_END(PASTA_POLY_POINTWISE_ADD)

}

void pasta_poly_uniform_test() {
    size_t i;
    int ret;
    poly A,B,C;

    for (size_t i = 0; i < N; i++) {
        A.coeffs[i] = 1 % Q;
        B.coeffs[i] = 1 % Q;
    }
    
    BENCH_INIT()
    BENCH_START()
    poly_uniform(&A, 0, 0, 0, 0, 0);
    BENCH_END(PASTA_POLY_UNIFORM)
}

static void test_ntt_lite_load_q_bench(void) {
    BENCH_INIT()
    const uint32_t q = Q;
    const uint32_t mu[2] = {0x0000ffff, 0x0000ffff};
    const uint32_t inv2 = 0x8001;

    BENCH_START()
    ntt_lite_load_q(q, mu, 7, 17, inv2, NTT_LITE_MODE_SINGLE);
    BENCH_END(ntt_lite_load_q)
}

static void test_ntt_lite_set_bound_bench(void) {
    BENCH_INIT()
    BENCH_START()
    ntt_lite_set_bound(2);
    BENCH_END(ntt_lite_set_bound)
}

static void test_ntt_lite_set_inv2_bench(void) {
    BENCH_INIT()
    BENCH_START()
    ntt_lite_set_inv2((STREAM128_BLOCKBYTES >> 2) * 4);
    BENCH_END(ntt_lite_set_inv2)
}

static void test_ntt_lite_decode_bench(void) {
    size_t i;
    static poly poly_c;
    static uint32_t decode_buf[128];

    for (i = 0; i < 128; i++) {
        decode_buf[i] = 0x12345678;
    }

    BENCH_INIT()
    BENCH_START()
    ntt_lite_decode((uint32_t *)poly_c.coeffs, decode_buf, 16);
    BENCH_END(ntt_lite_decode)
}

static void test_ntt_lite_rejsamp_bench(void) {
    size_t i;
    static poly poly_c;
    static uint32_t decode_buf[128];

    for (i = 0; i < 128; i++) {
        decode_buf[i] = 0x12345678;
    }

    ntt_lite_set_bound(Q);
    ntt_lite_set_inv2(512);

    BENCH_INIT()
    BENCH_START()
    ntt_lite_rejsamp((uint32_t *)poly_c.coeffs, decode_buf, 17, NTT_LITE_REJSAMP_CENTER_DIS);
    BENCH_END(ntt_lite_rejsamp)
}

static void test_ntt_lite_add_const_bench(void) {
    size_t i;
    static poly poly_a;
    static poly poly_c;

    for (i = 0; i < N; i++) {
        poly_a.coeffs[i] = TEST_KEY[i % 32];
    }

    ntt_lite_set_bound(1);

    BENCH_INIT()
    BENCH_START()
    ntt_lite_add_const((uint32_t *)poly_c.coeffs, (uint32_t *)poly_a.coeffs);
    BENCH_END(ntt_lite_add_const)
}

static void test_ntt_lite_mul_const_bench(void) {
    size_t i;
    static poly poly_a;
    static poly poly_c;

    for (i = 0; i < N; i++) {
        poly_a.coeffs[i] = TEST_KEY[i % 32];
    }

    ntt_lite_set_bound(2);

    BENCH_INIT()
    BENCH_START()
    ntt_lite_mul_const((uint32_t *)poly_c.coeffs, (uint32_t *)poly_a.coeffs);
    BENCH_END(ntt_lite_mul_const)
}

static void test_ntt_lite_pwm_bench(void) {
    size_t i;
    static poly poly_a;
    static poly poly_b;
    static poly poly_c;

    for (i = 0; i < N; i++) {
        poly_a.coeffs[i] = TEST_KEY[i % 32];
        poly_b.coeffs[i] = TEST_KEY[(i + 16) % 32];
    }

    BENCH_INIT()
    BENCH_START()
    ntt_lite_pwm((uint32_t *)poly_c.coeffs, (uint32_t *)poly_a.coeffs, (uint32_t *)poly_b.coeffs);
    BENCH_END(ntt_lite_pwm)
}

static void test_ntt_lite_sum_bench(void) {
    size_t i;
    static poly poly_a;
    static uint32_t sum_dst;

    for (i = 0; i < N; i++) {
        poly_a.coeffs[i] = TEST_KEY[i % 32];
    }

    BENCH_INIT()
    BENCH_START()
    ntt_lite_sum(&sum_dst, (uint32_t *)poly_a.coeffs);
    BENCH_END(ntt_lite_sum)
}

static void test_ntt_lite_add_bench(void) {
    size_t i;
    static poly poly_a;
    static poly poly_b;
    static poly poly_c;

    for (i = 0; i < N; i++) {
        poly_a.coeffs[i] = TEST_KEY[i % 32];
        poly_b.coeffs[i] = TEST_KEY[(i + 16) % 32];
    }

    BENCH_INIT()
    BENCH_START()
    ntt_lite_add((uint32_t *)poly_c.coeffs, (uint32_t *)poly_a.coeffs, (uint32_t *)poly_b.coeffs);
    BENCH_END(ntt_lite_add)
}

static void test_ntt_lite_sub_bench(void) {
    size_t i;
    static poly poly_a;
    static poly poly_b;
    static poly poly_c;

    for (i = 0; i < N; i++) {
        poly_a.coeffs[i] = TEST_KEY[i % 32];
        poly_b.coeffs[i] = TEST_KEY[(i + 16) % 32];
    }

    BENCH_INIT()
    BENCH_START()
    ntt_lite_sub((uint32_t *)poly_c.coeffs, (uint32_t *)poly_a.coeffs, (uint32_t *)poly_b.coeffs);
    BENCH_END(ntt_lite_sub)
}

static void test_ntt_lite_mac_bench(void) {
    size_t i;
    static poly poly_a;
    static poly poly_b;
    static poly poly_c;

    for (i = 0; i < N; i++) {
        poly_a.coeffs[i] = TEST_KEY[i % 32];
        poly_b.coeffs[i] = TEST_KEY[(i + 16) % 32];
        poly_c.coeffs[i] = TEST_KEY[(i + 8) % 32];
    }

    BENCH_INIT()
    BENCH_START()
    ntt_lite_mac((uint32_t *)poly_c.coeffs, (uint32_t *)poly_a.coeffs, (uint32_t *)poly_b.coeffs);
    BENCH_END(ntt_lite_mac)
}

static void test_ntt_lite_set_clr_with_twiddle_bench(void) {
    BENCH_INIT()
    BENCH_START()
    ntt_lite_set_clr_with_twiddle();
    BENCH_END(ntt_lite_set_clr_with_twiddle)
}

static void test_stream128_init_bench(void) {
    BENCH_INIT()
    BENCH_START()
    stream128_init(TEST_NONCE, 0x0, 0);
    BENCH_END(stream128_init)
}

static void test_stream128_squeeze_bench(void) {
    static uint8_t stream_buf[256];

    stream128_init(TEST_NONCE, 0x0, 0);

    BENCH_INIT()
    BENCH_START()
    stream128_squeeze(stream_buf, 256);
    BENCH_END(stream128_squeeze)
}

static void test_stream128_squeezeblocks_bench(void) {
    static uint8_t stream_buf[256];
    uint32_t buf[(STREAM128_BLOCKBYTES>>2)*4]; // 316 -> 128

    stream128_init(TEST_NONCE, 0x0, 0);

    BENCH_INIT()
    BENCH_START()
    stream128_squeezeblocks((uint8_t*) buf, 4);
    BENCH_END(stream128_squeezeblocks)
}

static void pasta_function_benchmark(void) {
    print_string("\n========== FUNCTION BENCHMARKS ==========\n\n");

    test_ntt_lite_load_q_bench();
    test_ntt_lite_set_bound_bench();
    test_ntt_lite_set_inv2_bench();
    test_ntt_lite_decode_bench();
    test_ntt_lite_rejsamp_bench();
    test_ntt_lite_add_const_bench();
    test_ntt_lite_mul_const_bench();
    test_ntt_lite_pwm_bench();
    test_ntt_lite_sum_bench();
    test_ntt_lite_add_bench();
    test_ntt_lite_sub_bench();
    test_ntt_lite_mac_bench();
    test_ntt_lite_set_clr_with_twiddle_bench();
    test_stream128_init_bench();
    test_stream128_squeeze_bench();
    test_stream128_squeezeblocks_bench();
    
    print_string("\n========== FUNCTION BENCHMARKS COMPLETE ==========\n\n");
}

void test_api() {

    static poly plaintext;
    static poly ciphertext;
    static uint32_t pasta_key[2 * N];
    uint64_t nonce;
    uint64_t block_ctr;  // will be filled by pasta_key_gen
    uint32_t seed[2] = {1, 1};

    int i;

    for (size_t i = 0; i < N; i++) {
        plaintext.coeffs[i] = 1 % Q;
    }

    BENCH_INIT()

    BENCH_START()
    for (i = 0; i < TEST_NUM; i++) {
        pasta_key_gen(pasta_key, &nonce, &block_ctr);
        (void) pasta_key;
    }
    BENCH_END_SHIFT_COL(PASTA_KEY_GEN, LOG_TEST_NUM);

    BENCH_START()
    for (i = 0; i < TEST_NUM; i++) {
        pasta_encrypt(&ciphertext, &plaintext, pasta_key, nonce);
        (void) ciphertext;
    }
    BENCH_END_SHIFT_COL(PASTA_ENCRYPT, LOG_TEST_NUM);

    x2x_seed(seed);
    BENCH_START()
    for (i = 0; i < TEST_NUM; i++) {
        masked_pasta_encrypt(&ciphertext, &plaintext, pasta_key, nonce);
        (void) ciphertext;  
    }
    BENCH_END_SHIFT_COL(MASKED_PASTA_ENCRYPT, LOG_TEST_NUM);

}


int main() {
#ifdef REJ_SAMP_DIS
    print_string("Rejection sampling    : DISABLED\n");
#else
    print_string("Rejection sampling    : ENABLED\n");
#endif

#ifdef MEMORY_OPT_DIS
    print_string("Memory optimization   : DISABLED\n");
#else
    print_string("Memory optimization   : ENABLED\n");
#endif

    uint32_t seed[2] = {2, 3};
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
    pasta_test();
    masked_pasta_test();
    pasta_poly_mul_test();
    pasta_poly_add_test();
    pasta_poly_uniform_test();
    pasta_function_benchmark();

    BENCH_LINE();
    BENCH_LINE();
    print_string("Finished\n");
    
}

