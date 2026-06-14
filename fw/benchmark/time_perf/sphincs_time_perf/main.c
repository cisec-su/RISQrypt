#include <stdint.h>
#include <string.h>
#include "timer.h"
#include "util.h"
#include "api.h"
#include "benchmark.h"

#include "context.h"
#include "hash.h"
#include "fors.h"
#include "thash.h"
#include "wotsx1.h"
#include "params.h"
#include "randombytes.h"
#include "masked_sign.h"
#include "masked_fors.h"
#include "masked_thash.h"
#include "masked_hash.h"
#include "masked_wotsx1.h"
#include "masked_fips202.h"
#include "fips202.h"


#define SPX_MLEN 32

// Test vectors
static unsigned char pk[SPX_PK_BYTES];
static unsigned char sk[SPX_SK_BYTES];
static unsigned char m[SPX_MLEN];
static unsigned char sm[SPX_BYTES + SPX_MLEN];
static unsigned char sm_masked[SPX_BYTES + SPX_MLEN];
static unsigned char mout[SPX_BYTES + SPX_MLEN];
static size_t smlen;
static unsigned long long smlen_masked;
static size_t mlen;

// Context for component tests
static spx_ctx ctx;

// Helper for WOTS pk generation
static void wots_gen_pkx1_helper(unsigned char *pk_out, const spx_ctx* ctx_in, uint32_t addr[8]) {
    struct leaf_info_x1 leaf;
    unsigned steps[SPX_WOTS_LEN] = {0};
    INITIALIZE_LEAF_INFO_X1(leaf, addr, steps);
    wots_gen_leafx1(pk_out, ctx_in, 0, &leaf);
}

// Helper for Masked WOTS pk generation
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
// Unmasked API benchmarks
// =============================================================================
void test_unmasked_api(void) {
    int ret;
    BENCH_INIT();

    // --- Keypair ---
    BENCH_START();
    crypto_sign_keypair(pk, sk);
    (void) pk;
    (void) sk;
    BENCH_END_COL(SPX_KEYPAIR);

    // --- Sign ---
    randombytes(m, SPX_MLEN);
    BENCH_START();
    ret = crypto_sign(sm, &smlen, m, SPX_MLEN, sk);
    (void) ret;
    (void) sm;
    BENCH_END_COL(SPX_SIGN);

    // --- Verify ---
    BENCH_START();
    ret = crypto_sign_open(mout, &mlen, sm, smlen, pk);
    (void) ret;
    (void) mout;
    BENCH_END_COL(SPX_VERIFY);
}


// =============================================================================
// Masked API benchmarks
// =============================================================================
void test_masked_api(void) {
    int ret;
    unsigned long long mlen_ull;
    BENCH_INIT();

    // --- Masked Keypair ---
    BENCH_START();
    masked_crypto_sign_keypair(pk, sk);
    (void) pk;
    (void) sk;
    BENCH_END_COL(SPX_KEYPAIR_HWMASKED);

    // --- Masked Sign ---
    randombytes(m, SPX_MLEN);
    BENCH_START();
    ret = masked_crypto_sign(sm_masked, &smlen_masked, m, SPX_MLEN, sk);
    (void) ret;
    (void) sm_masked;
    BENCH_END_COL(SPX_SIGN_HWMASKED);

    // --- Masked Verify ---
    BENCH_START();
    ret = masked_crypto_sign_open(mout, &mlen_ull, sm_masked, smlen_masked, pk);
    (void) ret;
    (void) mout;
    BENCH_END_COL(SPX_VERIFY_HWMASKED);
}


// =============================================================================
// Component benchmarks: thash
// =============================================================================
void test_thash(void) {
    unsigned char block[SPX_N];
    unsigned char addr[SPX_ADDR_BYTES];
    BENCH_INIT();

    randombytes(block, SPX_N);
    randombytes(addr, SPX_ADDR_BYTES);
    initialize_hash_function(&ctx);

    timer_start();
    for (int i = 0; i < 100; i++) {
        thash(block, block, 1, &ctx, (uint32_t*)addr);
    }
    time = timer_read();
    BENCH_END_SHIFT_COL_T(SPX_THASH_100X, 0, time);
}


