#include <stdint.h>
#include <stdlib.h>
#include "timer.h"
#include "uart.h"
#include "util.h"
#include "api.h"
#include "benchmark.h"
#include "unity.h"

#include "hash.h"
#include "fors.h"
#include "thash.h"
#include "params.h"
#include "randombytes.h"
#include "swmasked_sign.h"
#include "swmasked_fors.h"
#include "swmasked_wots.h"
#include "swmasked_wotsx1.h"
#include "swmasked_thash.h"
#include "swmasked_hash.h"

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
// Main
// =============================================================================

void test_sphincs_keypair_masked(void) {
    BENCH_INIT()
    int ret;

    print_string("\n[Keypair Masked] Generating masked key pair...\n");
    print_string("  Output: pk[");
    print_u32_int(SPX_PK_BYTES);
    print_string("], sk[");
    print_u32_int(SPX_SK_BYTES);
    print_string("]\n");

    BENCH_START()

    ret = crypto_sign_keypair_masked(pk, sk);

    BENCH_END(SPX_KEYPAIR)

    TEST_ASSERT_EQUAL_INT(0, ret);
    print_string("  Result: SUCCESS\n");
}

// =============================================================================
// Masked Sign Test
// =============================================================================
void test_sphincs_sign_masked(void) {
    BENCH_INIT()
    int ret;

    randombytes(m, SPX_MLEN);

    unsigned long long smlen_ull;
    unsigned long long mlen_ull;

    print_string("\n[Sign Masked] Signing message (masked implementation)...\n");
    print_string("  Message length:    ");
    print_u32_int(SPX_MLEN);
    print_string(" bytes\n");

    BENCH_START()

    ret = crypto_sign_masked(sm_masked, &smlen_ull, m, SPX_MLEN, sk);
    smlen_masked = (size_t)smlen_ull;

    BENCH_END(SPX_SIGN) // Reusing ID for now, or define new one if needed

    print_string("  Actual sig+msg:    ");
    print_u32_int((uint32_t)smlen_masked);
    print_string(" bytes\n");

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT32(SPX_BYTES + SPX_MLEN, smlen_masked);
    print_string("  Result: SUCCESS\n");
}

// =============================================================================
// Masked Verify Test
// =============================================================================
void test_sphincs_verify_masked(void) {
    BENCH_INIT()
    int ret;
    unsigned long long mlen_ull; // Use ULL for masked API

    print_string("\n[Verify Masked] Verifying masked signature...\n");
    
    BENCH_START()

    ret = crypto_sign_open_masked(mout, &mlen_ull, sm_masked, (unsigned long long)smlen_masked, pk);
    mlen = (size_t)mlen_ull;

    BENCH_END(SPX_VERIFY)
    
    if (ret != 0) {
        print_string("  Verify Masked FAILED with ret: ");
        print_u32_int((uint32_t)ret);
        print_string("\n");
        print_string("  smlen_masked used: ");
        print_u32_int((uint32_t)smlen_masked);
        print_string("\n");
    }

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_UINT32(SPX_MLEN, mlen);
    TEST_ASSERT_EQUAL_MEMORY(m, mout, SPX_MLEN);
    print_string("  Result: SUCCESS (message matches)\n");
}

// =============================================================================
// Masked Thash Benchmark
// =============================================================================
void test_thash_swmasked_bench(void) {
    unsigned char block1[SPX_N], block2[SPX_N]; // Inputs
    unsigned char out1[SPX_N], out2[SPX_N];     // Outputs
    unsigned char addr[SPX_ADDR_BYTES];

    print_string("\n[Thash Masked] Benchmarking masked thash (100 iterations)...\n");
    print_string("  Block size: ");
    print_u32_int(SPX_N);
    print_string(" bytes\n");

    randombytes(block1, SPX_N);
    randombytes(block2, SPX_N); // Mask share
    randombytes(addr, SPX_ADDR_BYTES);

    initialize_hash_function_masked(&ctx);

    timer_start();
    
    for (int i = 0; i < 100; i++) {
        swmasked_thash(out1, out2, block1, block2, 1, &ctx, (uint32_t*)addr);
    }

    unsigned int elapsed = timer_read();
    print_string("SPX_swmasked_THASH_100X:\t");
    print_u32_int(elapsed / 100);
    print_string(" cycles\n");

    TEST_PASS();
}


// =============================================================================
// Masked Thash Correctness Test
// =============================================================================
void test_swmasked_thash_correctness(void) {
    unsigned char block[SPX_N];
    unsigned char block1[SPX_N], block2[SPX_N];
    unsigned char out[SPX_N];
    unsigned char out1[SPX_N], out2[SPX_N];
    uint32_t addr[8] = {0};

    print_string("\n[Thash Correctness] Verifying swmasked_thash vs thash...\n");

    randombytes(block, SPX_N);
    randombytes(block2, SPX_N); // Mask
    
    // block1 = block ^ block2
    for(int i=0; i<SPX_N; i++) {
        block1[i] = block[i] ^ block2[i];
    }

    randombytes((unsigned char *)addr, SPX_ADDR_BYTES);
    initialize_hash_function_masked(&ctx);

    // Run unmasked
    thash(out, block, 1, &ctx, addr);

    // Run masked
    swmasked_thash(out1, out2, block1, block2, 1, &ctx, addr);

    // Recombine
    unsigned char out_combined[SPX_N];
    for(int i=0; i<SPX_N; i++) {
        out_combined[i] = out1[i] ^ out2[i];
    }

    if (memcmp(out, out_combined, SPX_N)) {
        print_string("  swmasked_thash produces different result!\n");
        print_hex_with_label("Expected", out, SPX_N);
        print_hex_with_label("Got", out_combined, SPX_N);
        TEST_FAIL();
    } else {
        print_string("  Result: SUCCESS (outputs match)\n");
        TEST_PASS();
    }
}

// keypair generation benchmark for SW masked implementation
void test_masked_keypair_generation_bench(void) {
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];

    print_string("\n[Masked Keypair Generation Bench] Benchmarking masked keypair generation (100 iterations)...\n");

    timer_start();

    for (int i = 0; i < 1; i++) {
        crypto_sign_keypair_masked(pk, sk);
    }

    unsigned int elapsed = timer_read();
    print_string("SPX_masked_KEYPAIR_1X:\t");
    print_u32_int(elapsed / 1);
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
    print_string("#     SPHINCS+ Hardware Implementation        #\n");
    print_string("#        with Keccak Masking Support          #\n");
    print_string("#          Unity Test Suite                   #\n");
    print_string("###############################################\n");

    // Print all parameters first
    print_parameters();

    print_string("\n--- Starting Masked Benchmarks ---\n");
    RUN_TEST(test_thash_swmasked_bench);

    print_string("\n--- Starting Masked Tests ---\n");
    RUN_TEST(test_swmasked_thash_correctness);
    RUN_TEST(test_sphincs_keypair_masked);
    RUN_TEST(test_sphincs_sign_masked);
    RUN_TEST(test_sphincs_verify_masked);

    // Component tests
    //RUN_TEST(test_fors); // This is unmasked FORS


    RUN_TEST(test_masked_keypair_generation_bench);



    print_string("\n--- All Tests Complete ---\n");

    return(UnityEnd());
}
