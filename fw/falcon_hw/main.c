#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "inner.h"

#include "util.h"
#include "symmetric.h"
#include "keccak.h"
#include "keccak_tests.h"
#include "falcon.h"

#include "extracted_vectors.h"
#include "falcon_testvectors.h"

#include "ntt_lite.h"
#include "timer.h"
#include "poly.h"

/* ================================================================== */
/* CONSTANTS & MACROS                                                 */
/* ================================================================== */

/* General purpose large buffer for tests (72KB to be safe for all operations) */
#define WORK_BUFFER_SIZE (72 * 1024)
#define FALCON_Q 12289

/* Allocate actual memory for operations */
static uint8_t pool_buffer[WORK_BUFFER_SIZE] __attribute__((aligned(4)));

/* Pointers used by tests */
uint8_t *tmpvv;
size_t tmpvv_len;


uint32_t sig_buffer[256] __attribute__((aligned(4)));

void test_falcon_verify() {
    int result;
    unsigned int time;
    /* sig_ptr = sig_buffer + 3 bytes, so sig_ptr + 1 = sig_buffer + 4 = aligned!
     * This is critical: falcon_verify_start does shake256_inject(sig + 1, 40)
     * which passes the address to HW Keccak DMA. DMA requires 4-byte alignment. */
    uint8_t *sig_ptr = ((uint8_t*) sig_buffer) + 3;

    print_string("\n=== Falcon-512 Verification ===\n");

    /* ----------------------------------------------------------------
     * TEST 1: COMPRESSED SIGNATURE adresleri kontrol et aligned mi değil mi ? 32 bite align olmalı adresler 4ün katı bizim accelaratorlerde 4un katı olmak zorunda __attribute__((aligned(4))) kullan 32 bit array tanımla cast et
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
     * Copy into sig_buffer+3 so that sig+1 (nonce) is 4-byte aligned
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
     * Copy into sig_buffer+3 so that sig+1 (nonce) is 4-byte aligned
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

int main() {
    tmpvv = pool_buffer;
    tmpvv_len = WORK_BUFFER_SIZE;
    
    //these tests worked well for falcon API
    // test_keccak_simple();
    // test_keccak_golden();
    // test_falcon_api();

    //keccak_newapi();

    //test_keccak_simple();
    test_falcon_verify();
    
    
    return 0;
}