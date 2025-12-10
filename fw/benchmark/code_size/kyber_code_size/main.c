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



void test() {
    uint8_t *pk_cca__;
    uint8_t *sk_cca__;
    uint8_t *K__;
    uint8_t *c_cca__;
    uint8_t *K__K;
    crypto_kem_keypair(pk_cca__, sk_cca__);

    (void) pk_cca__;
    (void) sk_cca__;

    crypto_kem_enc(c_cca__, K__, pk_cca__);
    (void) c_cca__;

#ifndef MASKING_EN
    crypto_kem_dec(K__K, c_cca__, sk_cca__);
    (void) K__K;
#else
    masked_crypto_kem_dec(K__K, c_cca__, sk_cca__);
    (void) K__K;
#endif

}


int main() {
#ifdef MASKING_EN
    uint32_t seed[2] = {1, 1};
    x2x_seed(seed);
#endif
    test();
}