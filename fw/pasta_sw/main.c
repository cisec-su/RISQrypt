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
#include "pasta.h"

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

void pasta_simple() {

    poly plaintext;
    poly ciphertext;
    size_t sig_len;
    size_t i;
    size_t j;
    uint32_t pasta_key[2 * N];
    uint64_t nonce;
    uint64_t block_ctr;
    int ret;


    BENCH_INIT() 

    print_string("\n --- pasta_sw test --- \n");

    for (i = 0; i < N; i++) {
        plaintext.coeffs[i] = (1) % Q;
    }
    for (i = 0; i < 2*N; i++) {
        pasta_key[i] = (1) % Q;  
    }

    nonce = 123456789;

    BENCH_START()
    pasta_encrypt_one_block_soft(&ciphertext, &plaintext, pasta_key, nonce);
    BENCH_END(PASTA_ENCRYPT_ONE_BLOCK_SOFT)
    print_u32_arr(ciphertext.coeffs,5);
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
    matmul_soft(&C, &A, 0,0,0);
    BENCH_END(PASTA_POLY_MULT)
    print_u32_arr(C.coeffs,5);
    TEST_ASSERT_EQUAL_INT(0, ret);

}

int main() {
    uint32_t seed[2] = {1, 1};
    UnityBegin("main.c");
    x2x_seed(seed);
    print_string("\n --- Pasta Unity Test Start --- \n");
    RUN_TEST(pasta_simple);
    RUN_TEST(poly_mult_soft);
    return(UnityEnd());
}

