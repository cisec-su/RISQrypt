#include <stdio.h>
#include <stdint.h>
#include "x2x.h"
#include "sign.h"
#include "masked_sign.h"


void test() {
    size_t sig_len;
    int ret;
    void *pk__;
    void *sk__;
    void *sig__;
    void *msg__;
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
    test();
}