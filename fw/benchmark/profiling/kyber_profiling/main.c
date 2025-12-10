#include <stdint.h>
#include "string.h"
#include "timer.h"
#include "uart.h"
#include "util.h"
#include "indcpa.h"
#include "kem.h"
#include "masked_indcpa.h"
#include "masked_kem.h"
#include "x2x.h"
#include "benchmark.h"
#include "timer.h"


extern unsigned int ntt_lite_cc;
extern unsigned int keccak_cc;
extern unsigned int x2x_cc;




void reset_modules_cc() {
    ntt_lite_cc = 0;
    keccak_cc = 0;
    x2x_cc = 0;
}


void print_modules_cc(unsigned int time, unsigned int shift) {
    print_string("NTT-Lite cycles:\t");
    print_u32_int(ntt_lite_cc >> shift);
    print_string("\n");
    print_string("Keccak cycles:\t");
    print_u32_int(keccak_cc >> shift);
    print_string("\n");
    print_string("X2X cycles:\t");
    print_u32_int(x2x_cc >> shift);
    print_string("\n");
    time = time - ntt_lite_cc - keccak_cc - x2x_cc;
    print_string("SW cycles:\t");
    print_u32_int(time >> shift);
    print_string("\n");
    reset_modules_cc();
}


void test() {
    static uint8_t pk_cca__[KYBER_PUBLICKEYBYTES];
    static uint8_t sk_cca__[KYBER_SECRETKEYBYTES];
    static uint8_t K__[KYBER_SSBYTES];
    static uint8_t c_cca__[KYBER_CIPHERTEXTBYTES];
    static uint8_t K__K[KYBER_SSBYTES];

    BENCH_INIT()
    reset_modules_cc(); 
    BENCH_START()

    crypto_kem_keypair(pk_cca__, sk_cca__);

    BENCH_END(KEM_KEYPAIR)
    print_modules_cc(time, 0);

    (void) pk_cca__;
    (void) sk_cca__;

    BENCH_START()

    crypto_kem_enc(c_cca__, K__, pk_cca__);
    (void) c_cca__;
    (void) K__;

    BENCH_END(KEM_ENC)
    print_modules_cc(time, 0);

    BENCH_START()

#ifndef MASKING_EN
    crypto_kem_dec(K__K, c_cca__, sk_cca__);
#else
    masked_crypto_kem_dec(K__K, c_cca__, sk_cca__);
#endif
    (void) K__K;

    BENCH_END(KEM_DEC)
    print_modules_cc(time, 0);

}


int main() {
#ifdef MASKING_EN
    uint32_t seed[2] = {1, 1};
    x2x_seed(seed);
#endif
    test();
}