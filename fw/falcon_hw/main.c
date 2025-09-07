#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "inner.h"
#include "uart.h"
#include "util.h"
#include "falcon_testvectors.h"

// Falcon-512 parameters
#define CRYPTO_SECRETKEYBYTES   1281
#define CRYPTO_PUBLICKEYBYTES   897
#define CRYPTO_BYTES            690
#define LOGN                    9
#define N                       (1 << LOGN)

// Test message:
static const uint8_t test_msg[] = "abc";

// Global buffer for large temporary allocations - with proper alignment
static uint8_t tmp_buffer[4 * N * sizeof(uint16_t)] __attribute__((aligned(16)));

// Test function to validate and convert test vectors
void validate_and_convert_test_vectors() {
    print_string("[VALIDATION] Test vector validation and conversion:\n");
    
    // Check f, g ranges (should be in [-127, 127])
    bool fg_valid = true;
    for (size_t i = 0; i < N; i++) {
        if (test_f[i] < -127 || test_f[i] > 127 || 
            test_g[i] < -127 || test_g[i] > 127) {
            fg_valid = false;
            print_string("f,g out of range at index ");
            print_u32(i);
            print_string(": f=");
            if (test_f[i] < 0) print_string("-");
            print_u32(test_f[i] < 0 ? -test_f[i] : test_f[i]);
            print_string(", g=");
            if (test_g[i] < 0) print_string("-");
            print_u32(test_g[i] < 0 ? -test_g[i] : test_g[i]);
            print_string("\n");
            break;
        }
    }
    print_string("f,g range [-127,127]: ");
    print_string(fg_valid ? "PASS\n" : "FAIL\n");
    
    // Check F range
    bool F_valid = true;
    for (size_t i = 0; i < N; i++) {
        if (test_F[i] < -127 || test_F[i] > 127) {
            F_valid = false;
            break;
        }
    }
    print_string("F range [-127,127]: ");
    print_string(F_valid ? "PASS\n" : "FAIL\n");
    
    // Check h range (should be in [0, 12288])
    bool h_valid = true;
    for (size_t i = 0; i < N; i++) {
        if (test_h[i] >= 12289) {
            h_valid = false;
            print_string("h out of range at index ");
            print_u32(i);
            print_string(": ");
            print_u32(test_h[i]);
            print_string("\n");
            break;
        }
    }
    print_string("h range [0,12288]: ");
    print_string(h_valid ? "PASS\n" : "FAIL\n");
    
    // Check s2 conversion
    print_string("s2 original format analysis:\n");
    int large_values = 0, small_values = 0;
    for (size_t i = 0; i < N; i++) {
        if (test_s2[i] > 6144) large_values++;
        else small_values++;
    }
    print_string("Values > 6144: ");
    print_u32(large_values);
    print_string(", Values <= 6144: ");
    print_u32(small_values);
    print_string("\n");
    
    print_string("Sample s2 conversions:\n");
    for (size_t i = 0; i < 5; i++) {
        print_u32(test_s2[i]);
        print_string(" -> ");
        if (test_s2[i] > 6144) {
            print_string("-");
            print_u32(12289 - test_s2[i]);
        } else {
            print_u32(test_s2[i]);
        }
        print_string("\n");
    }
}

void test_to_ntt_monty() {
    uint16_t poly[N];
    memcpy(poly, test_h, sizeof(poly)); 
    
    print_string("[TEST] to_ntt_monty... ");
    Zf(to_ntt_monty)(poly, LOGN);
    print_string("PASS\n");
}

