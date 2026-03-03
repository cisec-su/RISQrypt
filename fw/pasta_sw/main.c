#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "params.h"
#include "x2x.h"
#include "util.h"
#include "timer.h"
#include "unity.h"
#include "unity_internals.h"
#include "benchmark.h"
#include "pasta_soft.h"

////////////////////////////////////////////////////////////////
void setUp(void)
{
}

void tearDown(void)
{
}
////////////////////////////////////////////////////////////////


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

void pasta_test() {

    poly plaintext;
    poly ciphertext;
    uint32_t pasta_key[2 * N];
    uint64_t nonce;
    uint64_t block_ctr;
    int ret;

    // assign plaintext
    for (size_t i = 0; i < N; i++) {
        plaintext.coeffs[i] = 1 % Q;
    }
    print_string("\n --- pasta_sw test --- \n");

    BENCH_INIT()
    BENCH_START()
    pasta_soft_key_gen((int32_t *)pasta_key, &nonce, &block_ctr);
    BENCH_END(PASTA_KEY_GEN)

    /* perform software encryption */
    BENCH_START()
    pasta_soft_encrypt(&ciphertext, &plaintext, pasta_key, nonce);
    BENCH_END(PASTA_SOFT_ENCRYPT)
    // print_u32_arr(ciphertext.coeffs, 5);
    TEST_ASSERT_EQUAL_INT(0, ret); 
}

void poly_uniform_soft() {
    int ret;
    poly A;

    print_string("\n --- pasta_soft_poly_uniform --- \n");

    BENCH_INIT()
    BENCH_START()
    pasta_soft_poly_uniform(&A, 0, 0, 0, 1);
    BENCH_END(PASTA_POLY_UNIFORM)

    TEST_ASSERT_EQUAL_INT(0, ret);
}

void poly_mul_soft() {
    size_t i;
    int ret;

    print_string("\n --- poly_mul_soft --- \n");
    static poly A;
    static poly B;
    static poly C;

    for (i = 0; i < N; i++) {
        B.coeffs[i] = (1<<15) % Q;
        A.coeffs[i] = (i*i) % Q;
    }

    BENCH_INIT()
    BENCH_START()
    pasta_soft_poly_pointwise_mult(&C, &A, &B);
    BENCH_END(PASTA_POLY_POINTWISE_MUL)

    TEST_ASSERT_EQUAL_INT(0, ret);

}

void poly_add_soft() {
    size_t i;
    int ret;

    print_string("\n --- poly_add_soft --- \n");
    static poly A;
    static poly B;
    static poly C;

    for (i = 0; i < N; i++) {
        B.coeffs[i] = (1<<15) % Q;
        A.coeffs[i] = (i*i) % Q;
    }

    BENCH_INIT()
    BENCH_START()
    pasta_soft_poly_pointwise_add(&C, &A, &B);
    BENCH_END(PASTA_POLY_POINTWISE_ADD)

    TEST_ASSERT_EQUAL_INT(0, ret);

}

void coeff_mul_soft() {
    size_t i;
    int ret;

    print_string("\n --- coeff_mul_soft --- \n");
    volatile static poly A;
    volatile static poly B;
    volatile static poly C;

    for (i = 0; i < N; i++) {
        B.coeffs[i] = (1<<15) % Q;
        A.coeffs[i] = (i*i) % Q;
    }

    BENCH_INIT()
    BENCH_START()
    C.coeffs[0] = (A.coeffs[0] * B.coeffs[0]) % Q;
    BENCH_END(PASTA_COEFF_MUL)

    TEST_ASSERT_EQUAL_INT(0, ret);
}

void coeff_add_soft() {
    size_t i;
    int ret;

    print_string("\n --- coeff_add_soft --- \n");
    volatile static poly A;
    volatile static poly B;
    volatile static poly C;

    for (i = 0; i < N; i++) {
        B.coeffs[i] = (1<<15) % Q;
        A.coeffs[i] = (i*i) % Q;
    }

    BENCH_INIT()
    BENCH_START()
    C.coeffs[0] = (A.coeffs[0] + B.coeffs[0]) % Q;
    BENCH_END(PASTA_COEFF_ADD)

    TEST_ASSERT_EQUAL_INT(0, ret);
}

int main() {

    UnityBegin("main.c");
    print_string("\n --- Pasta Test Start --- \n");
    RUN_TEST(pasta_test);
    RUN_TEST(poly_uniform_soft);    
    RUN_TEST(poly_mul_soft);
    RUN_TEST(poly_add_soft);
    RUN_TEST(coeff_mul_soft);
    RUN_TEST(coeff_add_soft);
    return(UnityEnd());
}

