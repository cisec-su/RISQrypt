#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "inner.h"
#include "uart.h"
#include "util.h"
#include "falcon_testvectors.h"
#include "stdbool.h"
#include "ntt_lite.h"

// Falcon-512 parameters
#define CRYPTO_SECRETKEYBYTES   1281
#define CRYPTO_PUBLICKEYBYTES   897
#define CRYPTO_BYTES            690
#define LOGN                    9
#define N                       (1 << LOGN)
#define Q                       12289

// Test message:
static const uint8_t test_msg[] = "abc";

// Global buffer for large temporary allocations - with proper alignment
static uint8_t tmp_buffer[4 * N * sizeof(uint16_t)] __attribute__((aligned(16)));

void test_to_ntt_monty() {
    uint16_t poly[N];
    memcpy(poly, ntt_input_h, sizeof(poly)); 
    
    print_string("[TEST] to_ntt_monty... ");
    Zf(to_ntt_monty)(poly, LOGN);
    
    // Compare with expected output
    if (memcmp(poly, ntt_output_h, sizeof(poly)) == 0) {
        print_string("PASS\n");
    } else {
        print_string("FAIL (output mismatch)\n");
        
    
        print_string("First 5 element comparison:\n");
        for (int i = 0; i < 5; i++) {
            print_string("[");
            print_u32(i);
            print_string("] computed=");
            print_u32(poly[i]);
            print_string(", expected=");
            print_u32(ntt_output_h[i]);
            if (poly[i] == ntt_output_h[i]) {
                print_string(" ok\n");
            } else {
                print_string(" fail\n");
            }
        }
        
       
        int error_count = 0;
        for (size_t i = 0; i < N; i++) {
            if (poly[i] != ntt_output_h[i]) {
                error_count++;
            }
        }
        
        print_string("Total errors: ");
        print_u32(error_count);
        print_string("/");
        print_u32(N);
        print_string("\n");
        
        // İlk 5 hatayı göster
        if (error_count > 0) {
            print_string("First 5 errors:\n");
            int shown = 0;
            for (size_t i = 0; i < N && shown < 5; i++) {
                if (poly[i] != ntt_output_h[i]) {
                    print_string("[");
                    print_u32(i);
                    print_string("] computed=");
                    print_u32(poly[i]);
                    print_string(", expected=");
                    print_u32(ntt_output_h[i]);
                    print_string(" (diff=");
                    if (poly[i] > ntt_output_h[i]) {
                        print_u32(poly[i] - ntt_output_h[i]);
                    } else {
                        print_u32(ntt_output_h[i] - poly[i]);
                    }
                    print_string(")\n");
                    shown++;
                }
            }
        }
    }
}

void test_compute_public() {
    uint16_t h[N];
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    int result;
    print_string("[TEST] compute_public... ");
    
    result = Zf(compute_public)(h, compute_public_f, compute_public_g, LOGN, tmp_buffer);
    if (result) {
        if (memcmp(h, compute_public_h2, sizeof(h)) == 0) {
            print_string("PASS\n");
        } else {
            print_string("FAIL (output mismatch)\n");
            
            // Show first few mismatches
            int mismatches = 0;
            for (size_t i = 0; i < N && mismatches < 5; i++) {
                if (h[i] != compute_public_h2[i]) {
                    print_string("Mismatch[");
                    print_u32(i);
                    print_string("]: computed=");
                    print_u32(h[i]);
                    print_string(", expected=");
                    print_u32(compute_public_h2[i]);
                    print_string("\n");
                    mismatches++;
                }
            }
        }
    } else {
        print_string("FAIL (f not invertible)\n");
    }
}

