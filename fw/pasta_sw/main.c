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
#include "masked_pasta_soft.h"

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

static const uint32_t PASTA_SW_EXP_CIPHERTEXT[] = {
0x00003e54, 0x0000c08b, 0x0000670c, 0x000012a5, 0x000065d4, 0x00001784, 0x0000c57b, 0x00002bcd,
0x0000d582, 0x000026d4, 0x0000aadb, 0x0000d83f, 0x0000071a, 0x000041b7, 0x00007995, 0x0000c816,
0x000045a7, 0x0000515b, 0x0000c27c, 0x00008b46, 0x0000a848, 0x00006271, 0x0000459d, 0x0000ab3a,
0x00007479, 0x0000fa00, 0x00003455, 0x00009287, 0x0000138a, 0x0000bba0, 0x0000b636, 0x000016af,
0x0000dffe, 0x0000e43e, 0x000040d2, 0x0000ea81, 0x00008caf, 0x0000183b, 0x00004df6, 0x00000f47,
0x000083e1, 0x00008471, 0x0000d02f, 0x0000d178, 0x0000cf15, 0x0000df3f, 0x0000abab, 0x00000328,
0x00003bf6, 0x00009bca, 0x00007473, 0x0000a271, 0x0000edba, 0x0000448e, 0x0000cad4, 0x0000ba2a,
0x00001bca, 0x00001b13, 0x0000ae71, 0x0000ff73, 0x0000827f, 0x000059fb, 0x0000dc57, 0x0000ee76,
0x0000fd75, 0x0000091e, 0x00004530, 0x00004a3b, 0x00007d92, 0x00004c4b, 0x000058c8, 0x0000a916,
0x0000e5b9, 0x0000d516, 0x0000b033, 0x0000c1c2, 0x000014d7, 0x000043c5, 0x00000b9f, 0x00002538,
0x00008792, 0x00008f9d, 0x00008997, 0x00003f9f, 0x000011af, 0x0000ae32, 0x0000277e, 0x0000f5eb,
0x00000465, 0x0000f3ca, 0x0000c9e9, 0x000064a1, 0x00002b1f, 0x00001588, 0x0000e3e1, 0x0000bb1f,
0x0000ebcb, 0x0000481f, 0x0000cf5d, 0x0000c797, 0x0000b283, 0x000019ce, 0x00006267, 0x0000e453,
0x000031ec, 0x00000928, 0x00002c73, 0x00000b47, 0x00009358, 0x00007b2a, 0x000051a9, 0x000081b0,
0x0000dac8, 0x0000140c, 0x0000b009, 0x000019d1, 0x0000ff44, 0x0000f6e8, 0x0000d2db, 0x000055bb,
0x0000e74c, 0x0000492e, 0x0000f0f3, 0x00009cf0, 0x0000b4ca, 0x00008f73, 0x0000af61, 0x00004330,
};

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
        A.coeffs[i] = 0x10000;//(1<<15) % Q;
        B.coeffs[i] = 0x10000;//(1<<15) % Q;
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
        A.coeffs[i] = 0x10000;//(1<<15) % Q;
        B.coeffs[i] = 0x10000;//(1<<15) % Q;
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
        A.coeffs[i] = 0x10000;//(1<<15) % Q;
        B.coeffs[i] = 0x10000;//(1<<15) % Q;
    }

    BENCH_INIT()
    BENCH_START()
    C.coeffs[0] = MOD_Q_MULT((uint32_t)A.coeffs[0], (uint32_t)B.coeffs[0]);
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
        A.coeffs[i] = 0x10000;//(1<<15) % Q;
        B.coeffs[i] = 0x10000;//(1<<15) % Q;
    }

    volatile uint32_t a;
    volatile uint32_t b;
    volatile uint32_t c;

    BENCH_INIT()
    BENCH_START()
    C.coeffs[0] = MOD_Q_ADD(A.coeffs[0], B.coeffs[0]); // 19-cycle
    BENCH_END(PASTA_COEFF_ADD)

    TEST_ASSERT_EQUAL_INT(0, ret);
}

void pasta_test() {

    poly plaintext;
    poly ciphertext;
    uint32_t pasta_key[2 * N];
    uint64_t nonce;
    uint64_t block_ctr;
    size_t i;

    // assign plaintext from constant array
    for (i = 0; i < N; i++) {
        plaintext.coeffs[i] = PASTA_PLAINTEXT[i];
    }
    print_string("\n --- pasta_sw test --- \n");

    BENCH_INIT()
    BENCH_START()
    pasta_soft_key_gen((int32_t *)pasta_key, &nonce, &block_ctr);
    BENCH_END(PASTA_KEY_GEN)

    /* perform software encryption */
    BENCH_START()
    pasta_soft_encrypt(&ciphertext, &plaintext, (int32_t *)pasta_key, nonce);
    BENCH_END(PASTA_SOFT_ENCRYPT)
    TEST_ASSERT_EQUAL_HEX32_ARRAY(PASTA_SW_EXP_CIPHERTEXT, ciphertext.coeffs, N);
}

/**
 * @brief Test masked PASTA software encryption
 * @description Verifies masked_pasta_soft_encrypt produces same result as unmasked version
 */
void masked_pasta_soft_test() {
    static poly plaintext;
    static poly ciphertext_masked;
    uint32_t pasta_key[2 * N];
    uint64_t nonce;
    uint64_t block_ctr;
    size_t i;

    // Setup plaintext from constant array
    for (i = 0; i < N; i++) {
        plaintext.coeffs[i] = PASTA_PLAINTEXT[i];
    }

    print_string("\n --- masked_pasta_soft_encrypt test --- \n");

    BENCH_INIT()
    BENCH_START()
    pasta_soft_key_gen((int32_t *)pasta_key, &nonce, &block_ctr);
    BENCH_END(PASTA_KEY_GEN)

    // Run masked encryption
    BENCH_START()
    masked_pasta_soft_encrypt(&ciphertext_masked, &plaintext, (int32_t *)pasta_key, nonce);
    BENCH_END(MASKED_PASTA_SOFT_ENCRYPT)

    // Verify against test vector
    TEST_ASSERT_EQUAL_HEX32_ARRAY(PASTA_SW_EXP_CIPHERTEXT, ciphertext_masked.coeffs, N);
}

int main() {

    UnityBegin("main.c");
    print_string("\n --- Pasta Test Start --- \n");
#ifdef USE_BARRETT
    print_string("Reduction: Barrett\n");
#else
    print_string("Reduction: % Q\n");
#endif
    RUN_TEST(pasta_test);
    RUN_TEST(masked_pasta_soft_test);
    RUN_TEST(poly_uniform_soft);
    RUN_TEST(poly_mul_soft);
    RUN_TEST(poly_add_soft);
    RUN_TEST(coeff_mul_soft);
    RUN_TEST(coeff_add_soft);
    return(UnityEnd());
}