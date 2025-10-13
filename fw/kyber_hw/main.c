#include <stdint.h>
#include "string.h"
#include "timer.h"
#include "uart.h"
#include "util.h"
#include "indcpa.h"
#include "kem.h"
#include "masked_indcpa.h"
#include "x2x.h"


#define MASKING_EN


void test_indcca() {
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
#endif

}


#ifdef MASKING_EN

void test_masked_indcpa_dec() {
    uint8_t *sk__;
    uint8_t *c__;
    uint8_t *mm__;

    masked_indcpa_dec(mm__, c__, sk__);
    (void) mm__;
}


void test_masked_indcpa_enc_cmp() {
    int fail;
    uint8_t *masked_coins__;
    uint8_t *pk__;
    uint8_t *mm__;
    uint8_t *c__;

    fail = masked_indcpa_enc_cmp(c__, mm__, pk__, masked_coins__);
    (void) fail;
}

#endif


int main() {
#ifdef MASKING_EN
    uint32_t seed[2] = {1, 1};
    x2x_seed(seed);
#endif
    test_indcca();
#ifdef MASKING_EN
    test_masked_indcpa_dec();
    test_masked_indcpa_enc_cmp();
#endif
}
