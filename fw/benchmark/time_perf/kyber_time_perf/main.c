#include <stdint.h>
#include "string.h"
#include "timer.h"
#include "util.h"
#include "params.h"
#include "indcpa.h"
#include "kem.h"
#include "masked_cbd.h"
#include "masked_poly.h"
#include "masked_indcpa.h"
#include "masked_kem.h"
#include "masked_gadgets.h"
#include "masked_symmetric.h"
#include "x2x.h"
#include "benchmark.h"
#include "timer.h"


#define LOG_TEST_NUM 10
#define TEST_NUM (1 << LOG_TEST_NUM)


void test_masked_cbd() {
    uint32_t buf[MASKING_N][((KYBER_N*KYBER_ETA2*2)/8) >> 2];
    masked_poly r;
    int i;

    BENCH_INIT();

    poly_init_q();
    masked_gadgets_init_q();

    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        masked_cbd_eta2(&r, buf);
        (void) r;
    }

    BENCH_END_SHIFT_COL(MASKED_CBD, LOG_TEST_NUM);
}


void test_masked_cbd_xof() {
    uint32_t buf[MASKING_N][((KYBER_N*KYBER_ETA2*2)/8) >> 2];
    masked_poly r;
    masked_sym coins;
    int i;
    masked_ptr ptr = {(uint8_t*) buf[0], (uint8_t*) buf[1]};

    BENCH_INIT();

    poly_init_q();
    masked_gadgets_init_q();

    BENCH_START();

    masked_prf_absorb(coins, -1);
    for (i = 0; i < TEST_NUM; i++) {
        masked_prf_squeeze(ptr, sizeof(buf) / MASKING_N);
        masked_prf_absorb(coins, i);
        masked_cbd_eta2(&r, buf);
        (void) r;
    }

    BENCH_END_SHIFT_COL(MASKED_CBD_XOF, LOG_TEST_NUM);
}


void test_masked_poly_from_msg() {
    masked_msg mm;
    masked_poly r;
    int i;

    BENCH_INIT();

    poly_init_q();
    masked_gadgets_init_q();

    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        masked_poly_frommsg(&r, mm);
        (void) r;
    }

    BENCH_END_SHIFT_COL(MASKED_POLY_FROM_MSG, LOG_TEST_NUM);
}


void test_masked_poly_to_msg() {
    masked_msg mm;
    masked_poly r;
    int i;

    BENCH_INIT();

    poly_init_q();

    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        masked_poly_tomsg(mm, &r);
        (void) mm;
    }

    BENCH_END_SHIFT_COL(MASKED_POLY_TO_MSG, LOG_TEST_NUM);
}


void test_masked_sub_compress() {
    masked_msg mm;
    masked_poly_u32 r;
    masked_poly a;
    uint8_t b[KYBER_POLYCOMPRESSEDBYTES];
    int i;

    BENCH_INIT();
    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        masked_poly_sub_compress(&r, &a, b);
        (void) r;
    }

    BENCH_END_SHIFT_COL(MASKED_POLY_SUB_COMPRESS, LOG_TEST_NUM);
}


void test_masked_zero_test_vec() {
    masked_poly_u32 mpu32;
    masked_polyvec_u32 mpvu32;
    masked_u32 t0;
    int i;

    BENCH_INIT();
    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        masked_gadgets_zero_test_vec(t0, &mpvu32, &mpu32);
        (void) t0;
    }

    BENCH_END_SHIFT_COL(MASKED_ZERO_TEST_VEC, LOG_TEST_NUM);
}


void test_indcpa_api() {
    static uint8_t pk_cpa[KYBER_INDCPA_PUBLICKEYBYTES];
    static uint8_t sk_cpa[KYBER_INDCPA_SECRETKEYBYTES];
    static uint8_t coins[KYBER_SYMBYTES];
    static uint8_t m[KYBER_SYMBYTES] = {0x00};
    static uint8_t c_cpa[KYBER_INDCPA_BYTES];
    static uint8_t m_[KYBER_SYMBYTES];
    static masked_msg mm;
    static masked_sym masked_coins;
    int i, ret;

    BENCH_INIT();
    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        indcpa_keypair(pk_cpa, sk_cpa);
        (void) pk_cpa;
        (void) sk_cpa;
    }

    BENCH_END_SHIFT_COL(INDCPA_KEYPAIR, LOG_TEST_NUM);


    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        m[0] += 1;
        indcpa_enc(c_cpa, m, pk_cpa, coins);
        (void) c_cpa;
        (void) m;
    }

    BENCH_END_SHIFT_COL(INDCPA_ENC, LOG_TEST_NUM);


    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        c_cpa[0] += 1;
        indcpa_dec(m, c_cpa, sk_cpa);
        (void) m;
    }

    BENCH_END_SHIFT_COL(INDCPA_DEC, LOG_TEST_NUM);

    
    BENCH_START();

    for (int i = 0; i < TEST_NUM; i++) {
        c_cpa[0] += 1;
        masked_indcpa_dec(mm, c_cpa, sk_cpa);
        (void) mm;
    }

    BENCH_END_SHIFT_COL(MASKED_INDCPA_DEC, LOG_TEST_NUM);


    BENCH_START();

    for (int i = 0; i < TEST_NUM; i++) {
        c_cpa[0] += 1;
        ret = masked_indcpa_enc_cmp(c_cpa, mm, pk_cpa, masked_coins);
        (void) ret;
    }

    BENCH_END_SHIFT_COL(MASKED_INDCPA_ENCCMP, LOG_TEST_NUM);

}



void test_indcca_api() {
    static uint8_t pk_cca[KYBER_PUBLICKEYBYTES];
    static uint8_t sk_cca[KYBER_SECRETKEYBYTES];
    static uint8_t K[KYBER_SSBYTES];
    static uint8_t c_cca[KYBER_CIPHERTEXTBYTES];
    static uint8_t KK[KYBER_SSBYTES];
    int i;

    BENCH_INIT();
    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        crypto_kem_keypair(pk_cca, sk_cca);
        (void) pk_cca;
        (void) sk_cca;
    }

    BENCH_END_SHIFT_COL(KEM_KEYPAIR, LOG_TEST_NUM);


    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        crypto_kem_enc(c_cca, K, pk_cca);
        (void) c_cca;
        (void) K;
    }

    BENCH_END_SHIFT_COL(KEM_ENC, LOG_TEST_NUM);


    BENCH_START();

    for (i = 0; i < TEST_NUM; i++) {
        crypto_kem_dec(KK, c_cca, sk_cca);
        (void) KK;
    }

    BENCH_END_SHIFT_COL(KEM_DEC, LOG_TEST_NUM);

    
    BENCH_START();

    for (int i = 0; i < TEST_NUM; i++) {
        masked_crypto_kem_dec(KK, c_cca, sk_cca);
        (void) KK;
    }

    BENCH_END_SHIFT_COL(MASKED_KEM_DEC, LOG_TEST_NUM);
}


int main() {
    uint32_t seed[2] = {5, 2};
    x2x_seed(seed);

    BENCH_LINE();
    BENCH_LINE();
    BENCH_TITLE("Kyber");
    BENCH_LINE();
    BENCH_LINE();
    BENCH_HEADER();
    BENCH_LINE();

    test_indcca_api();
    BENCH_LINE();
    test_indcpa_api();
    BENCH_LINE();
    test_masked_cbd();
    test_masked_cbd_xof();
    test_masked_poly_from_msg();
    test_masked_poly_to_msg();
    test_masked_sub_compress();
    test_masked_zero_test_vec();

    BENCH_LINE();
    BENCH_LINE();
    print_string("Finished\n");
}