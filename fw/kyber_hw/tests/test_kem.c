#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "fw/kyber_hw/params.h"
#include "fw/kyber_hw/tests/support/test_rng.h"

#ifdef __has_include
#  if __has_include("test_rng.h")
#    include "test_rng.h"
#    define HAVE_TEST_RNG 1
#  endif
#endif
#ifndef HAVE_TEST_RNG
static inline void test_rng_seed_hex(const char *hex) { (void)hex; }
static inline void test_rng_seed_case(int suite, const char *tag) { (void)suite; (void)tag; }
#endif

// ---------- tiny assert helpers (no external framework required)
#ifndef USE_UNITY
static int g_failures = 0;

#define TEST_ASSERT_EQUAL_INT(exp, act) do {     \
  if ((int)(exp) != (int)(act)) {                \
    fprintf(stderr, "[ASSERT] %s:%d expected %d got %d\n", \
            __FILE__, __LINE__, (int)(exp), (int)(act));   \
    g_failures++;                                \
  }                                              \
} while (0)

#define TEST_ASSERT_FALSE_MESSAGE(cond, msg) do {  \
  if ((cond)) {                                    \
    fprintf(stderr, "[ASSERT] %s:%d %s\n", __FILE__, __LINE__, (msg)); \
    g_failures++;                                  \
  }                                                \
} while (0)

static void TEST_ASSERT_EQUAL_UINT8_ARRAY(const uint8_t *a, const uint8_t *b, size_t n) {
  if (memcmp(a, b, n) != 0) {
    fprintf(stderr, "[ASSERT] %s:%d buffers differ\n", __FILE__, __LINE__);
    g_failures++;
  }
}
#endif

// ---------- test configuration
#ifndef KEM_TEST_ROUNDS
#define KEM_TEST_ROUNDS 8
#endif

// Accept all supported parameter sets in your repo.
// (Default KYBER_K in many trees is 3 (Kyber-768).)
#if (KYBER_K != 2) && (KYBER_K != 3) && (KYBER_K != 4)
#error "Unsupported KYBER_K. Build with -DKYBER_K=2, -DKYBER_K=3, or -DKYBER_K=4."
#endif

enum { SUITE_BASIC = 1, SUITE_STRESS = 2 };

// ---------- one authenticated round-trip with optional RNG seed tag
static void kem_roundtrip_once(uint32_t round_seed_tag) {
  uint8_t pk[KYBER_PUBLICKEYBYTES];
  uint8_t sk[KYBER_SECRETKEYBYTES];
  uint8_t ct_ok[KYBER_CIPHERTEXTBYTES];
  uint8_t ss1[KYBER_SSBYTES];
  uint8_t ss2[KYBER_SSBYTES];

  // deterministic seeding if available
#ifdef HAVE_TEST_RNG
  test_rng_seed_case(SUITE_BASIC, ""); // high-level case gate
#endif
  (void)round_seed_tag;

  TEST_ASSERT_EQUAL_INT(0, crypto_kem_keypair(pk, sk));
  TEST_ASSERT_EQUAL_INT(0, crypto_kem_enc(ct_ok, ss1, pk));
  TEST_ASSERT_EQUAL_INT(0, crypto_kem_dec(ss2, ct_ok, sk));
  TEST_ASSERT_EQUAL_UINT8_ARRAY(ss1, ss2, KYBER_SSBYTES);

  // Negative test: flip 1 bit in ciphertext; dec must NOT yield the same ss
  uint8_t ct_corrupt[KYBER_CIPHERTEXTBYTES];
  memcpy(ct_corrupt, ct_ok, sizeof ct_corrupt);
  ct_corrupt[0] ^= 0x81;

  uint8_t ss_bad[KYBER_SSBYTES];
  (void)crypto_kem_dec(ss_bad, ct_corrupt, sk);
  TEST_ASSERT_FALSE_MESSAGE(
      memcmp(ss_bad, ss1, KYBER_SSBYTES) == 0,
      "Corrupted CT produced a valid shared secret");
}

// ---------- basic test (single round with seed hook)
static void test_kem_basic(void) {
#ifdef HAVE_TEST_RNG
  test_rng_seed_case(SUITE_BASIC, "0");
#endif
  kem_roundtrip_once(0);
}

// ---------- stress test (multiple rounds)
static void test_kem_stress(void) {
#ifdef HAVE_TEST_RNG
  test_rng_seed_case(SUITE_STRESS, "seeded");
#endif
  for (uint32_t i = 0; i < (uint32_t)KEM_TEST_ROUNDS; i++) {
    kem_roundtrip_once(i + 1);
  }
}

// ---------- runner
static int test_kem_suite(void) {
  test_kem_basic();
  test_kem_stress();
#ifndef USE_UNITY
  if (g_failures) {
    fprintf(stderr, "TESTS FAILED: %d failure(s)\n", g_failures);
    return 1;
  }
  printf("All KEM tests passed.\n");
#endif
  return 0;
}

#ifndef TEST_STANDALONE
int main(void) {
  return test_kem_suite();
}
#endif
