#ifndef SYMMETRIC_H
#define SYMMETRIC_H

#include <stdint.h>
#include <stddef.h>

#include "params.h"

#define SHAKE128_RATE 168
#define SHAKE_PAD 0x1F
#define STREAM128_BLOCKBYTES SHAKE128_RATE

typedef void stream128_state;

void pasta_shake128_stream_init(volatile uint64_t seed, volatile uint64_t nonce, uint8_t poly_ctr);

void pasta_shake128_squeeze(uint8_t *dst, unsigned int dst_len);
void pasta_shake128_squeezeblocks(uint8_t *dst, unsigned int num_blocks);

#define stream128_init(SEED, NONCE, POLY_CTR) pasta_shake128_stream_init(SEED, NONCE, POLY_CTR)

#define stream128_squeezeblocks(OUT, OUTBLOCKS) \
        pasta_shake128_squeezeblocks(OUT, OUTBLOCKS)

#define stream128_squeeze(OUT, OUT_LEN) \
        pasta_shake128_squeeze(OUT, OUT_LEN)

#endif