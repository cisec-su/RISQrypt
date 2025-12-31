#ifndef KECCAK_TESTS_H__
#define KECCAK_TESTS_H__

#include <stdint.h>

// SHAKE256 golden test vectors
extern const uint8_t golden_abcdefgh_dil[32];
extern const uint8_t golden_abcd_dil[32];

// Test functions
void test_keccak_simple(void);
void test_keccak_golden(void);
void test_falcon_api(void);

#endif /* KECCAK_TESTS_H__ */