void test_complete_private() {
    int8_t G[N];
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    int result;
    print_string("[TEST] complete_private... ");

    result = Zf(complete_private)(G, complete_private_f, complete_private_g, complete_private_F, LOGN, tmp_buffer);
    if (result) {
        if (memcmp(G, complete_private_G, sizeof(G)) == 0) {
            print_string("PASS\n");
        } else {
            print_string("FAIL (output mismatch)\n");
            
            // Show first few mismatches
            int mismatches = 0;
            for (size_t i = 0; i < N && mismatches < 5; i++) {
                if (G[i] != complete_private_G[i]) {
                    print_string("Mismatch[");
                    print_u32(i);
                    print_string("]: computed=");
                    if (G[i] < 0) print_string("-");
                    print_u32(G[i] < 0 ? -G[i] : G[i]);
                    print_string(", expected=");
                    if (complete_private_G[i] < 0) print_string("-");
                    print_u32(complete_private_G[i] < 0 ? -complete_private_G[i] : complete_private_G[i]);
                    print_string("\n");
                    mismatches++;
                }
            }
        }
    } else {
        print_string("FAIL (f not invertible)\n");
    }
}

void test_is_invertible() {
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    int result;
    
    print_string("[TEST] is_invertible... ");    
    result = Zf(is_invertible)(verify_raw_sig + N, LOGN, tmp_buffer); // s2 part of signature
    print_string(result ? "PASS\n" : "FAIL\n");
}

void test_verify_raw() {
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    int result;
    print_string("[TEST] verify_raw... ");
    
    result = Zf(verify_raw)(verify_raw_hm, verify_raw_sig, verify_raw_h, LOGN, tmp_buffer);
    print_string(result ? "PASS\n" : "FAIL\n");
}

void test_verify_recover() {
    uint16_t h[N];
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    int result;
    print_string("[TEST] verify_recover... ");

    result = Zf(verify_recover)(h, verify_raw_hm, verify_recover_s1, verify_raw_sig, LOGN, tmp_buffer);
    Zf(to_ntt_monty)(h, LOGN);
    if (result) {
        if (memcmp(h, verify_raw_h, sizeof(h)) == 0) {
            print_string("PASS\n");
        } else {
            print_string("FAIL (output mismatch)\n");
            
            // Show first few mismatches
            int mismatches = 0;
            for (size_t i = 0; i < N && mismatches < 5; i++) {
                if (h[i] != verify_raw_h[i]) {
                    print_string("Mismatch[");
                    print_u32(i);
                    print_string("]: recovered=");
                    print_u32(h[i]);
                    print_string(", expected=");
                    print_u32(verify_raw_h[i]);
                    print_string("\n");
                    mismatches++;
                }
            }
        }
    } else {
        print_string("FAIL (verification failed)\n");
    }
}

void test_count_nttzero() {
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    int count;
    print_string("[TEST] count_nttzero... ");
    count = Zf(count_nttzero)(verify_raw_sig + N, LOGN, tmp_buffer); // s2 part of signature
    
    print_string("Count: ");
    print_u32(count);
    if (count >= 0 && count <= N) {
        print_string(" (PASS)\n");
    } else {
        print_string(" (FAIL - invalid count)\n");
    }
}