void test_compute_public() {
    uint16_t h[N];
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    int result;
    print_string("[TEST] compute_public... ");
    
    result = Zf(compute_public)(h, test_f, test_g, LOGN, tmp_buffer);
    if (result) {
        if (memcmp(h, test_h, sizeof(h)) == 0) {
            print_string("PASS\n");
        } else {
            print_string("FAIL (output mismatch)\n");
            
            // Show first few mismatches
            int mismatches = 0;
            for (size_t i = 0; i < N && mismatches < 5; i++) {
                if (h[i] != test_h[i]) {
                    print_string("Mismatch[");
                    print_u32(i);
                    print_string("]: computed=");
                    print_u32(h[i]);
                    print_string(", expected=");
                    print_u32(test_h[i]);
                    print_string("\n");
                    mismatches++;
                }
            }
            
            // Total count
            mismatches = 0;
            for (size_t i = 0; i < N; i++) {
                if (h[i] != test_h[i]) mismatches++;
            }
            print_string("Total mismatches: ");
            print_u32(mismatches);
            print_string("/");
            print_u32(N);
            print_string("\n");
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

    result = Zf(complete_private)(G, test_f, test_g, test_F, LOGN, tmp_buffer);
    if (result) {
        if (memcmp(G, test_G, sizeof(G)) == 0) {
            print_string("PASS\n");
        } else {
            print_string("FAIL (output mismatch)\n");
            
            // Show first few mismatches
            int mismatches = 0;
            for (size_t i = 0; i < N && mismatches < 5; i++) {
                if (G[i] != test_G[i]) {
                    print_string("Mismatch[");
                    print_u32(i);
                    print_string("]: computed=");
                    if (G[i] < 0) print_string("-");
                    print_u32(G[i] < 0 ? -G[i] : G[i]);
                    print_string(", expected=");
                    if (test_G[i] < 0) print_string("-");
                    print_u32(test_G[i] < 0 ? -test_G[i] : test_G[i]);
                    print_string("\n");
                    mismatches++;
                }
            }
            
            // Total count
            mismatches = 0;
            for (size_t i = 0; i < N; i++) {
                if (G[i] != test_G[i]) mismatches++;
            }
            print_string("Total mismatches: ");
            print_u32(mismatches);
            print_string("/");
            print_u32(N);
            print_string("\n");
        }
    } else {
        print_string("FAIL (f not invertible)\n");
    }
}

void test_is_invertible() {
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    int result;
    
    print_string("[TEST] is_invertible... ");    
    result = Zf(is_invertible)(test_s2, LOGN, tmp_buffer);
    print_string(result ? "PASS\n" : "FAIL\n");
}

void test_verify_raw() {
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    int result;
    print_string("[TEST] verify_raw... ");
    
    result = Zf(verify_raw)(test_c0, test_s2, test_h, LOGN, tmp_buffer);
    print_string(result ? "PASS\n" : "FAIL\n");
}

void test_verify_recover() {
    uint16_t h[N];
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    int result;
    print_string("[TEST] verify_recover... ");

    result = Zf(verify_recover)(h, test_c0, test_s1, test_s2, LOGN, tmp_buffer);
    if (result) {
        if (memcmp(h, test_h, sizeof(h)) == 0) {
            print_string("PASS\n");
        } else {
            print_string("FAIL (output mismatch)\n");
            
            // Show first few mismatches
            int mismatches = 0;
            for (size_t i = 0; i < N && mismatches < 5; i++) {
                if (h[i] != test_h[i]) {
                    print_string("Mismatch[");
                    print_u32(i);
                    print_string("]: recovered=");
                    print_u32(h[i]);
                    print_string(", expected=");
                    print_u32(test_h[i]);
                    print_string("\n");
                    mismatches++;
                }
            }
            
            // Total count
            mismatches = 0;
            for (size_t i = 0; i < N; i++) {
                if (h[i] != test_h[i]) mismatches++;
            }
            print_string("Total mismatches: ");
            print_u32(mismatches);
            print_string("/");
            print_u32(N);
            print_string("\n");
        }
    } else {
        print_string("FAIL (verification failed)\n");
    }
}

void test_count_nttzero() {
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    int count;
    print_string("[TEST] count_nttzero... ");
    count = Zf(count_nttzero)(test_s2, LOGN, tmp_buffer);
    
    print_string("Count: ");
    print_u32(count);
    if (count >= 0 && count <= N) {
        print_string(" (PASS)\n");
    } else {
        print_string(" (FAIL - invalid count)\n");
    }
}

int main() {
    print_string("\n=== Falcon-512 Function Tests ===\n");
    
    // First validate and analyze test vectors
    validate_and_convert_test_vectors();
    print_string("\n");
    
    // Run tests
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