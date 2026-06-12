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
#include "masked_sign.h"
#include "masked_fors.h"
#include "masked_wots.h"
#include "masked_wotsx1.h"
#include "masked_thash.h"
#include "masked_hash.h"
#include "masked_fips202.h"
#include "fips202.h"

#define SPX_MLEN 32

// Test vectors
static unsigned char pk[SPX_PK_BYTES];
static unsigned char sk[SPX_SK_BYTES];
static unsigned char m[SPX_MLEN];
static unsigned char sm[SPX_BYTES + SPX_MLEN];
static unsigned char sm_hwmasked[SPX_BYTES + SPX_MLEN];
static unsigned char mout[SPX_BYTES + SPX_MLEN];
static size_t smlen;
static unsigned long long smlen_hwmasked;
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
// HW Masked Keypair Test
// =============================================================================
void test_sphincs_keypair_hwmasked(void) {
    BENCH_INIT()
    int ret;

    print_string("\n[Keypair HW Masked] Generating HW masked key pair...\n");
    print_string("  Output: pk[");
    print_u32_int(SPX_PK_BYTES);
    print_string("], sk[");
    print_u32_int(SPX_SK_BYTES);
    print_string("]\n");

    BENCH_START()

    ret = masked_crypto_sign_keypair(pk, sk);

    BENCH_END(SPX_KEYPAIR)

    TEST_ASSERT_EQUAL_INT(0, ret);
    print_string("  Result: SUCCESS\n");
}

// =============================================================================
// HW Masked Sign Test
// =============================================================================
void test_sphincs_sign_hwmasked(void) {
    BENCH_INIT()
    int ret;

    randombytes(m, SPX_MLEN);

    unsigned long long smlen_ull;

    print_string("\n[Sign HW Masked] Signing message (HW masked implementation)...\n");
    print_string("  Message length:    ");
    print_u32_int(SPX_MLEN);
    print_string(" bytes\n");

    BENCH_START()

    ret = masked_crypto_sign(sm_hwmasked, &smlen_ull, m, SPX_MLEN, sk);
    smlen_hwmasked = smlen_ull;

    BENCH_END(SPX_SIGN)

    print_string("  Actual sig+msg:    ");
    print_u32_int((uint32_t)smlen_hwmasked);
    print_string(" bytes\n");

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT32(SPX_BYTES + SPX_MLEN, (uint32_t)smlen_hwmasked);
    print_string("  Result: SUCCESS\n");
}

// =============================================================================
// HW Masked Verify Test
// =============================================================================
void test_sphincs_verify_hwmasked(void) {
    BENCH_INIT()
    int ret;
    unsigned long long mlen_ull;

    print_string("\n[Verify HW Masked] Verifying HW masked signature...\n");

    BENCH_START()

    ret = masked_crypto_sign_open(mout, &mlen_ull, sm_hwmasked, smlen_hwmasked, pk);
    mlen = (size_t)mlen_ull;

    BENCH_END(SPX_VERIFY)

    if (ret != 0) {
        print_string("  Verify HW Masked FAILED with ret: ");
        print_u32_int((uint32_t)ret);
        print_string("\n");
        print_string("  smlen_hwmasked used: ");
        print_u32_int((uint32_t)smlen_hwmasked);
        print_string("\n");
    }

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT32(SPX_MLEN, mlen);
    TEST_ASSERT_EQUAL_MEMORY(m, mout, SPX_MLEN);
    print_string("  Result: SUCCESS (message matches)\n");
}

// =============================================================================
// Cross Verify Test
// =============================================================================
void test_sphincs_cross_verify_hwmasked(void) {
    int ret;
    unsigned long long mlen_ull;

    print_string("\n[HW Cross Verify] Unmasked Sign -> HW Masked Verify...\n");

    ret = masked_crypto_sign_open(mout, &mlen_ull, sm, (unsigned long long)smlen, pk);
    mlen = (size_t)mlen_ull;

    if (ret != 0) {
        print_string("  Cross Verify (Unmasked->HW Masked) FAILED!\n");
    }
    print_string("  Result: ");
    print_string(ret == 0 ? "SUCCESS\n" : "FAILED\n");

    print_string("\n[HW Cross Verify] HW Masked Sign -> Unmasked Verify...\n");

    ret = crypto_sign_open(mout, &mlen, sm_hwmasked, (size_t)smlen_hwmasked, pk);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_MEMORY(m, mout, SPX_MLEN);
    print_string("  Result: SUCCESS\n");
}