void test_hw_basic_ops() {
    uint16_t poly_a_falcon[N];
    uint16_t poly_b_falcon[N];
    uint16_t poly_result_falcon[N];
    
    uint32_t poly_a_hw[256];
    uint32_t poly_b_hw[256];
    uint32_t poly_result_hw[256];
    
    print_string("\n[TEST] Hardware Basic Operations (ADD/SUB)...\n");
    print_string("  Strategy: Pack 512×16-bit into 256×32-bit words\n");
    
    // Initialize Falcon data
    for (size_t i = 0; i < N; i++) {
        poly_a_falcon[i] = (uint16_t)(i % 100);
        poly_b_falcon[i] = (uint16_t)(50);
        poly_result_falcon[i] = 0xFFFF;
    }
    
    print_string("  Input: poly_a[0..4]={");
    for (int i = 0; i < 5; i++) {
        print_u32(poly_a_falcon[i]);
        if (i < 4) print_string(",");
    }
    print_string("}\n");
    
    // Test 1: Addition - SINGLE call now!
    print_string("  Test 1: ADD operation (all 512 elements at once)...\n");
    
    falcon_to_hw_format(poly_a_hw, poly_a_falcon, 512);
    falcon_to_hw_format(poly_b_hw, poly_b_falcon, 512);
    
    int ret = ntt_lite_add(poly_result_hw, poly_a_hw, poly_b_hw);
    print_string("    Return code: ");
    print_u32(ret);
    print_string("\n");
    
    hw_to_falcon_format(poly_result_falcon, poly_result_hw, 512);
    
    print_string("  Output: poly_result[0..4]={");
    for (int i = 0; i < 5; i++) {
        print_u32(poly_result_falcon[i]);
        if (i < 4) print_string(",");
    }
    print_string("}\n");
    
    // Verify all 512 elements
    bool add_pass = true;
    for (size_t i = 0; i < N; i++) {
        uint32_t expected = (poly_a_falcon[i] + poly_b_falcon[i]) % Q;
        if (poly_result_falcon[i] != expected) {
            print_string("    ADD FAIL at [");
            print_u32(i);
            print_string("]: got ");
            print_u32(poly_result_falcon[i]);
            print_string(", expected ");
            print_u32(expected);
            print_string("\n");
            add_pass = false;
            break;
        }
    }
    
    if (add_pass) {
        print_string("    ADD: PASS (all 512 elements)\n");
    }
    
    print_string("[TEST] Hardware Basic Operations Complete\n");
}

