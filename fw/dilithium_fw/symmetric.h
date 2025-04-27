#ifndef SYMMETRIC_H
#define SYMMETRIC_H

#include <stdint.h>
#include "params.h"

#define SHAKE128_RATE 168
#define SHAKE256_RATE 136
#define SHAKE_PAD 0x1F
#define STREAM128_BLOCKBYTES SHAKE128_RATE
#define STREAM256_BLOCKBYTES SHAKE256_RATE

typedef void stream128_state;
typedef void stream256_state;

typedef struct {
        // Empty struct because keccak HW handles everything
        int dummy;
      } shake256incctx;
      
#define dilithium_shake128_stream_init DILITHIUM_NAMESPACE(dilithium_shake128_stream_init)
void dilithium_shake128_stream_init(const uint8_t seed[SEEDBYTES], uint16_t nonce);

#define dilithium_shake256_stream_init DILITHIUM_NAMESPACE(dilithium_shake256_stream_init)
void dilithium_shake256_stream_init(const uint8_t seed[CRHBYTES], uint16_t nonce);

#define stream128_init(SEED, NONCE) dilithium_shake128_stream_init(SEED, NONCE)
#define stream256_init(SEED, NONCE) dilithium_shake256_stream_init(SEED, NONCE)

#define stream128_squeezeblocks(OUT, OUTBLOCKS) \
        keccak_squeeze((uint32_t*)OUT, NULL, (OUTBLOCKS)*(SHAKE128_RATE/4))

#define stream256_squeezeblocks(OUT, OUTBLOCKS) \
        keccak_squeeze((uint32_t*)OUT, NULL, (OUTBLOCKS)*(SHAKE256_RATE/4))

#endif