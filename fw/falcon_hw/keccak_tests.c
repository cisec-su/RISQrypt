/*basic keccak tests for hw*/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "inner.h"

#include "util.h"
#include "symmetric.h"
#include "keccak.h"
#include "falcon.h"


/* SHAKE256("abc", 32) -> 48336660... */
static const uint8_t golden_abcdefgh_dil[32] = {
    0x3a,0x3b,0x28,0xb6,0x96,0x22,0xde,0x01,0x55,
    0xfd,0xd9,0xdf,0xbd,0x6f,0xcb,0xca,0xfc,0x87,
    0x87,0x45,0xe9,0xe5,0x5c,0x3d,0x27,0x36,0xc0,
    0xc9,0xf5,0x08,0x23,0xc1
};

/* SHAKE256("abcd", 32) -> 16c60651... */
static const uint8_t golden_abcd_dil[32] = {
    0x16, 0xc6, 0x06 ,0x51 ,0xe6 ,0x44 ,0x8e ,0xb9 
    ,0xc1 ,0x77 ,0x23 ,0x4f ,0xdd ,0x73 ,0xce ,0x60 
    ,0xcb ,0xfe ,0x6d ,0x80 ,0x5c ,0x0e ,0x8f ,0x4c 
    ,0x95 ,0x69 ,0x86 ,0x37 ,0x6b ,0xe2 ,0x86 ,0xd6
};


void test_keccak_simple() {
    shake256_context sc;
    uint8_t out[16];
    
    print_string("\n--- Keccak Check ---\n");
    
    memset(&sc, 0, sizeof(sc));
    
    shake256_init_prng_from_seed(&sc, "abcd", 4);
    
    shake256_extract(&sc, out, 16);
    
    uint32_t val = (out[0] << 24) | (out[1] << 16) | (out[2] << 8) | out[3];
    
    print_string("First 4 bytes (Expect 0x16c60651): 0x");
    print_u32(val); 
    print_string("\n");
}


void test_keccak_golden() {
    shake256_context sc;
    uint8_t out[32];
    int i;
    
    print_string("\n=== Keccak FULL DEBUG Check ===\n");

    /* TEST 1: "abcd" (4 bytes) */
    print_string("\n[TEST] 'abcd' (4 bytes)\n");
    shake256_init(&sc);
    shake256_inject(&sc, "abcd", 4);
    shake256_flip(&sc);
    shake256_extract(&sc, out, 32);
    
    print_string(" Expect: ");
    print_hex(golden_abcd_dil, 32, 0);
    print_string(" Got:    ");
    print_hex(out, 32, 0);

    if (memcmp(out, golden_abcd_dil, 32) == 0) print_string(" RESULT: PASS\n");
    else print_string(" RESULT: FAIL\n");

    /* TEST 2: "abcd" (4 bytes) */
    print_string("\n[TEST] 'abcdefgh' (8 bytes)\n");
    shake256_init(&sc);
    shake256_inject(&sc, "abcdefgh", 8);
    shake256_flip(&sc);
    shake256_extract(&sc, out, 32);
    
    print_string(" Expect: ");
    print_hex(golden_abcdefgh_dil, 32, 0);
    print_string(" Got:    ");
    print_hex(out, 32, 0);

    if (memcmp(out, golden_abcdefgh_dil, 32) == 0) print_string(" RESULT: PASS\n");
    else print_string(" RESULT: FAIL\n");

    
    print_string("==================================\n");
}




/* ================================================================== */
/* TEST FUNCTION                                                      */
/* ================================================================== */

void test_falcon_api() {
    uint8_t out[32];

    print_string("\n=== Test via Symmetric.h API (Falcon) ===\n");

    /* ----------------------------------------------------------------
     * TEST 1: "abcd" (4 bytes)
     * ---------------------------------------------------------------- */
    print_string("\n[TEST] falcon_shake256('abcd')...\n");
    
    falcon_shake256(out, 32, (const uint8_t*)"abcd", 4);
    
    print_string(" Expect: ");
    print_hex(golden_abcd_dil, 32, 0);
    print_string(" Got:    ");
    print_hex(out, 32, 0);

    if (memcmp(out, golden_abcd_dil, 32) == 0) {
        print_string(" RESULT: PASS\n");
    } else {
        print_string(" RESULT: FAIL\n");
    }

    /* ----------------------------------------------------------------
     * TEST 2: "abc" (3 bytes)
     * Unaligned. Falcon typically ignores partial words (3 >> 2 = 0).
     * This confirms if Falcon API fails for unaligned data.
     * ---------------------------------------------------------------- */
    print_string("\n[TEST] falcon_shake256('abcdefgh')....\n");
    
    falcon_shake256(out, 32, (const uint8_t*)"abcdefgh", 8);
    
    print_string(" Expect: ");
    print_hex(golden_abcdefgh_dil, 32, 0);
    print_string(" Got:    ");
    print_hex(out, 32, 0);

    if (memcmp(out, golden_abcdefgh_dil, 32) == 0) {
        print_string(" RESULT: PASS\n");
    } else {
        print_string(" RESULT: FAIL (Expected behavior for Falcon unaligned)\n");
    }
}
