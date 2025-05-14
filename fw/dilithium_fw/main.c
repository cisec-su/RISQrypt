//main.c file for gen and verify signatures for dilithium
// purpose is to compare 2 results, which are generated in FPGA and python
// Author: Yusuf SUR 

#include <stdio.h>
#include <stdint.h>
#include "params.h"
#include "sign.h"
#include "util.h"
#include "randombytes.h"
#include "symmetric.h"


int main() {
    uint8_t pk[CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[CRYPTO_SECRETKEYBYTES];
    uint8_t sig[CRYPTO_BYTES];
    uint8_t message[] = "Test message for dilithium signing";
    size_t msg_len = sizeof(message) - 1;
    size_t sig_len;
    int ret;

    print_string("\n Dilithium key generation \n");
    ret = crypto_sign_keypair(pk, sk);
    if (ret != 0) {
        print_string("Key generation failed\n");
        return -1;
    }
    print_hex_data("Public Key: ", pk, CRYPTO_PUBLICKEYBYTES);
    print_hex_data("Secret Key: ", sk, CRYPTO_SECRETKEYBYTES);

    print_string("\n Signing the message \n");
    ret = crypto_sign_signature(sig, &sig_len, message, msg_len, sk);
    if (ret != 0) {
        print_string("Signature generation failed\n");
        return -1;
    }
    print_hex_data("Signature: ", sig, sig_len);

    print_string("\n verifying the signature \n");
    ret = crypto_sign_verify(sig, sig_len, message, msg_len, pk);
    if (ret != 0) {
        print_string("Signature verification failed\n");
        return -1;
    }
    print_string("Signature verification successful\n");

    return 0;
}