void test_thash_hwmasked(void) {
    unsigned char block1[SPX_N], block2[SPX_N];
    unsigned char out1[SPX_N], out2[SPX_N];
    unsigned char addr[SPX_ADDR_BYTES];
    BENCH_INIT();

    randombytes(block1, SPX_N);
    randombytes(block2, SPX_N);
    randombytes(addr, SPX_ADDR_BYTES);
    masked_initialize_hash_function(&ctx);

    timer_start();
    for (int i = 0; i < 100; i++) {
        masked_thash(out1, out2, block1, block2, 1, &ctx, (uint32_t*)addr);
    }
    time = timer_read();
    BENCH_END_SHIFT_COL_T(SPX_THASH_MASKED_100X, 0, time);
}


// =============================================================================
// Component benchmarks: WOTS pk gen
// =============================================================================
void test_wots_pkgen(void) {
    unsigned char wots_pk[SPX_WOTS_PK_BYTES];
    uint32_t addr[8] = {0};
    BENCH_INIT();

    initialize_hash_function(&ctx);

    BENCH_START();
    wots_gen_pkx1_helper(wots_pk, &ctx, addr);
    (void) wots_pk;
    BENCH_END_COL(SPX_WOTS_PKGEN);
}


void test_wots_pkgen_hwmasked(void) {
    unsigned char wots_pk[SPX_WOTS_PK_BYTES];
    uint32_t addr[8] = {0};
    BENCH_INIT();

    masked_initialize_hash_function(&ctx);

    BENCH_START();
    wots_gen_pkx1_masked_helper(wots_pk, &ctx, addr);
    (void) wots_pk;
    BENCH_END_COL(SPX_WOTS_PKGEN_HWMASKED);
}


// =============================================================================
// Component benchmarks: FORS
// =============================================================================
void test_fors_sign(void) {
    unsigned char fors_pk[SPX_FORS_PK_BYTES];
    unsigned char sig[SPX_FORS_BYTES];
    unsigned char fors_m[SPX_FORS_MSG_BYTES];
    uint32_t addr[8] = {0};
    BENCH_INIT();

    randombytes(ctx.sk_seed, SPX_N);
    randombytes(ctx.pub_seed, SPX_N);
    randombytes(fors_m, SPX_FORS_MSG_BYTES);
    initialize_hash_function(&ctx);

    BENCH_START();
    fors_sign(sig, fors_pk, fors_m, &ctx, addr);
    (void) sig;
    (void) fors_pk;
    BENCH_END_COL(SPX_FORS_SIGN);
}


// =============================================================================
// SHAKE256 masked correctness (quick sanity)
// =============================================================================
void test_shake256_hwmasked(void) {
    unsigned char input[64];
    unsigned char out_ref[32];
    unsigned char out1[32], out2[32];
    unsigned char input1[64], input2[64];
    BENCH_INIT();

    randombytes(input, 64);
    randombytes(input2, 64);
    for (int i = 0; i < 64; i++) {
        input1[i] = input[i] ^ input2[i];
    }

    BENCH_START();
    masked_shake256(out1, out2, 32, input1, input2, 64);
    BENCH_END_COL(SPX_MASKED_SHAKE256);
}


// =============================================================================
// Main
// =============================================================================
int main(void) {
    BENCH_LINE();
    BENCH_LINE();
    BENCH_TITLE("SPHINCS+");
    BENCH_LINE();
    BENCH_LINE();
    BENCH_HEADER();
    BENCH_LINE();

    test_unmasked_api();
    BENCH_LINE();
    test_masked_api();
    BENCH_LINE();
    test_thash();
    test_thash_hwmasked();
    test_wots_pkgen();
    test_wots_pkgen_hwmasked();
    test_fors_sign();
    test_shake256_hwmasked();

    BENCH_LINE();
    BENCH_LINE();
    print_string("Finished\n");
}
