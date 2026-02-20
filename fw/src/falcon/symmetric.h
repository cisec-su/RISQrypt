#ifndef SYMMETRIC_H
#define SYMMETRIC_H

#include <stdint.h>
#include <stddef.h>

#include "params.h"

#define SHAKE256_RATE 136
#define SHAKE_PAD 0x1F
#define STREAM256_BLOCKBYTES SHAKE256_RATE


#define falcon_shake256_init FALCON_NAMESPACE(falcon_shake256_init)
void falcon_shake256_init();

#define falcon_shake256_absorb FALCON_NAMESPACE(falcon_shake256_absorb)
void falcon_shake256_absorb(const uint8_t *src, size_t src_len);

#define falcon_shake256_finish FALCON_NAMESPACE(falcon_shake256_finish)
void falcon_shake256_finish();

#define falcon_shake256_squeeze FALCON_NAMESPACE(falcon_shake256_squeeze)
void falcon_shake256_squeeze(uint8_t *dst, size_t dst_len);

#define falcon_shake256_squeezeblocks FALCON_NAMESPACE(falcon_shake256_squeezeblocks)
void falcon_shake256_squeezeblocks(uint8_t *dst, unsigned int num_blocks);

#endif