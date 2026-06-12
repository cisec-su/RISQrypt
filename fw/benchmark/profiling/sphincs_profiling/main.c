#include <stdint.h>
#include <string.h>
#include "timer.h"
#include "uart.h"
#include "util.h"
#include "api.h"
#include "benchmark.h"
#include "masked_sign.h"
#include "randombytes.h"


extern unsigned int keccak_cc;


void reset_modules_cc() {
    keccak_cc = 0;
}


void print_modules_cc(unsigned int time, unsigned int shift) {
    print_string("Keccak cycles:\t");
    print_u32_int(keccak_cc >> shift);
    print_string("\n");
    time = time - keccak_cc;
    print_string("SW cycles:\t");
    print_u32_int(time >> shift);
    print_string("\n");
    reset_modules_cc();
}


void test() {
    static unsigned char pk__[SPX_PK_BYTES];
    static unsigned char sk__[SPX_SK_BYTES];
    static unsigned char m__[32];
    static unsigned char sm__[SPX_BYTES + 32];
    static unsigned char mout__[SPX_BYTES + 32];
#ifdef MASKING_EN
    unsigned long long smlen__;
    unsigned long long mlen__;
#else
    size_t smlen__;
    size_t mlen__;
#endif
    int ret;

    BENCH_INIT()
    reset_modules_cc();
    BENCH_START()

#ifndef MASKING_EN
    crypto_sign_keypair(pk__, sk__);
#else
    unsigned char seed[3*SPX_N];
    randombytes(seed, 3*SPX_N);
    crypto_sign_seed_keypair_hwmasked(pk__, sk__, seed);
#endif

    BENCH_END(SPX_KEYPAIR)
    print_modules_cc(time, 0);

    (void) pk__;
    (void) sk__;

    BENCH_START()

#ifndef MASKING_EN
    crypto_sign(sm__, &smlen__, m__, 32, sk__);
#else
    crypto_sign_hwmasked(sm__, &smlen__, m__, 32, sk__);
#endif

    BENCH_END(SPX_SIGN)
    print_modules_cc(time, 0);

    (void) sm__;
    (void) smlen__;

    BENCH_START()

#ifndef MASKING_EN
    ret = crypto_sign_open(mout__, &mlen__, sm__, smlen__, pk__);
#else 
    ret = crypto_sign_open_hwmasked(mout__, &mlen__, sm__, smlen__, pk__);
#endif
    (void) ret;
    (void) mout__;
    (void) mlen__;

    BENCH_END(SPX_VERIFY)
    print_modules_cc(time, 0);
}


int main() {
    test();
}
