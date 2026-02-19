#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "inner.h"

#include "util.h"
#include "symmetric.h"
#include "keccak.h"
#include "falcon.h"

#include "extracted_vectors.h"
#include "falcon_testvectors.h"

#include "ntt_lite.h"
#include "timer.h"
#include "poly.h"

/* ================================================================== */
/* CONSTANTS & MACROS                                                 */
/* ================================================================== */

/* General purpose large buffer for tests (5KB to be safe for all operations) */
#define WORK_BUFFER_SIZE (5 * 1024)

/* Allocate actual memory for operations */
static uint8_t pool_buffer[WORK_BUFFER_SIZE] __attribute__((aligned(4)));

/* Pointers used by tests */
uint8_t *tmpvv;
size_t tmpvv_len;
extern uint32_t psi_inv[256];
extern uint32_t psi[256];
extern uint32_t zetas[256];

uint32_t sig_buffer[256] __attribute__((aligned(4)));

void test_falcon_verify() {
    int result;
    unsigned int time;

    uint8_t *sig_ptr = ((uint8_t*) sig_buffer) + 3;

    print_string("\n=== Falcon-512 Verification ===\n");

    /* ----------------------------------------------------------------
     * TEST 1: COMPRESSED SIGNATURE
     * ---------------------------------------------------------------- */
    print_string("[TEST] Verify COMPRESSED... ");
    

    memcpy(sig_ptr, test_sig_compressed, test_sig_compressed_len);


    timer_start();

    result = falcon_verify(sig_ptr, test_sig_compressed_len, FALCON_SIG_COMPRESSED,
                           test_pubkey, sizeof(test_pubkey),
                           test_message, test_message_len,
                           tmpvv, tmpvv_len);
    
    time = timer_read();
    print_string("\nTime: ");
    print_u32_int(time);
    print_string(" cycles");
    print_string("\n");

    if (result == 0) {
        print_string("PASS COMPRESSED SIGNATURE");
        print_string("\nSignature Size: ");
        print_u32_int((uint32_t)test_sig_compressed_len);
        print_string(" bytes\n");
    } else {
        print_string("FAIL (Error: ");
        print_u32((uint32_t)result);
        print_string(")\n");
    }

    /* ----------------------------------------------------------------
     * TEST 2: PADDED SIGNATURE
     * ---------------------------------------------------------------- */
    print_string("[TEST] Verify PADDED...     ");

    memcpy(sig_ptr, test_sig_padded, test_sig_padded_len);

    timer_reset();
    timer_start();

    result = falcon_verify(sig_ptr, test_sig_padded_len, FALCON_SIG_PADDED,
                           test_pubkey, sizeof(test_pubkey),
                           test_message, test_message_len,
                           tmpvv, tmpvv_len);
    time = timer_read();
    print_string("\nTime: ");
    print_u32_int(time);
    print_string(" cycles");
    print_string("\n");
    
    if (result == 0) {
        print_string("PASS PADDED SIGNATURE");
        print_string("\nSignature Size: ");
        print_u32_int((uint32_t)test_sig_padded_len);
        print_string(" bytes\n");
    } else {
        print_string("FAIL (Error: ");
        print_u32((uint32_t)result);
        print_string(")\n");
    }

    /* ----------------------------------------------------------------
     * TEST 3: CT (CONSTANT-TIME) SIGNATURE
     * ---------------------------------------------------------------- */
    print_string("[TEST] Verify CT...         ");

    memcpy(sig_ptr, test_sig_ct, test_sig_ct_len);

    timer_reset();
    timer_start();

    result = falcon_verify(sig_ptr, test_sig_ct_len, FALCON_SIG_CT,
                           test_pubkey, sizeof(test_pubkey),
                           test_message, test_message_len,
                           tmpvv, tmpvv_len);
    
    time = timer_read();
    print_string("\nTime: ");
    print_u32_int(time);
    print_string(" cycles");
    print_string("\n");

    if (result == 0) {
        print_string("PASS CT SIGNATURE");
        print_string("\nSignature Size: ");
        print_u32_int((uint32_t)test_sig_ct_len);
        print_string(" bytes\n");
    } else if (result == FALCON_ERR_SIZE) {
        print_string("FAIL (FALCON_ERR_SIZE)\n");
    } else if (result == FALCON_ERR_FORMAT) {
        print_string("FAIL (FALCON_ERR_FORMAT)\n");
    } else if (result == FALCON_ERR_BADSIG) {
        print_string("FAIL (FALCON_ERR_BADSIG)\n");
    } else {
        print_string("FAIL (Error: ");
        print_u32((uint32_t)result);
        print_string(")\n");
    }
    
    print_string("\n=== Falcon-512 Verification END===\n");
}