void test_ntt_funcs() {
    uint16_t poly_test[N];
    uint16_t poly_backup[N];
    uint16_t poly_expected[N];
    
    print_string("\n[TEST] Hardware NTT Functions...\n");
    
    // ==================== Test 1: Forward NTT ====================
    print_string("  Test 1: Forward NTT (mq_NTT)...\n");
    
    // Initialize with simple pattern
    for (size_t i = 0; i < N; i++) {
        poly_test[i] = (uint16_t)(i % 100);
        poly_backup[i] = poly_test[i];
    }
    
    print_string("    Input: poly[0..4]={");
    for (int i = 0; i < 5; i++) {
        print_u32(poly_test[i]);
        if (i < 4) print_string(",");
    }
    print_string("}\n");
    poly_init_ntt();
    // Call HW-accelerated NTT
    mq_NTT(poly_test, LOGN);
    
    print_string("    Output: poly[0..4]={");
    for (int i = 0; i < 5; i++) {
        print_u32(poly_test[i]);
        if (i < 4) print_string(",");
    }
    print_string("}\n");
    
    // Check that output is different (NTT should transform the data)
    bool ntt_changed = false;
    for (size_t i = 0; i < N; i++) {
        if (poly_test[i] != poly_backup[i]) {
            ntt_changed = true;
            break;
        }
    }
    
    if (ntt_changed) {
        print_string("    Forward NTT: Data transformed (PASS)\n");
    } else {
        print_string("    Forward NTT: FAIL - Data unchanged!\n");
    }
    
    // ==================== Test 2: Inverse NTT ====================
    print_string("  Test 2: Inverse NTT (mq_iNTT)...\n");
    poly_init_invntt();
    // Apply iNTT to get back original data
    mq_iNTT(poly_test, LOGN);
    
    print_string("    Output: poly[0..4]={");
    for (int i = 0; i < 5; i++) {
        print_u32(poly_test[i]);
        if (i < 4) print_string(",");
    }
    print_string("}\n");
    
    // Verify we got back original data
    bool intt_pass = true;
    for (size_t i = 0; i < N; i++) {
        if (poly_test[i] != poly_backup[i]) {
            print_string("    iNTT FAIL at [");
            print_u32(i);
            print_string("]: got ");
            print_u32(poly_test[i]);
            print_string(", expected ");
            print_u32(poly_backup[i]);
            print_string("\n");
            intt_pass = false;
            break;
        }
    }
    
    if (intt_pass) {
        print_string("    Inverse NTT: Original data recovered (PASS)\n");
    }
    
    // ==================== Test 3: NTT + Pointwise Mul ====================
    print_string("  Test 3: Pointwise multiplication (mq_poly_montymul_ntt)...\n");
    
    uint16_t poly_a[N];
    uint16_t poly_b[N];
    
    // Initialize polynomials
    for (size_t i = 0; i < N; i++) {
        poly_a[i] = (uint16_t)((i % 50) + 1);  // 1-50
        poly_b[i] = (uint16_t)2;                // constant 2
    }
    poly_init_ntt();
    // Transform to NTT domain
    mq_NTT(poly_a, LOGN);
    mq_NTT(poly_b, LOGN);
    
    print_string("    After NTT: poly_a[0..2]={");
    for (int i = 0; i < 3; i++) {
        print_u32(poly_a[i]);
        if (i < 2) print_string(",");
    }
    print_string("}\n");
    
    // Pointwise multiplication in NTT domain
    mq_poly_montymul_ntt(poly_a, poly_b, LOGN);
    
    print_string("    After PWM: poly_a[0..2]={");
    for (int i = 0; i < 3; i++) {
        print_u32(poly_a[i]);
        if (i < 2) print_string(",");
    }
    print_string("}\n");
    
    // Check that multiplication happened (values should have changed)
    bool pwm_changed = false;
    mq_NTT(poly_b, LOGN);  // Get original NTT(poly_b) for comparison
    for (size_t i = 0; i < N; i++) {
        if (poly_a[i] != poly_b[i]) {
            pwm_changed = true;
            break;
        }
    }
    
    if (pwm_changed) {
        print_string("    Pointwise multiplication: Data changed (PASS)\n");
    } else {
        print_string("    Pointwise multiplication: FAIL - No change detected\n");
    }
    
    // ==================== Test 4: Polynomial Subtraction ====================
    print_string("  Test 4: Polynomial subtraction (mq_poly_sub)...\n");
    
    uint16_t poly_x[N];
    uint16_t poly_y[N];
    
    for (size_t i = 0; i < N; i++) {
        poly_x[i] = (uint16_t)(100 + (i % 50));  // 100-149
        poly_y[i] = (uint16_t)(50);              // constant 50
    }
    
    print_string("    Input: poly_x[0..2]={");
    for (int i = 0; i < 3; i++) {
        print_u32(poly_x[i]);
        if (i < 2) print_string(",");
    }
    print_string("}, poly_y[0]=");
    print_u32(poly_y[0]);
    print_string("\n");
    
    // Subtract
    mq_poly_sub(poly_x, poly_y, LOGN);
    
    print_string("    Output: poly_x[0..2]={");
    for (int i = 0; i < 3; i++) {
        print_u32(poly_x[i]);
        if (i < 2) print_string(",");
    }
    print_string("}\n");
    
    // Verify subtraction
    bool sub_pass = true;
    for (size_t i = 0; i < 10; i++) {
        uint16_t expected = (uint16_t)((100 + (i % 50) - 50 + Q) % Q);
        if (poly_x[i] != expected) {
            print_string("    SUB FAIL at [");
            print_u32(i);
            print_string("]: got ");
            print_u32(poly_x[i]);
            print_string(", expected ");
            print_u32(expected);
            print_string("\n");
            sub_pass = false;
            break;
        }
    }
    
    if (sub_pass) {
        print_string("    Polynomial subtraction: PASS\n");
    }
    
    // ==================== Test 5: Full NTT Cycle ====================
    print_string("  Test 5: Full cycle (NTT→operations→iNTT)...\n");
    
    uint16_t poly_orig[N];
    uint16_t poly_work[N];
    
    // Initialize
    for (size_t i = 0; i < N; i++) {
        poly_orig[i] = (uint16_t)(i % 200);
        poly_work[i] = poly_orig[i];
    }
    
    // Forward NTT
    poly_init_ntt();
    mq_NTT(poly_work, LOGN);
    print_string("    After NTT: poly[0]=");
    print_u32(poly_work[0]);
    print_string("\n");

    poly_init_invntt();
    // Inverse NTT
    mq_iNTT(poly_work, LOGN);
    print_string("    After iNTT: poly[0]=");
    print_u32(poly_work[0]);
    print_string("\n");
    
    // Verify round-trip
    bool cycle_pass = true;
    int error_count = 0;
    for (size_t i = 0; i < N; i++) {
        if (poly_work[i] % Q != poly_orig[i % Q]) {
            if (error_count < 3) {
                print_string("    Cycle FAIL at [");
                print_u32(i);
                print_string("]: got ");
                print_u32(poly_work[i]);
                print_string(", expected ");
                print_u32(poly_orig[i]);
                print_string("\n");
            }
            error_count++;
            cycle_pass = false;
        }
    }
    
    if (cycle_pass) {
        print_string("    Full NTT cycle: PASS (all 512 elements preserved)\n");
    } else {
        print_string("    Full NTT cycle: FAIL (");
        print_u32(error_count);
        print_string(" errors)\n");
    }
    
    print_string("[TEST] Hardware NTT Functions Complete\n");
}
void test_ntt_montgomery_compat() {
    uint16_t poly_hw[N];
    uint16_t poly_sw[N];
    
    print_string("\n[TEST] NTT+Montgomery Compatibility...\n");
    
    // Initialize same data
    for (size_t i = 0; i < N; i++) {
        poly_hw[i] = (uint16_t)(i % 100);
        poly_sw[i] = (uint16_t)(i % 100);
    }
    
    // HW path: NTT then convert to Montgomery
    mq_NTT(poly_hw, LOGN);
    mq_poly_tomonty(poly_hw, LOGN);
    
    print_string("  HW+SW_tomonty: poly[0..4]={");
    for (int i = 0; i < 5; i++) {
        print_u32(poly_hw[i] );
        if (i < 4) print_string(",");
    }
    print_string("}\n");
    
    print_string("  Expected:      poly[0..4]={");
    for (int i = 0; i < 5; i++) {
        print_u32(ntt_output_h[i] );  // From test vectors
        if (i < 4) print_string(",");
    }
    print_string("}\n");
    
    // Check if they match
    bool match = true;
    for (int i = 0; i < 10; i++) {
        if (poly_hw[i]  != ntt_output_h[i] ) {
            match = false;
            break;
        }
    }
    
    if (match) {
        print_string("  Montgomery compatibility: PASS\n");
    } else {
        print_string("  Montgomery compatibility: FAIL - Different output\n");
        print_string("  This means HW NTT produces different results than SW\n");
    }
}

