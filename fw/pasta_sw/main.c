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

    BENCH_INIT()
    print_string("\n --- pasta_sw test --- \n");

    /* generate constant key/nonce/ctr once */
    BENCH_START()
    pasta_soft_key_gen((int32_t *)pasta_key, &nonce, &block_ctr);
    // assign plaintext
    for (size_t i = 0; i < N; i++) {
        plaintext.coeffs[i] = 1 % Q;
    }
    BENCH_END(PASTA_KEY_GEN)

    /* perform software encryption */
    BENCH_START()
    pasta_soft_encrypt(&ciphertext, &plaintext, pasta_key, nonce);
    BENCH_END(PASTA_SOFT_ENCRYPT)
    // print_u32_arr(ciphertext.coeffs, 5);
    TEST_ASSERT_EQUAL_INT(0, ret); 
}

void poly_mult_soft() {
    size_t sig_len;
    size_t i;
    size_t j;
    int ret;
    BENCH_INIT()

    print_string("\n --- pasta_key pasta_key --- \n");

    poly A;
    poly B;
    poly C;

    for (i = 0; i < N; i++) {
        B.coeffs[i] = (1<<15) % Q;
        A.coeffs[i] = (i*i) % Q;
    }
    
    BENCH_START()
    // pasta_soft_matmul(&C, &A, 0,0,0);
    // pasta_soft_poly_pointwise_mult(&C, &A, &B);

    C.coeffs[0] = ((B.coeffs[0] * A.coeffs[0]) % Q ) + A.coeffs[1] % Q;

    BENCH_END(PASTA_POLY_MULT)
    // print_u32_arr(C.coeffs,5);
    TEST_ASSERT_EQUAL_INT(0, ret);

}

int main() {
    uint32_t seed[2] = {1, 1};
    UnityBegin("main.c");
    x2x_seed(seed);
    print_string("\n --- Pasta Unity Test Start --- \n");
    RUN_TEST(pasta_test);
    // RUN_TEST(poly_mult_soft);
    return(UnityEnd());
}

