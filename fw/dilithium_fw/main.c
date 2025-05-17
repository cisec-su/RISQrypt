// main.c file for gen and verify signatures for Dilithium
// Purpose is to compare 2 results, which are generated in FPGA and Python
// Author: Yusuf SUR

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "sign.h"
#include "util.h"
#include "randombytes.h"
#include "symmetric.h"

// Helper function to print hex data with a label
void print_hex_data(const char *label, const uint8_t *data, size_t len) {
    print_string(label);
    print_hex(data, len, 0);
}

int main() {
    uint8_t pk[CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[CRYPTO_SECRETKEYBYTES];
    uint8_t sig[CRYPTO_BYTES];
    uint8_t message[] = "Test message for Dilithium signing";
    size_t msg_len = sizeof(message) - 1;
    size_t sig_len;
    int ret;

    print_string("\n Dilithium key generation \n");
    ret = crypto_sign_keypair(pk, sk);
    if (ret != 0) {
        print_string("Key generation failed\n");
        return -1;
    }
    print_string("Public Key: ");
    print_hex(pk, CRYPTO_PUBLICKEYBYTES, 0);
    print_string("\n");

    print_string("Secret Key: ");
    print_hex(sk, CRYPTO_SECRETKEYBYTES, 0);
    print_string("\n");

    print_string("\n Signing the message \n");
    ret = crypto_sign_signature(sig, &sig_len, message, msg_len, sk);
    if (ret != 0) {
        print_string("Signature generation failed\n");
        return -1;
    }
    print_string("Signature: ");
    print_hex(sig, sig_len, 0);
    print_string("\n");

    print_string("\n Verifying the signature \n");
    ret = crypto_sign_verify(sig, sig_len, message, msg_len, pk);
    if (ret != 0) {
        print_string("Signature verification failed\n");
        return -1;
    }
    print_string("Signature verification successful\n");

    return 0;
}
