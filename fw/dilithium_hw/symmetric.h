#ifndef SYMMETRIC_H
#define SYMMETRIC_H

#include <stdint.h>
#include <stddef.h>

#include "params.h"

#define SHAKE128_RATE 168
#define SHAKE256_RATE 136
#define SHAKE_PAD 0x1F
#define STREAM128_BLOCKBYTES SHAKE128_RATE
#define STREAM256_BLOCKBYTES SHAKE256_RATE

typedef void stream128_state;
typedef void stream256_state;


#define dilithium_shake128_stream_init DILITHIUM_NAMESPACE(dilithium_shake128_stream_init)
void dilithium_shake128_stream_init(const uint8_t seed[SEEDBYTES], uint16_t nonce);

#define dilithium_shake256_stream_init DILITHIUM_NAMESPACE(dilithium_shake256_stream_init)
void dilithium_shake256_stream_init(const uint8_t seed[CRHBYTES], uint16_t nonce);

#define stream128_init(SEED, NONCE) dilithium_shake128_stream_init(SEED, NONCE)
#define stream256_init(SEED, NONCE) dilithium_shake256_stream_init(SEED, NONCE)

#define stream128_squeezeblocks(OUT, OUTBLOCKS) \
        dilithium_shake128_squeezeblocks(OUT, OUTBLOCKS)
void dilithium_shake128_squeezeblocks(uint8_t *dst, unsigned int num_blocks);

#define stream256_squeezeblocks(OUT, OUTBLOCKS) \
        dilithium_shake256_squeezeblocks(OUT, OUTBLOCKS)
void dilithium_shake256_squeezeblocks(uint8_t *dst, unsigned int num_blocks);

void dilithium_shake256(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen);
void dilithium_shake256_challenge(uint8_t *out, const uint8_t *mu, const uint8_t *w1packed);
void dilithium_shake256_mu_crh(uint8_t *mu, const uint8_t *pk, const uint8_t *m, size_t mlen);
void dilithium_shake256_absorb_double(uint8_t *out, size_t outlen, const uint8_t *in1, size_t in1len, const uint8_t *in2, size_t in2len);

#endif