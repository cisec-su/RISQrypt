#include <stdint.h>
#include <string.h>
#include "timer.h"
#include "uart.h"
#include "util.h"
#include "api.h"
#include "benchmark.h"


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
    unsigned long long smlen__;
    unsigned long long mlen__;
    int ret;

    BENCH_INIT()
    reset_modules_cc();
    BENCH_START()

    crypto_sign_keypair(pk__, sk__);

    BENCH_END(SPX_KEYPAIR)
    print_modules_cc(time, 0);

    (void) pk__;
    (void) sk__;

    BENCH_START()

    crypto_sign(sm__, &smlen__, m__, 32, sk__);

    BENCH_END(SPX_SIGN)
    print_modules_cc(time, 0);

    (void) sm__;
    (void) smlen__;

    BENCH_START()

    ret = crypto_sign_open(mout__, &mlen__, sm__, smlen__, pk__);
    (void) ret;
    (void) mout__;
    (void) mlen__;

    BENCH_END(SPX_VERIFY)
    print_modules_cc(time, 0);
}


int main() {
    test();
}
