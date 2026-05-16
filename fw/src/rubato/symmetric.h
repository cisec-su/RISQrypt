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

void rubato_shake128_stream_init(uint64_t seed, uint64_t nonce, uint8_t poly_ctr);
void rubato_shake128_squeeze(uint8_t *dst, unsigned int dst_len);
void rubato_shake128_squeezeblocks(uint8_t *dst, unsigned int num_blocks);

void rubato_shake256_stream_init(uint64_t seed, uint64_t nonce, uint8_t poly_ctr);
void rubato_shake256_squeeze(uint8_t *dst, unsigned int dst_len);
void rubato_shake256_squeezeblocks(uint8_t *dst, unsigned int num_blocks);

#define stream128_init(SEED, NONCE, POLY_CTR)   rubato_shake128_stream_init(SEED, NONCE, POLY_CTR)
#define stream128_squeezeblocks(OUT, OUTBLOCKS) rubato_shake128_squeezeblocks(OUT, OUTBLOCKS)
#define stream128_squeeze(OUT, OUT_LEN)         rubato_shake128_squeeze(OUT, OUT_LEN)

#define stream256_init(SEED, NONCE, POLY_CTR)   rubato_shake256_stream_init(SEED, NONCE, POLY_CTR)
#define stream256_squeezeblocks(OUT, OUTBLOCKS) rubato_shake256_squeezeblocks(OUT, OUTBLOCKS)
#define stream256_squeeze(OUT, OUT_LEN)         rubato_shake256_squeeze(OUT, OUT_LEN)

/* Generic XOF stream — resolved by XOF_SHAKE128 / XOF_SHAKE256 in params.h */
#if defined(XOF_SHAKE128)
#  define RUBATO_STREAM_BLOCKBYTES   STREAM128_BLOCKBYTES
#  define rubato_stream_init         stream128_init
#  define rubato_stream_squeezeblocks stream128_squeezeblocks
#  define rubato_stream_squeeze      stream128_squeeze
#elif defined(XOF_SHAKE256)
#  define RUBATO_STREAM_BLOCKBYTES   STREAM256_BLOCKBYTES
#  define rubato_stream_init         stream256_init
#  define rubato_stream_squeezeblocks stream256_squeezeblocks
#  define rubato_stream_squeeze      stream256_squeeze
#else
#  error "No XOF (XOF_SHAKE128 or XOF_SHAKE256) selected in params.h"
#endif

#endif