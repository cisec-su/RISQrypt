#include <stdio.h>
#include <stdint.h>
#include "x2x.h"
#include "sign.h"
#include "masked_sign.h"
#include "util.h"
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
    size_t sig_len;
    int ret;
    void *pk__;
    void *sk__;
    void *sig__;
    uint8_t *msg__;
    uint32_t msg_len__ = 32;
    unsigned int log_test_num = 7;
    unsigned int test_num = 1 << log_test_num;

    BENCH_INIT()
    reset_modules_cc(); 
    BENCH_START()

    crypto_sign_keypair(pk__, sk__);
    (void) pk__;
    (void) sk__;

    BENCH_END(SIGN_KEYPAIR)
    print_modules_cc(time, 0);

    BENCH_START()
    for (int i = 0; i < test_num; i++) {
        msg__[0] += 1;
#ifdef MASKING_EN
        masked_crypto_sign_signature(sig__, &sig_len, msg__, msg_len__, sk__);
#else
        crypto_sign_signature(sig__, &sig_len, msg__, msg_len__, sk__);
#endif
    }
    (void) sig__;
    (void) msg__;
    (void) sig_len;

    BENCH_END_SHIFT(SIGN_SIGNATURE, log_test_num)
    print_modules_cc(time, log_test_num);

    BENCH_START() 

    ret = crypto_sign_verify(sig__, CRYPTO_BYTES, msg__, msg_len__, pk__);
    (void) ret;

    BENCH_END(SIGN_VERIFY)
    print_modules_cc(time, 0);
}


int main() {
#ifdef MASKING_EN
    uint32_t seed[2] = {1, 1};
    x2x_seed(seed);
#endif
    test();
}