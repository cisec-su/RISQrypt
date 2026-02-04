#include <stdint.h>
#include <stdlib.h>
#include "timer.h"
#include "uart.h"
#include "util.h"
#include "api.h"
#include "benchmark.h"
#include "unity.h"

#include "context.h"
#include "hash.h"
#include "fors.h"
#include "thash.h"
#include "wotsx1.h"
#include "params.h"
#include "randombytes.h"

#define SPX_MLEN 32

// Test vectors
static unsigned char pk[SPX_PK_BYTES];
static unsigned char sk[SPX_SK_BYTES];
static unsigned char m[SPX_MLEN];
static unsigned char sm[SPX_BYTES + SPX_MLEN];
static unsigned char sm_masked[SPX_BYTES + SPX_MLEN];  // For masked signature
static unsigned char mout[SPX_BYTES + SPX_MLEN];
static size_t smlen;
static size_t smlen_masked;
static size_t mlen;

// Context for component tests
static spx_ctx ctx;

// Helper for WOTS pk generation benchmark
static void wots_gen_pkx1(unsigned char *pk_out, const spx_ctx* ctx_in, uint32_t addr[8]) {
    struct leaf_info_x1 leaf;
    unsigned steps[SPX_WOTS_LEN] = {0};
    INITIALIZE_LEAF_INFO_X1(leaf, addr, steps);
    wots_gen_leafx1(pk_out, ctx_in, 0, &leaf);
}

void setUp(void) {
}

void tearDown(void) {
}

// =============================================================================
// Print SPHINCS+ Parameters
// =============================================================================
void print_parameters(void) {
    print_string("\n========================================\n");
    print_string("       SPHINCS+ Parameter Set\n");
    print_string("========================================\n");
    
    print_string("Security parameter (n):     ");
    print_u32_int(SPX_N);
    print_string(" bytes\n");
    
    print_string("Full tree height:           ");
    print_u32_int(SPX_FULL_HEIGHT);
    print_string("\n");
    
    print_string("Number of layers (d):       ");
    print_u32_int(SPX_D);
    print_string("\n");
    
    print_string("Subtree height:             ");
    print_u32_int(SPX_TREE_HEIGHT);
    print_string("\n");
    
    print_string("FORS tree height:           ");
    print_u32_int(SPX_FORS_HEIGHT);
    print_string("\n");
    
    print_string("FORS trees (k):             ");
    print_u32_int(SPX_FORS_TREES);
    print_string("\n");
    
    print_string("Winternitz parameter (w):   ");
    print_u32_int(SPX_WOTS_W);
    print_string("\n");
    
    print_string("WOTS length:                ");
    print_u32_int(SPX_WOTS_LEN);
    print_string("\n");
    
    print_string("\n--- Key and Signature Sizes ---\n");
    
    print_string("Public key size:            ");
    print_u32_int(SPX_PK_BYTES);
    print_string(" bytes\n");
    
    print_string("Secret key size:            ");
    print_u32_int(SPX_SK_BYTES);
    print_string(" bytes\n");
    
    print_string("Signature size:             ");
    print_u32_int(SPX_BYTES);
    print_string(" bytes (");
    print_u32_int(SPX_BYTES / 1024);
    print_string(" KiB)\n");
    
    print_string("\n--- Component Sizes ---\n");
    
    print_string("FORS signature:             ");
    print_u32_int(SPX_FORS_BYTES);
    print_string(" bytes\n");
    
    print_string("WOTS signature:             ");
    print_u32_int(SPX_WOTS_BYTES);
    print_string(" bytes\n");
    
    print_string("FORS message bytes:         ");
    print_u32_int(SPX_FORS_MSG_BYTES);
    print_string(" bytes\n");
    
    print_string("Address bytes:              ");
    print_u32_int(SPX_ADDR_BYTES);
    print_string(" bytes\n");
    
    print_string("========================================\n\n");
}

// =============================================================================
// Keypair Test
// =============================================================================
void test_sphincs_keypair(void) {
    BENCH_INIT()
    int ret;

    print_string("\n[Keypair] Generating key pair...\n");
    print_string("  Output: pk[");
    print_u32_int(SPX_PK_BYTES);
    print_string("], sk[");
    print_u32_int(SPX_SK_BYTES);
    print_string("]\n");

    BENCH_START()

    ret = crypto_sign_keypair(pk, sk);

    BENCH_END(SPX_KEYPAIR)

    TEST_ASSERT_EQUAL_INT(0, ret);
    print_string("  Result: SUCCESS\n");
}

// =============================================================================
// Sign Test
// =============================================================================
void test_sphincs_sign(void) {
    BENCH_INIT()
    int ret;

    randombytes(m, SPX_MLEN);

    print_string("\n[Sign] Signing message...\n");
    print_string("  Message length:    ");
    print_u32_int(SPX_MLEN);
    print_string(" bytes\n");
    print_string("  Expected sig size: ");
    print_u32_int(SPX_BYTES);
    print_string(" bytes\n");

    BENCH_START()

    ret = crypto_sign(sm, &smlen, m, SPX_MLEN, sk);

    BENCH_END(SPX_SIGN)

    print_string("  Actual sig+msg:    ");
    print_u32_int((uint32_t)smlen);
    print_string(" bytes\n");

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT32(SPX_BYTES + SPX_MLEN, smlen);
    print_string("  Result: SUCCESS\n");
}

