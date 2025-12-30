#include <stdint.h>
#include <stdlib.h>
#include "timer.h"
#include "uart.h"
#include "util.h"
#include "api.h"
#include "benchmark.h"
#include "unity.h"

// Includes for tests
#include "context.h"
#include "hash.h"
#include "fors.h"
#include "thash.h"
#include "wotsx1.h"
#include "params.h"
#include "randombytes.h"

#define SPX_MLEN 32
#define NTESTS 10

void setUp(void) {
}

void tearDown(void) {
}

void print_str(const char *s) {
    uart_transmit_string(s, strlen(s));
}

void print_ull(unsigned long long n) {
    char buf[32];
    int i = 0;
    if (n == 0) {
        print_str("0");
        return;
    }
    while (n > 0) {
        buf[i++] = (n % 10) + '0';
        n /= 10;
    }
    for (int j = 0; j < i / 2; j++) {
        char t = buf[j];
        buf[j] = buf[i - 1 - j];
        buf[i - 1 - j] = t;
    }
    buf[i] = 0;
    print_str(buf);
}

// --- Helpers for Benchmark ---
static int cmp_llu(const void *a, const void*b) {
  if(*(unsigned long long *)a < *(unsigned long long *)b) return -1;
  if(*(unsigned long long *)a > *(unsigned long long *)b) return 1;
  return 0;
}

static unsigned long long median(unsigned long long *l, size_t llen) {
  qsort(l,llen,sizeof(unsigned long long),cmp_llu);
  if(llen%2) return l[llen/2];
  else return (l[llen/2-1]+l[llen/2])/2;
}

static void wots_gen_pkx1(unsigned char *pk, const spx_ctx* ctx, uint32_t addr[8]) {
    struct leaf_info_x1 leaf;
    unsigned steps[ SPX_WOTS_LEN ] = { 0 };
    INITIALIZE_LEAF_INFO_X1(leaf, addr, steps);
    wots_gen_leafx1(pk, ctx, 0, &leaf);
}

// --- Tests ---

void test_fors(void) {
    spx_ctx ctx;
    unsigned char pk1[SPX_FORS_PK_BYTES];
    unsigned char pk2[SPX_FORS_PK_BYTES];
    unsigned char sig[SPX_FORS_BYTES];
    unsigned char m[SPX_FORS_MSG_BYTES];
    uint32_t addr[8] = {0};

    randombytes(ctx.sk_seed, SPX_N);
    randombytes(ctx.pub_seed, SPX_N);
    randombytes(m, SPX_FORS_MSG_BYTES);
    randombytes((unsigned char *)addr, 8 * sizeof(uint32_t));

    initialize_hash_function(&ctx);

    fors_sign(sig, pk1, m, &ctx, addr);
    fors_pk_from_sig(pk2, sig, m, &ctx, addr);

    TEST_ASSERT_EQUAL_MEMORY(pk1, pk2, SPX_FORS_PK_BYTES);
}

void test_spx(void) {
    unsigned char pk[SPX_PK_BYTES];
    unsigned char sk[SPX_SK_BYTES];
    // Use static to avoid stack overflow on small embedded stacks
    static unsigned char m[SPX_MLEN];
    static unsigned char sm[SPX_BYTES + SPX_MLEN];
    static unsigned char mout[SPX_BYTES + SPX_MLEN];
    unsigned long long smlen;
    unsigned long long mlen;
    int ret;

    randombytes(m, SPX_MLEN);

    print_str("\n[TEST] SPHINCS+ Signature\n");
    ret = crypto_sign_keypair(pk, sk);
    TEST_ASSERT_EQUAL(0, ret);

    print_str("[TEST] Signing Message\n");
    ret = crypto_sign(sm, &smlen, m, SPX_MLEN, sk);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT32(SPX_BYTES + SPX_MLEN, smlen);

    print_str("[TEST] Verifying Signature\n");
    ret = crypto_sign_open(mout, &mlen, sm, smlen, pk);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_UINT32(SPX_MLEN, mlen);
    TEST_ASSERT_EQUAL_MEMORY(m, mout, SPX_MLEN);

    print_str("[TEST] Fault Injection Test\n");
    // Fault injection test
    sm[smlen - 1] ^= 1;
    ret = crypto_sign_open(mout, &mlen, sm, smlen, pk);
    TEST_ASSERT_NOT_EQUAL(0, ret);
    sm[smlen - 1] ^= 1; // Restore
}

void test_benchmark(void) {
    print_str("\n[BENCHMARK] Starting...\n");
    
    spx_ctx ctx;
    unsigned char pk[SPX_PK_BYTES];
    unsigned char sk[SPX_SK_BYTES];
    static unsigned char m[SPX_MLEN];
    static unsigned char sm[SPX_BYTES + SPX_MLEN];
    static unsigned char mout[SPX_BYTES + SPX_MLEN];
    unsigned char fors_pk[SPX_FORS_PK_BYTES];
    unsigned char fors_m[SPX_FORS_MSG_BYTES];
    unsigned char fors_sig[SPX_FORS_BYTES];
    unsigned char addr[SPX_ADDR_BYTES];
    unsigned char block[SPX_N];
    unsigned char wots_pk[SPX_WOTS_PK_BYTES];
    unsigned long long smlen;
    unsigned long long mlen;
    unsigned long long t[NTESTS];
    int i;

    randombytes(m, SPX_MLEN);
    randombytes(addr, SPX_ADDR_BYTES);
    
    // Initialize context for lower level calls
    initialize_hash_function(&ctx); 

    #define MEASURE_LOOP(TEXT, COUNT, FNCALL) \
        print_str(TEXT); \
        for(i = 0; i < NTESTS; i++) { \
            timer_start(); \
            for(int j=0; j<COUNT; j++) { FNCALL; } \
            unsigned long long end = (unsigned long long)timer_read(); \
            t[i] = end/COUNT; \
        } \
        print_ull(median(t, NTESTS)); \
        print_str(" cycles\n");

    #define MEASURE(TEXT, FNCALL) MEASURE_LOOP(TEXT, 1, FNCALL)

    MEASURE_LOOP("thash: ", 100, thash(block, block, 1, &ctx, (uint32_t*)addr));
    MEASURE("Keypair: ", crypto_sign_keypair(pk, sk));
    MEASURE("WOTS pk gen: ", wots_gen_pkx1(wots_pk, &ctx, (uint32_t *) addr));
    MEASURE("Sign: ", crypto_sign(sm, &smlen, m, SPX_MLEN, sk));
    MEASURE("FORS sign: ", fors_sign(fors_sig, fors_pk, fors_m, &ctx, (uint32_t *) addr));
    MEASURE("Verify: ", crypto_sign_open(mout, &mlen, sm, smlen, pk));
}

int main(void) {
    UnityBegin("main.c");
    print_string("\n --- SPHINCS+ Unity Test Start --- \n");
    RUN_TEST(test_fors);
    RUN_TEST(test_spx);
    RUN_TEST(test_benchmark);
    return(UnityEnd());
}
