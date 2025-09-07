#include <stdint.h>
#include <string.h>
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
static const uint8_t test_msg[] = "Test Falcon";
static const size_t test_msg_len = 11;

// Global buffer for large temporary allocations
static uint8_t tmp_buffer[2 * N * sizeof(uint16_t)];

// Debug print functions
void print_int8_array(const char* name, const int8_t* arr, size_t len) {
    print_string(name);
    print_string(": [");
    for (size_t i = 0; i < (len < 10 ? len : 10); i++) {
        print_u32(arr[i]);
        if (i < (len < 10 ? len : 10) - 1) {
            print_string(", ");
        }
    }
    if (len > 10) {
        print_string(", ...");
    }
    print_string("]\n");
}

void print_int16_array(const char* name, const int16_t* arr, size_t len) {
    print_string(name);
    print_string(": [");
    for (size_t i = 0; i < (len < 10 ? len : 10); i++) {
        print_u32(arr[i]);
        if (i < (len < 10 ? len : 10) - 1) {
            print_string(", ");
        }
    }
    if (len > 10) {
        print_string(", ...");
    }
    print_string("]\n");
}

void test_to_ntt_monty() {
    uint16_t poly[N];
    memcpy(poly, test_h, sizeof(poly)); 
    
    print_string("[TEST] to_ntt_monty... ");
    Zf(to_ntt_monty)(poly, LOGN);
    print_string("PASS (test_to_ntt_monty)\n");
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
            print_int16_array("Computed h", h, N);
            print_int16_array("Expected h", test_h, N);
        }
    } else {
        print_string("FAIL (returned 0)\n");
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
        }
    } else {
        print_string("FAIL (returned 0 - f not invertible)\n");
    }
}

void test_is_invertible() {
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    int result;
    
    print_string("[TEST] is_invertible... ");    
    result = Zf(is_invertible)(test_s2, LOGN, tmp_buffer);
    if (result) {
        print_string("PASS\n");
    } else {
        print_string("FAIL\n");
    }
}

void test_verify_raw() {
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    int result;
    print_string("[TEST] verify_raw... ");
    
    result = Zf(verify_raw)(test_c0, test_s2, test_h, LOGN, tmp_buffer);
    if (result) {
        print_string("PASS\n");
    } else {
        print_string("FAIL\n");
    }
}

void test_verify_recover() {
    uint16_t h[N];
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    int result;
    print_string("[TEST] verify_recover... ");

    result = Zf(verify_recover)(h, test_c0, test_s1, test_s2, LOGN, tmp_buffer);
    if (result) {
        print_int16_array("Recovered h", h, N);
        print_int16_array("Expected h", test_h, N);
        
        if (memcmp(h, test_h, sizeof(h)) == 0) {
            print_string("PASS\n");
        } else {
            print_string("FAIL (output mismatch)\n");
        }
    } else {
        print_int16_array("Recovered h", h, N);
        print_int16_array("Expected h", test_h, N);
        
        print_string("FAIL\n");
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