// =============================================================================
// Verify Test
// =============================================================================
void test_sphincs_verify(void) {
    BENCH_INIT()
    int ret;

    print_string("\n[Verify] Verifying signature...\n");
    print_string("  Signature length:  ");
    print_u32_int((uint32_t)smlen);
    print_string(" bytes\n");

    BENCH_START()

    ret = crypto_sign_open(mout, &mlen, sm, smlen, pk);

    BENCH_END(SPX_VERIFY)

    print_string("  Recovered msg len: ");
    print_u32_int((uint32_t)mlen);
    print_string(" bytes\n");

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT32(SPX_MLEN, mlen);
    TEST_ASSERT_EQUAL_MEMORY(m, mout, SPX_MLEN);
    print_string("  Result: SUCCESS (message matches)\n");
}

// =============================================================================
// Verify Fail Test - Corrupted signature should fail
// =============================================================================
void test_sphincs_verify_fail(void) {
    int ret;

    print_string("\n[Verify Fail] Testing corrupted signature...\n");
    print_string("  Corrupting last byte of signature\n");

    // Corrupt the signature
    sm[smlen - 1] ^= 1;

    ret = crypto_sign_open(mout, &mlen, sm, smlen, pk);

    TEST_ASSERT_NOT_EQUAL(0, ret);
    print_string("  Result: SUCCESS (correctly rejected)\n");

    // Restore
    sm[smlen - 1] ^= 1;
}

// =============================================================================
// FORS Test
// =============================================================================
void test_fors(void) {
    BENCH_INIT()
    unsigned char pk1[SPX_FORS_PK_BYTES];
    unsigned char pk2[SPX_FORS_PK_BYTES];
    unsigned char sig[SPX_FORS_BYTES];
    unsigned char fors_m[SPX_FORS_MSG_BYTES];
    uint32_t addr[8] = {0};

    print_string("\n[FORS] Testing FORS sign/verify...\n");
    print_string("  FORS signature size: ");
    print_u32_int(SPX_FORS_BYTES);
    print_string(" bytes\n");
    print_string("  FORS message size:   ");
    print_u32_int(SPX_FORS_MSG_BYTES);
    print_string(" bytes\n");
    print_string("  FORS trees:          ");
    print_u32_int(SPX_FORS_TREES);
    print_string("\n");
    print_string("  FORS height:         ");
    print_u32_int(SPX_FORS_HEIGHT);
    print_string("\n");

    randombytes(ctx.sk_seed, SPX_N);
    randombytes(ctx.pub_seed, SPX_N);
    randombytes(fors_m, SPX_FORS_MSG_BYTES);
    randombytes((unsigned char *)addr, 8 * sizeof(uint32_t));

    initialize_hash_function(&ctx);

    BENCH_START()

    fors_sign(sig, pk1, fors_m, &ctx, addr);

    BENCH_END(SPX_FORS_SIGN)

    fors_pk_from_sig(pk2, sig, fors_m, &ctx, addr);

    TEST_ASSERT_EQUAL_MEMORY(pk1, pk2, SPX_FORS_PK_BYTES);
    print_string("  Result: SUCCESS (pk matches)\n");
}

// =============================================================================
// Thash Benchmark (runs 100 iterations for more accurate timing)
// =============================================================================
void test_thash_bench(void) {
    unsigned char block[SPX_N];
    unsigned char addr[SPX_ADDR_BYTES];

    print_string("\n[Thash] Benchmarking thash (100 iterations)...\n");
    print_string("  Block size: ");
    print_u32_int(SPX_N);
    print_string(" bytes\n");

    randombytes(block, SPX_N);
    randombytes(addr, SPX_ADDR_BYTES);

    initialize_hash_function(&ctx);

    timer_start();

    for (int i = 0; i < 100; i++) {
        thash(block, block, 1, &ctx, (uint32_t*)addr);
    }

    unsigned int elapsed = timer_read();
    print_string("SPX_THASH_100X:\t");
    print_u32_int(elapsed / 100);
    print_string(" cycles\n");

    TEST_PASS();
}

// =============================================================================
// WOTS pk gen benchmark
// =============================================================================
void test_wots_pk_bench(void) {
    BENCH_INIT()
    unsigned char wots_pk[SPX_WOTS_PK_BYTES];
    uint32_t addr[8] = {0};

    print_string("\n[WOTS] Benchmarking WOTS pk generation...\n");
    print_string("  WOTS pk size:  ");
    print_u32_int(SPX_WOTS_PK_BYTES);
    print_string(" bytes\n");
    print_string("  WOTS length:   ");
    print_u32_int(SPX_WOTS_LEN);
    print_string("\n");
    print_string("  Winternitz w:  ");
    print_u32_int(SPX_WOTS_W);
    print_string("\n");

    initialize_hash_function(&ctx);

    BENCH_START()

    wots_gen_pkx1(wots_pk, &ctx, addr);

    BENCH_END(SPX_WOTS_PKGEN)

    TEST_PASS();
}


// =============================================================================
// Main
// =============================================================================
int main(void) {
    UnityBegin("main.c");

    print_string("\n");
    print_string("###############################################\n");
    print_string("#     SPHINCS+ Hardware Implementation        #\n");
    print_string("#        with Keccak Masking Support          #\n");
    print_string("#          Unity Test Suite                   #\n");
    print_string("###############################################\n");

    // Print all parameters first
    print_parameters();

    print_string("--- Starting Unmasked Tests ---\n");

    // Core signature scheme tests (unmasked)
    RUN_TEST(test_sphincs_keypair);
    RUN_TEST(test_sphincs_sign);
    RUN_TEST(test_sphincs_verify);
    RUN_TEST(test_sphincs_verify_fail);

    // Component tests
    RUN_TEST(test_fors);

    // Benchmark tests
    RUN_TEST(test_thash_bench);
    RUN_TEST(test_wots_pk_bench);

    print_string("\n--- All Tests Complete ---\n");

    return(UnityEnd());
}
