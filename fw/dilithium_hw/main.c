#include <stdio.h>
#include <stdint.h>
#include "x2x.h"
#include "sign.h"
#include "masked_sign.h"


#define MASKING_EN


void dilithium_simple() {
    size_t sig_len;
    int ret;
    uint8_t *pk__;
    uint8_t *sk__;
    uint8_t *sig__;
    uint8_t *msg__;
    uint32_t msg_len__;

    crypto_sign_keypair(pk__, sk__);
    (void) pk__;
    (void) sk__;

#ifdef MASKING_EN
    masked_crypto_sign_signature(sig__, &sig_len, msg__, msg_len__, sk__);
#else
    crypto_sign_signature(sig__, &sig_len, msg__, msg_len__, sk__);
#endif
    (void) sig__;
    (void) msg__;
    (void) sig_len;

    ret = crypto_sign_verify(sig__, CRYPTO_BYTES, msg__, msg_len__, pk__);
    (void) ret;

}


int main() {
#ifdef MASKING_EN
    uint32_t seed[2] = {1, 1};
    x2x_seed(seed);
#endif
    dilithium_simple();
}