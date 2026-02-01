#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "unity.h"

#include "test_rng.h"
#include "kem.h"

/* Kyber params (from params.h via kem.h -> params.h) */
#ifndef KYBER_SSBYTES
#error "KYBER_SSBYTES is not defined. Check includes/params.h."
#endif
#ifndef KYBER_PUBLICKEYBYTES
#error "KYBER_PUBLICKEYBYTES is not defined. Check includes/params.h."
#endif
#ifndef KYBER_SECRETKEYBYTES
#error "KYBER_SECRETKEYBYTES is not defined. Check includes/params.h."
#endif
#ifndef KYBER_CIPHERTEXTBYTES
#error "KYBER_CIPHERTEXTBYTES is not defined. Check includes/params.h."
#endif


static void test_kem_keypair(void)
{
    ALIGN(32) uint8_t pk[KYBER_PUBLICKEYBYTES];
    ALIGN(32) uint8_t sk[KYBER_SECRETKEYBYTES];

    int ret = crypto_kem_keypair(pk, sk);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "crypto_kem_keypair failed");
}

static void test_kem_enc_dec(void)
{
    ALIGN(32) uint8_t pk[KYBER_PUBLICKEYBYTES];
    ALIGN(32) uint8_t sk[KYBER_SECRETKEYBYTES];
    ALIGN(32) uint8_t ct[KYBER_CIPHERTEXTBYTES];

    ALIGN(32) uint8_t ss1[KYBER_SSBYTES];
    ALIGN(32) uint8_t ss2[KYBER_SSBYTES];

    int ret;

    ret = crypto_kem_keypair(pk, sk);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "crypto_kem_keypair failed");

    ret = crypto_kem_enc(ct, ss1, pk);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "crypto_kem_enc failed");

    ret = crypto_kem_dec(ss2, ct, sk);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "crypto_kem_dec failed");

    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(ss1, ss2, KYBER_SSBYTES,
                                         "Shared secrets do not match");
}

static void test_kem_dec_fail_on_modified_ct(void)
{
    ALIGN(32) uint8_t pk[KYBER_PUBLICKEYBYTES];
    ALIGN(32) uint8_t sk[KYBER_SECRETKEYBYTES];
    ALIGN(32) uint8_t ct[KYBER_CIPHERTEXTBYTES];

    ALIGN(32) uint8_t ss1[KYBER_SSBYTES];
    ALIGN(32) uint8_t ss2[KYBER_SSBYTES];

    int ret;

    ret = crypto_kem_keypair(pk, sk);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "crypto_kem_keypair failed");

    ret = crypto_kem_enc(ct, ss1, pk);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "crypto_kem_enc failed");

    /* Corrupt ciphertext */
    ct[0] ^= 0x01;

    ret = crypto_kem_dec(ss2, ct, sk);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "crypto_kem_dec failed (return code)");

    /* For CCA-secure KEM, decapsulation on invalid ct should not match ss1 */
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, memcmp(ss1, ss2, KYBER_SSBYTES),
                                  "Decapsulation did not change SS on invalid ciphertext");
}

int test_kem_suite(void)
{

    RUN_TEST(test_kem_keypair);
    RUN_TEST(test_kem_enc_dec);
    RUN_TEST(test_kem_dec_fail_on_modified_ct);

    return UNITY_END();
}
