#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "symmetric.h"
#include "keccak.h"
#include "symmetric.h"
#include "util.h"

void pasta_shake128_stream_init(volatile uint64_t seed, volatile uint64_t nonce, uint8_t poly_ctr)
{
    volatile uint32_t t;

    keccak_init(SHAKE128_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*)&seed, NULL, sizeof(uint64_t) >> 2);
    keccak_absorb((uint32_t*)&nonce, NULL, sizeof(uint64_t) >> 2);
    t = (SHAKE_PAD << 8) | ((uint32_t)poly_ctr);
    keccak_finish((uint32_t*)&t);
}

void pasta_shake128_squeeze(uint8_t *dst, unsigned int dst_len) {
    keccak_squeeze((uint32_t*) dst, NULL, dst_len >> 2);
}

void pasta_shake128_squeezeblocks(uint8_t *dst, unsigned int num_blocks) {
    keccak_squeeze((uint32_t*) dst, NULL, num_blocks*(SHAKE128_RATE >> 2));
}

