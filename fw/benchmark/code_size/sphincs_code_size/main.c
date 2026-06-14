#include <stdint.h>
#include <string.h>
#include "timer.h"
#include "uart.h"
#include "util.h"
#include "api.h"
#include "masked_sign.h"


void test() {
    unsigned char *pk__;
    unsigned char *sk__;
    unsigned char *m__;
    unsigned char *sm__;
    unsigned char *mout__;
    unsigned long long smlen__;
    unsigned long long mlen__;
    size_t smlen_u;
    size_t mlen_u;
    int ret;

    // Unmasked API
    crypto_sign_keypair(pk__, sk__);
    (void) pk__;
    (void) sk__;

    crypto_sign(sm__, &smlen_u, m__, 32, sk__);
    (void) sm__;
    (void) smlen_u;

    ret = crypto_sign_open(mout__, &mlen_u, sm__, smlen_u, pk__);
    (void) ret;
    (void) mout__;
    (void) mlen_u;

    // HW-masked API
    masked_crypto_sign_keypair(pk__, sk__);

    masked_crypto_sign(sm__, &smlen__, m__, 32, sk__);
    (void) smlen__;

    ret = masked_crypto_sign_open(mout__, &mlen__, sm__, smlen__, pk__);
    (void) mlen__;
}


int main() {
    test();
}