/* ================================================================== */
/* HW NTT INTEGRATION TEST                                            */
/* ================================================================== */
void test_hw_ntt() {
    print_string("\n=== HW NTT Test ===\n");

    poly a;
    int pass;

    poly_init_q();

    for (int i = 0 ; i < N ; i++){
        a.coeffs[i] = i;
    }
    int32_t orig[N];
    for (int i = 0; i < N; i++) orig[i] = a.coeffs[i];

    print_string("  In[0..3]: ");
    print_u32_arr((uint32_t*)a.coeffs, 8);

    poly_init_ntt();
    ntt_lite_forward_ntt((uint32_t*)a.coeffs, (uint32_t*)a.coeffs);

    print_string("  NTT[0..3]: ");
    print_u32_arr((uint32_t*)a.coeffs, 8);

    poly_init_invntt();
    ntt_lite_backward_ntt((uint32_t*)a.coeffs, (uint32_t*)a.coeffs);

    print_string("  Out[0..3]: ");
    print_u32_arr((uint32_t*)a.coeffs, 8);
    print_string("  Exp[0..3]: ");
    print_u32_arr((uint32_t*)orig, 8);

    pass = 1;
    for (int i = 0; i < N; i++) {
        if (a.coeffs[i] != orig[i]) {
            print_string("  FAIL["); print_u32_int(i);
            print_string("] got="); print_u32(a.coeffs[i]);
            print_string(" exp="); print_u32(orig[i]);
            print_string("\n");
            pass = 0;
            if (i >= 3) break;
        }
    }
    if (pass) print_string("  PASS\n");

    /* ============================================================
     * Test 2: NTT -> basemul -> iNTT
     * ============================================================ */
    print_string("\n[NTT + basemul + iNTT]\n");
    poly b;
    poly r;
    /* Reset input */
    for (int i = 0; i < N; i++) a.coeffs[i] = 0;
    for (int i = 0; i < 2; i++) a.coeffs[i] = 1;

    /* basemul with identity (all 1s) */
    for (int i = 0; i < N; i++) b.coeffs[i] = 0;
    for (int i = 0; i < 2; i++) b.coeffs[i] = 1;
    poly_init_ntt();
    ntt_lite_forward_ntt((uint32_t*)a.coeffs, (uint32_t*)a.coeffs);
    ntt_lite_forward_ntt((uint32_t*)b.coeffs, (uint32_t*)b.coeffs);
    poly_basemul(&r, &a, &b);

    print_string("  basemul[0..3]: ");
    // for (int i = 0; i < 16; i++) { print_u32(r.coeffs[i]); print_string(" "); }
    print_u32_arr((uint32_t*)r.coeffs, 16);
    print_string("\n");

    /* Backward NTT */
    poly_init_invntt();
    ntt_lite_backward_ntt((uint32_t*)r.coeffs, (uint32_t*)r.coeffs);

    print_string("  Out[0..3]: ");
    // for (int i = 0; i < 4; i++) { print_u32(r.coeffs[i]); print_string(" "); }
    print_u32_arr((uint32_t*)r.coeffs, 16);
    print_string("\n");
    print_string("  Exp[0..3]: ");
    // for (int i = 0; i < 4; i++) { print_u32(orig[i]); print_string(" "); }
    print_u32_arr((uint32_t*)orig, 16);
    print_string("\n");

    pass = 1;
    for (int i = 0; i < N; i++) {
        if (r.coeffs[i] != orig[i]) {
            print_string("  FAIL["); print_u32_int(i);
            print_string("] got="); print_u32(r.coeffs[i]);
            print_string(" exp="); print_u32(orig[i]);
            print_string("\n");
            pass = 0;
            if (i >= 3) break;
        }
    }
    if (pass) print_string("  PASS\n");

    print_string("\n=== HW NTT Test END ===\n");
}

int main() {
    tmpvv = pool_buffer;
    tmpvv_len = WORK_BUFFER_SIZE;
    
    test_hw_ntt();
    test_falcon_verify();
    
    
    return 0;
}