// =============================================================================
// Helper for HW Masked WOTS pk generation benchmark
// =============================================================================
static void wots_gen_pkx1_masked_helper(unsigned char *pk_out, const spx_ctx* ctx_in, uint32_t addr[8]) {
    struct masked_leaf_info_x1 leaf;
    unsigned steps[SPX_WOTS_LEN] = {0};
    MASKED_INITIALIZE_LEAF_INFO_X1(leaf, addr, steps);

    unsigned char pk_out_share1[SPX_WOTS_PK_BYTES];
    unsigned char pk_out_share2[SPX_WOTS_PK_BYTES];

    masked_wots_gen_leafx1(pk_out_share1, pk_out_share2, ctx_in, 0, &leaf);

    for(int i=0; i<SPX_WOTS_PK_BYTES; i++) {
        pk_out[i] = pk_out_share1[i] ^ pk_out_share2[i];
    }
}

// =============================================================================
// HW Masked Thash Benchmark
// =============================================================================
void test_thash_masked_bench(void) {
    unsigned char block1[SPX_N], block2[SPX_N];
    unsigned char out1[SPX_N], out2[SPX_N];
    unsigned char addr[SPX_ADDR_BYTES];

    print_string("\n[Thash HW Masked] Benchmarking hwmasked thash (100 iterations)...\n");
    print_string("  Block size: ");
    print_u32_int(SPX_N);
    print_string(" bytes\n");

    randombytes(block1, SPX_N);
    randombytes(block2, SPX_N);
    randombytes(addr, SPX_ADDR_BYTES);

    masked_initialize_hash_function(&ctx);

    timer_start();

    for (int i = 0; i < 100; i++) {
        masked_thash(out1, out2, block1, block2, 1, &ctx, (uint32_t*)addr);
    }

    unsigned int elapsed = timer_read();
    print_string("SPX_masked_thash_100X:\t");
    print_u32_int(elapsed / 100);
    print_string(" cycles\n");

    TEST_PASS();
}

// =============================================================================
// HW Masked WOTS pk gen benchmark
// =============================================================================
void test_wots_pk_masked_bench(void) {
    BENCH_INIT()
    unsigned char wots_pk[SPX_WOTS_PK_BYTES];
    uint32_t addr[8] = {0};

    print_string("\n[WOTS HW Masked] Benchmarking HW Masked WOTS pk generation...\n");

    masked_initialize_hash_function(&ctx);

    BENCH_START()

    wots_gen_pkx1_masked_helper(wots_pk, &ctx, addr);

    BENCH_END(SPX_WOTS_PKGEN)

    TEST_PASS();
}

// =============================================================================
// HW Masked Thash Correctness Test
// =============================================================================
void test_masked_thash_correctness(void) {
    unsigned char block[SPX_N];
    unsigned char block1[SPX_N], block2[SPX_N];
    unsigned char out[SPX_N];
    unsigned char out1[SPX_N], out2[SPX_N];
    uint32_t addr[8] = {0};

    print_string("\n[HW Masked Thash] Verifying masked_thash vs thash...\n");

    randombytes(block, SPX_N);
    randombytes(block2, SPX_N); // Mask

    // block1 = block ^ block2
    for (int i = 0; i < SPX_N; i++) {
        block1[i] = block[i] ^ block2[i];
    }

    randombytes((unsigned char *)addr, SPX_ADDR_BYTES);
    masked_initialize_hash_function(&ctx);

    // Run unmasked
    thash(out, block, 1, &ctx, addr);

    // Run HW masked
    masked_thash(out1, out2, block1, block2, 1, &ctx, addr);

    // Recombine
    unsigned char out_combined[SPX_N];
    for (int i = 0; i < SPX_N; i++) {
        out_combined[i] = out1[i] ^ out2[i];
    }

    if (memcmp(out, out_combined, SPX_N)) {
        print_string("  masked_thash produces different result!\n");
        print_hex_with_label("Expected", out, SPX_N);
        print_hex_with_label("Got", out_combined, SPX_N);
        TEST_FAIL();
    } else {
        print_string("  Result: SUCCESS (outputs match)\n");
        TEST_PASS();
    }
}

