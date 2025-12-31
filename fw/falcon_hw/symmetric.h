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

#define SEEDBYTES 32
#define CRHBYTES 64

typedef void stream128_state;
typedef void stream256_state;


#define falcon_shake128_stream_init FALCON_NAMESPACE(falcon_shake128_stream_init)
void falcon_shake128_stream_init(const uint8_t seed[SEEDBYTES], uint16_t nonce);

#define falcon_shake256_stream_init FALCON_NAMESPACE(falcon_shake256_stream_init)
void falcon_shake256_stream_init(const uint8_t seed[CRHBYTES], uint16_t nonce);

#define falcon_shake256_stream_init_seed FALCON_NAMESPACE(falcon_shake256_stream_init_seed)
void falcon_shake256_stream_init_seed(const uint8_t seed[SEEDBYTES]);

#define stream128_init(SEED, NONCE) falcon_shake128_stream_init(SEED, NONCE)
#define stream256_init(SEED, NONCE) falcon_shake256_stream_init(SEED, NONCE)


#define stream128_squeezeblocks(OUT, OUTBLOCKS) \
        falcon_shake128_squeezeblocks(OUT, OUTBLOCKS)
void falcon_shake128_squeezeblocks(uint8_t *dst, unsigned int num_blocks);

#define stream256_squeezeblocks(OUT, OUTBLOCKS) \
        falcon_shake256_squeezeblocks(OUT, OUTBLOCKS)
void falcon_shake256_squeezeblocks(uint8_t *dst, unsigned int num_blocks);

void falcon_shake256(uint8_t *dst, size_t dst_len, const uint8_t *src, size_t src_len);
void falcon_shake256_nonce(uint8_t *dst, size_t dst_len, const uint8_t *src, size_t src_len, uint16_t nonce);
void falcon_shake256_challenge(uint8_t *dst, const uint8_t *mu, const uint8_t *w1packed);
void falcon_shake256_mu_crh(uint8_t *mu, const uint8_t *pk, const uint8_t *m, size_t mlen);
void falcon_shake256_absorb_double(uint8_t *dst, size_t dst_len, const uint8_t *src0, size_t src0_len, const uint8_t *src1, size_t src1_len);

#endif