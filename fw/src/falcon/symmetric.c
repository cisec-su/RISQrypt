#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "symmetric.h"
#include "keccak.h"
#include "symmetric.h"



void falcon_shake256_init() {
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
}


void falcon_shake256_absorb(const uint8_t *src, size_t src_len) {
    keccak_absorb((uint32_t*)src, NULL, (src_len  ) >> 2);
}


void falcon_shake256_finish() {
    volatile uint32_t t = SHAKE_PAD;
    keccak_finish((uint32_t*)&t);
}


void falcon_shake256_squeeze(uint8_t *dst, size_t dst_len) {
    keccak_squeeze((uint32_t*)dst, NULL, dst_len >> 2);
}


void falcon_shake256_squeezeblocks(uint8_t *dst, unsigned int num_blocks) {
    keccak_squeeze((uint32_t*) dst, NULL, num_blocks*(SHAKE256_RATE >> 2));
}

