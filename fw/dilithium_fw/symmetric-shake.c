#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "symmetric.h"
#include "fips202.h"
#include "keccak.h"
#include "symmetric.h"

void dilithium_shake128_stream_init(const uint8_t seed[SEEDBYTES], uint16_t nonce)
{
    volatile uint32_t t;
    uint8_t nonce_bytes[2] = {nonce & 0xFF, nonce >> 8};
    
    keccak_init(SHAKE128_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*)seed, NULL, SEEDBYTES >> 2);
    t = (SHAKE_PAD << 16) | (((uint32_t)nonce_bytes[1]) << 8) | ((uint32_t)nonce_bytes[0]);
    keccak_finish((uint32_t*)&t);
}



void dilithium_shake256_stream_init(const uint8_t seed[CRHBYTES], uint16_t nonce)
{
    volatile uint32_t t;
    uint8_t nonce_bytes[2] = {nonce & 0xFF, nonce >> 8};
    
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*)seed, NULL, CRHBYTES >> 2);
    t = (SHAKE_PAD << 16) | (((uint32_t)nonce_bytes[1]) << 8) | ((uint32_t)nonce_bytes[0]);
    keccak_finish((uint32_t*)&t);
}


void dilithium_shake128_squeezeblocks(uint8_t *dst, unsigned int num_blocks) {
    keccak_squeeze((uint32_t*) dst, NULL, num_blocks*(SHAKE128_RATE >> 2));
}


