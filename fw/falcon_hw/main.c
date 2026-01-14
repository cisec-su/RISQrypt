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

/* ================================================================== */
/* CONSTANTS & MACROS                                                 */
/* ================================================================== */

/* General purpose large buffer for tests (72KB to be safe for all operations) */
#define WORK_BUFFER_SIZE (72 * 1024)

/* Allocate actual memory for operations */
static uint8_t pool_buffer[WORK_BUFFER_SIZE];

/* Pointers used by tests */
uint8_t *tmpvv;
size_t tmpvv_len;

void test_falcon_verify() {
    int result;
    unsigned int time;

    print_string("\n=== Falcon-512 Verification NEW ===\n");

    /* ----------------------------------------------------------------
     * TEST 1: COMPRESSED SIGNATURE
     * ---------------------------------------------------------------- */
    print_string("[TEST] Verify COMPRESSED... ");
    
    timer_start();

    result = falcon_verify(test_sig_compressed, test_sig_compressed_len, FALCON_SIG_COMPRESSED,
                           test_pubkey, sizeof(test_pubkey),
                           test_message, test_message_len,
                           tmpvv, tmpvv_len);
    
    time = timer_read();
    print_string("\nTime: ");
    print_dec(time);
    print_string(" cycles");
    print_string("\n");

    if (result == 0) {
        print_string("PASS COMPRESSED SIGNATURE");
        print_string("\nSignature Size: ");
        print_dec((uint32_t)test_sig_compressed_len);
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
    timer_reset();
    timer_start();

    result = falcon_verify(test_sig_padded, test_sig_padded_len, FALCON_SIG_PADDED,
                           test_pubkey, sizeof(test_pubkey),
                           test_message, test_message_len,
                           tmpvv, tmpvv_len);
    time = timer_read();
    print_string("\nTime: ");
    print_dec(time);
    print_string(" cycles");
    print_string("\n");
    
    if (result == 0) {
        print_string("PASS PADDED SIGNATURE");
        print_string("\nSignature Size: ");
        print_dec((uint32_t)test_sig_padded_len);
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

    timer_reset();
    timer_start();

    result = falcon_verify(test_sig_ct, test_sig_ct_len, FALCON_SIG_CT,
                           test_pubkey, sizeof(test_pubkey),
                           test_message, test_message_len,
                           tmpvv, tmpvv_len);
    
    time = timer_read();
    print_string("\nTime: ");
    print_dec(time);
    print_string(" cycles");
    print_string("\n");

    if (result == 0) {
        print_string("PASS CT SIGNATURE");
        print_string("\nSignature Size: ");
        print_dec((uint32_t)test_sig_ct_len);
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

/* --- NTT Hardware Debug Helpers --- */
/* Unwraps the hardware output: (Val * 72) % 12289 */
void verify_scaled_output(uint32_t *hw_out, uint32_t *original, int count) {
    print_string("\n--- VERIFYING SCALED OUTPUT (Factor 72) ---\n");
    print_string("idx | Input (Lo/Hi) | HW Out (Lo/Hi)  | Decoded (Lo/Hi) | Status\n");
    print_string("----|---------------|-----------------|-----------------|-------\n");

    int i;
    for (i = 0; i < count; i++) {
        uint32_t raw = hw_out[i];
        uint32_t orig = original[i];

        /* 1. Unpack Hardware Output */
        uint32_t hw_lo = raw & 0xFFFF;
        uint32_t hw_hi = (raw >> 16) & 0xFFFF;

        /* 2. Unpack Original Input */
        uint32_t in_lo = orig & 0xFFFF;
        uint32_t in_hi = (orig >> 16) & 0xFFFF;

        /* 3. Apply Software Correction (Multiply by 72 mod Q) */
        /* This cancels out the *512 and *R form hardware   */
        uint32_t dec_lo = (hw_lo * 72) % 12289;
        uint32_t dec_hi = (hw_hi * 72) % 12289;

        /* 4. Print & Compare */
        print_dec(i); print_string("\t| ");
        
        /* Print Input */
        print_dec(in_lo); print_string("/"); print_dec(in_hi); print_string("\t| ");
        
        /* Print Raw HW */
        print_hex((uint8_t*)&raw, 4, 1); print_string("  | "); // Hex for debug
        
        /* Print Decoded */
        print_dec(dec_lo); print_string("/"); print_dec(dec_hi); print_string("\t| ");

        if (dec_lo == in_lo && dec_hi == in_hi) {
            print_string("OK\n");
        } else {
            print_string("FAIL\n");
        }
    }
}
#define FALCON_Q 12289

/* Software helper to verify dual-mode addition (checks 2x 16-bit lanes) */
uint32_t soft_add_check(uint32_t a, uint32_t b) {
    /* Unpack Lower 16-bits */
    uint32_t a0 = a & 0xFFFF;
    uint32_t b0 = b & 0xFFFF;
    uint32_t r0 = (a0 + b0) % FALCON_Q;

    /* Unpack Upper 16-bits */
    uint32_t a1 = (a >> 16) & 0xFFFF;
    uint32_t b1 = (b >> 16) & 0xFFFF;
    uint32_t r1 = (a1 + b1) % FALCON_Q;

    /* Repack */
    return r0 | (r1 << 16);
}
/* Prototypes for functions defined in vrfy.c */
void poly_init_q(void);
void poly_init_ntt(void);
void poly_init_invntt(void);

/* Simple Pseudo-Random Generator */
static uint32_t prng_state = 123456789;
uint32_t rand_u32(void) {
    prng_state = prng_state * 1103515245 + 12345;
    return prng_state;
}

uint32_t rand_falcon_pair(void) {
    uint16_t low = rand_u32() % FALCON_Q;
    uint16_t high = rand_u32() % FALCON_Q;
    return low | (high << 16);
}

void test_ntt_hw_basic(void) {
    print_string("      NTT HARDWARE DEBUGGER          \n");

    poly_init_q();      
    poly_init_ntt(); 
    
    #define N_WORDS 256
    uint32_t A[N_WORDS], B[N_WORDS], RES[N_WORDS], CHECK[N_WORDS];
    int i;

    /* 1. Setup Data: First 5 indices are SMALL numbers for easy reading */
    for(i=0; i<N_WORDS; i++) {
        if (i < 5) {
            /* Small numbers: 
               Low lane:  i + 1
               High lane: i + 5 
            */
            uint16_t low_a = i + 1;
            uint16_t high_a = i + 5;
            A[i] = low_a | (high_a << 16);

            uint16_t low_b = 2;       /* Add 2 to everything */
            uint16_t high_b = 10;     /* Add 10 to everything */
            B[i] = low_b | (high_b << 16);
        } else {
            /* Random noise for the rest */
            A[i] = rand_falcon_pair();
            B[i] = rand_falcon_pair();
        }
    }

    /* --- VISUAL TEST 1: ADDITION (Small Numbers) --- */
    print_string("\n[TEST] Addition (Small Numbers)\n");
    print_string("idx |   A (Hex)  |   B (Hex)  | HW Output  | Expected   | Status\n");
    print_string("----|------------|------------|------------|------------|-------\n");
    
    ntt_lite_add(RES, A, B);

    /* Check the first 4 small indices */
    for(int k=0; k<4; k++) {
        uint32_t sw_res = soft_add_check(A[k], B[k]);
        
        print_dec(k); print_string("\t| ");
        print_u32(A[k]); print_string(" | ");
        print_u32(B[k]); print_string(" | ");
        print_u32(RES[k]); print_string(" | ");
        print_u32(sw_res); print_string(" | ");
        
        if(RES[k] == sw_res) print_string("OK\n");
        else print_string("FAIL\n");
    }

    /* --- VISUAL TEST 2: SUBTRACTION --- */
    print_string("\n[TEST] Subtraction (A - B)\n");
    ntt_lite_sub(RES, A, B); 
    
    /* Check same small indices */
    for(int k=0; k<4; k++) {
        /* SW Calc inline */
        uint32_t a0 = A[k] & 0xFFFF; uint32_t b0 = B[k] & 0xFFFF;
        uint32_t a1 = A[k] >> 16;    uint32_t b1 = B[k] >> 16;
        
        uint32_t r0 = (a0 >= b0) ? (a0 - b0) : (a0 + FALCON_Q - b0);
        uint32_t r1 = (a1 >= b1) ? (a1 - b1) : (a1 + FALCON_Q - b1);
        uint32_t sw_res = r0 | (r1 << 16);

        print_dec(k); print_string("\t| ");
        print_u32(A[k]); print_string(" | ");
        print_u32(B[k]); print_string(" | ");
        print_u32(RES[k]); print_string(" | ");
        
        if(RES[k] == sw_res) print_string("OK\n");
        else print_string("FAIL\n");
    }

    /* --- VISUAL TEST 3: NTT DIAGNOSTIC --- */
    print_string("\n[TEST] NTT Pipeline Diagnostic\n");
    
    ntt_lite_forward_ntt(RES, A);
    
    print_string("1. Checking Forward NTT Output (Is it zero?)\n");
    int nonzero_count = 0;
    for(i=0; i<8; i++) {
        print_string("   NTT["); print_dec(i); print_string("]: ");
        print_u32(RES[i]); print_string("\n");
        if(RES[i] != 0) nonzero_count++;
    }
    
    if (nonzero_count == 0) {
        print_string("   [CRITICAL FAIL] Forward NTT output is ALL ZEROS.\n");
        return; 
    } else {
        print_string("   [OK] Forward NTT produced non-zero data.\n");
    }

    /* Step 2: Backward NTT */
    poly_init_invntt(); 
    ntt_lite_backward_ntt(CHECK, RES);
    verify_scaled_output(CHECK, A, 4);

    print_string("2. Checking Backward NTT Output (Round Trip)\n");
    print_string("idx | Original   | iNTT(NTT)  | Status\n");
    print_string("----|------------|------------|-------\n");
    
    for(i=0; i<4; i++) {
        print_dec(i); print_string("\t| ");
        print_u32(A[i]); print_string(" | ");
        print_u32(CHECK[i]); print_string(" | ");
        
        if(A[i] == CHECK[i]) print_string("EXACT\n");
        else if (CHECK[i] == 0) print_string("ZERO\n");
        else print_string("GARBAGE\n");
    }
}

int main() {
    tmpvv = pool_buffer;
    tmpvv_len = WORK_BUFFER_SIZE;
    
    //these tests worked well for falcon API
    /*test_keccak_simple();
    test_keccak_golden();
    test_falcon_api();*/

    //test_ntt_hw_basic();
    //keccak_newapi();

    test_falcon_verify();
    
    
    return 0;
}