#pragma once
#include <stddef.h>
#include <stdint.h>

void test_rng_seed_bytes(const uint8_t *seed, size_t len);

void test_rng_seed_hex(const char *hex);

void test_rng_seed_case(uint32_t suite_id, uint32_t round_id);

int randombytes(unsigned char *out, unsigned long long outlen);