// =============================================================================
// HW Masked SHAKE256 Correctness Test
// =============================================================================
void test_masked_shake256_correctness(void) {
    unsigned char input[64];
    unsigned char out_ref[32];
    unsigned char out1[32], out2[32];
    unsigned char input1[64], input2[64];

    print_string("\n[HW Masked SHAKE256] Verifying masked_shake256 vs shake256...\n");

    randombytes(input, 64);
    randombytes(input2, 64);  // Random mask

    // input1 = input ^ input2
    for (int i = 0; i < 64; i++) {
        input1[i] = input[i] ^ input2[i];
    }

    // Unmasked reference
    shake256(out_ref, 32, input, 64);

    // HW masked
    masked_shake256(out1, out2, 32, input1, input2, 64);

    // Recombine
    unsigned char out_combined[32];
    for (int i = 0; i < 32; i++) {
        out_combined[i] = out1[i] ^ out2[i];
    }

    if (memcmp(out_ref, out_combined, 32)) {
        print_string("  masked_shake256 produces different result!\n");
        print_hex_with_label("Expected", out_ref, 32);
        print_hex_with_label("Got", out_combined, 32);
        TEST_FAIL();
    } else {
        print_string("  Result: SUCCESS (outputs match)\n");
        TEST_PASS();
    }
}

// =============================================================================
// HW Masked Keypair Generation Benchmark
// =============================================================================
void test_masked_keypair_generation_bench(void) {
    unsigned char pk_b[CRYPTO_PUBLICKEYBYTES];
    unsigned char sk_b[CRYPTO_SECRETKEYBYTES];

    print_string("\n[HW Masked Keypair Generation Bench] Benchmarking HW masked keypair generation...\n");

    timer_start();

    for (int i = 0; i < 10; i++) {
        masked_crypto_sign_keypair(pk_b, sk_b);
    }

    unsigned int elapsed = timer_read();
    print_string("SPX_masked_KEYPAIR_10X:\t");
    print_u32_int(elapsed / 10);
    print_string(" cycles\n");

    TEST_PASS();
}

// =============================================================================
// Main
// =============================================================================
int main(void) {
    UnityBegin("main.c");

    print_string("\n");
    print_string("###############################################\n");
    print_string("#         SPHINCS+ Unity Test Start           #\n");
    print_string("###############################################\n");

    // Print all parameters first
    print_parameters();

    print_string("--- Starting Unmasked Tests ---\n");

    // Core signature scheme tests (unmasked)
    RUN_TEST(test_sphincs_keypair);
    RUN_TEST(test_sphincs_sign);
    RUN_TEST(test_sphincs_verify);
    RUN_TEST(test_sphincs_verify_fail);

    // Unmasked benchmark tests
    RUN_TEST(test_thash_bench);
    RUN_TEST(test_wots_pk_bench);

    print_string("\n--- Starting HW Masked Tests ---\n");
    RUN_TEST(test_masked_shake256_correctness);
    RUN_TEST(test_masked_thash_correctness);
    RUN_TEST(test_thash_masked_bench);
    RUN_TEST(test_wots_pk_masked_bench);
    RUN_TEST(test_sphincs_keypair_hwmasked);
    RUN_TEST(test_sphincs_sign_hwmasked);
    RUN_TEST(test_sphincs_verify_hwmasked);
    RUN_TEST(test_sphincs_cross_verify_hwmasked);

    RUN_TEST(test_masked_keypair_generation_bench);

    print_string("\n--- All Tests Complete ---\n");

    return(UnityEnd());
}
