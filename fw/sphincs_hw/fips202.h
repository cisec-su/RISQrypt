#ifndef SPX_FIPS202_H
#define SPX_FIPS202_H

#include <stddef.h>
#include <stdint.h>

#define SHAKE_PAD 0x1F
#define SHAKE128_RATE 168
#define SHAKE256_RATE 136
#define SHA3_256_RATE 136
#define SHA3_512_RATE 72

void shake128_absorb(uint32_t *s, const uint8_t *input, size_t inlen);

void shake128_squeezeblocks(uint8_t *output, size_t nblocks, uint32_t *s);

void shake128_inc_init(uint32_t *s_inc);
void shake128_inc_absorb(uint32_t *s_inc, const uint8_t *input, size_t inlen);
void shake128_inc_finalize(uint32_t *s_inc);
void shake128_inc_squeeze(uint8_t *output, size_t outlen, uint32_t *s_inc);

void shake256_absorb(uint32_t *s, const uint8_t *input, size_t inlen);
void shake256_squeezeblocks(uint8_t *output, size_t nblocks, uint32_t *s);

void shake256_inc_init(uint32_t *s_inc);
void shake256_inc_absorb(uint32_t *s_inc, const uint8_t *input, size_t inlen);
void shake256_inc_finalize(uint32_t *s_inc);
void shake256_inc_squeeze(uint8_t *output, size_t outlen, uint32_t *s_inc);

void shake128(uint8_t *output, size_t outlen,
              const uint8_t *input, size_t inlen);

void shake256(uint8_t *output, size_t outlen,
              const uint8_t *input, size_t inlen);

#endif
