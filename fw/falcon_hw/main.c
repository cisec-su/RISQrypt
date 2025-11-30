#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "inner.h"
#include "uart.h"
#include "util.h"
#include "falcon_testvectors.h"


/* If FALCON_ERR_* constants are missing after commenting falcon.h, define them here: */
#ifndef FALCON_ERR_FORMAT
#define FALCON_ERR_RANDOM     -1
#define FALCON_ERR_SIZE       -2
#define FALCON_ERR_FORMAT     -3
#define FALCON_ERR_BADSIG     -4
#define FALCON_ERR_BADARG     -5
#endif
/* --- API Bridge Definitions --- */

/* INCREASED SIZE to prevent stack overflow */
#ifndef FALCON_H__
typedef struct { uint64_t opaque[64]; } shake256_context;
#endif

#define FALCON_SIG_COMPRESSED   1
#define FALCON_SIG_PADDED       2
#define FALCON_SIG_CT           3

/* API Prototypes */
int falcon_verify_start(shake256_context *hash_data, const void *sig, size_t sig_len);
void shake256_inject(shake256_context *sc, const void *data, size_t len);
int falcon_verify_finish(const void *sig, size_t sig_len, int sig_type, 
                        const void *pubkey, size_t pubkey_len, 
                        shake256_context *hash_data, void *tmp, size_t tmp_len);
int falcon_verify(const void *sig, size_t sig_len, int sig_type,
                 const void *pubkey, size_t pubkey_len,
                 const void *data, size_t data_len,
                 void *tmp, size_t tmp_len);
/* ------------------------------ */
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
                print_string(" OK\n");
            } else {
                print_string(" FAIL\n");
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

/* --- Updated test function --- */
void test_falcon_verify_api() {
    shake256_context hd;
    int result;
    
    /* Use ALIGNED stack buffers */
    uint64_t sig_store[100]; /* 800 bytes */
    uint64_t pk_store[120];  /* 960 bytes */
    uint8_t *sig_buf = (uint8_t *)sig_store;
    uint8_t *pk_buf  = (uint8_t *)pk_store;

    /* Prepare dummy data */
    memset(sig_buf, 0, 800);
    memset(pk_buf, 0, 960);
    sig_buf[0] = 0x39; /* Padded + LogN=9 */
    pk_buf[0]  = 0x09; /* LogN=9 */

    print_string("[TEST] falcon_verify_api (Streaming)... \n");
    
    /* Call the safe debug version */
    result = falcon_verify_start(&hd, sig_buf, 666);
    
    /* If it crashes before this line, check the DEBUG prints */
    shake256_inject(&hd, test_msg, sizeof(test_msg));
    
    result = falcon_verify_finish(sig_buf, 666, FALCON_SIG_PADDED,
                                pk_buf, 897, 
                                &hd, tmp_buffer, sizeof(tmp_buffer));
    
    print_string("Complete (Result: ");
    print_u32((uint32_t)result);
    print_string(")\n");
}

int main() {
    print_string("\n=== Falcon-512 Function Tests ===\n");
    test_falcon_verify_api();
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