void test_packing() {
    uint16_t test_falcon[512];
    uint32_t test_hw[256];
    uint16_t test_recovered[512];
    
    print_string("\n[TEST] Packing/Unpacking Verification...\n");
    
    // Initialize with simple sequential pattern
    for (size_t i = 0; i < 512; i++) {
        test_falcon[i] = (uint16_t)i;
    }
    
    print_string("  Original: [0..7]={");
    for (int i = 0; i < 8; i++) {
        print_u32(test_falcon[i]);
        if (i < 7) print_string(",");
    }
    print_string("}\n");
    
    // Pack
    falcon_to_hw_format(test_hw, test_falcon, 512);
    
    print_string("  Packed HW[0..3]={");
    for (int i = 0; i < 4; i++) {
        print_u32(test_hw[i]);
        if (i < 3) print_string(",");
    }
    print_string("}\n");
    
    // Unpack
    hw_to_falcon_format(test_recovered, test_hw, 512);
    
    print_string("  Recovered: [0..7]={");
    for (int i = 0; i < 8; i++) {
        print_u32(test_recovered[i]);
        if (i < 7) print_string(",");
    }
    print_string("}\n");
    
    // Verify pack/unpack round-trip
    bool pack_ok = true;
    for (size_t i = 0; i < 512; i++) {
        if (test_falcon[i] != test_recovered[i]) {
            print_string("  Pack/Unpack FAIL at [");
            print_u32(i);
            print_string("]: got ");
            print_u32(test_recovered[i]);
            print_string(", expected ");
            print_u32(test_falcon[i]);
            print_string("\n");
            pack_ok = false;
            break;
        }
    }
    
    if (pack_ok) {
        print_string("  Pack/Unpack: PASS\n");
    }
    
    // Now test with HW ADD operation
    print_string("  Testing pack→HW_ADD→unpack...\n");
    
    uint16_t add_a[512];
    uint16_t add_b[512];
    uint16_t add_result[512];
    uint32_t hw_a[256];
    uint32_t hw_b[256];
    uint32_t hw_result[256];
    
    for (size_t i = 0; i < 512; i++) {
        add_a[i] = (uint16_t)(i % 100);
        add_b[i] = 10;
    }
    
    falcon_to_hw_format(hw_a, add_a, 512);
    falcon_to_hw_format(hw_b, add_b, 512);
    ntt_lite_add(hw_result, hw_a, hw_b);
    hw_to_falcon_format(add_result, hw_result, 512);
    
    bool add_ok = true;
    for (size_t i = 0; i < 512; i++) {
        uint16_t expected = (add_a[i] + add_b[i]) % Q;
        if (add_result[i] != expected) {
            print_string("  ADD through pack FAIL at [");
            print_u32(i);
            print_string("]\n");
            add_ok = false;
            break;
        }
    }
    
    if (add_ok) {
        print_string("  Pack→ADD→Unpack: PASS\n");
    }
    
    print_string("[TEST] Packing Verification Complete\n");
}
void test_montgomery_check() {
    uint16_t poly_hw[N];
    uint16_t poly_sw[N];
    
    print_string("\n[TEST] Montgomery Representation Check...\n");
    
    // Initialize same data
    for (size_t i = 0; i < N; i++) {
        poly_hw[i] = (uint16_t)(i % 100);
        poly_sw[i] = (uint16_t)(i % 100);
    }
    
    // HW path: Just NTT (no tomonty)
    mq_NTT(poly_hw, LOGN);
    
    print_string("  HW NTT (no tomonty): poly[0..4]={");
    for (int i = 0; i < 5; i++) {
        print_u32(poly_hw[i]);
        if (i < 4) print_string(",");
    }
    print_string("}\n");
    
    print_string("  Expected (with tomonty): poly[0..4]={");
    for (int i = 0; i < 5; i++) {
        print_u32(ntt_output_h[i]);
        if (i < 4) print_string(",");
    }
    print_string("}\n");
    
    // Check if HW output matches the "with Montgomery" expected output
    bool matches_montgomery = true;
    for (int i = 0; i < 10; i++) {
        if (poly_hw[i] != ntt_output_h[i]) {
            matches_montgomery = false;
            break;
        }
    }
    
    if (matches_montgomery) {
        print_string("  Result: HW NTT already includes Montgomery! No tomonty needed.\n");
    } else {
        print_string("  Result: HW NTT is NOT in Montgomery form.\n");
        print_string("  We need to adjust or skip Montgomery conversion.\n");
    }
}
int main() {
    print_string("\n=== Falcon-512 Function Tests ===\n");
    
    // Initialize hardware ONCE at startup
    print_string("Initializing HW accelerator (logn=8, DUAL mode)...\n");
    poly_init_q();
    print_string("HW initialized.\n");

    test_packing();  
    test_montgomery_check();
    // Test hardware basic operations (ADD/SUB)
    test_hw_basic_ops();
    test_ntt_montgomery_compat();
    // Test NTT functions
    test_ntt_funcs();
    
    print_string("\n--- Software-only tests ---\n");
    // Run original tests (these now use HW-accelerated functions!)
    test_to_ntt_monty();
    test_compute_public();
    test_complete_private();
    test_is_invertible();
    test_verify_raw();
    test_verify_recover();
    test_count_nttzero();
    
    print_string("\n=== All Tests Completed ===\n");
    return 0;
}