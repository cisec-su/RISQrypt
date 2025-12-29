#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "symmetric.h"
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
    keccak_finish((uint32_t*) &t);
}


void dilithium_shake256_stream_init_seed(const uint8_t seed[SEEDBYTES])
{
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*) seed, NULL, SEEDBYTES >> 2);
    t = SHAKE_PAD;
    keccak_finish((uint32_t*) &t);
}


void dilithium_shake128_squeezeblocks(uint8_t *dst, unsigned int num_blocks) {
    keccak_squeeze((uint32_t*) dst, NULL, num_blocks*(SHAKE128_RATE >> 2));
}

void dilithium_shake256_squeezeblocks(uint8_t *dst, unsigned int num_blocks) {
    keccak_squeeze((uint32_t*) dst, NULL, num_blocks*(SHAKE256_RATE >> 2));
}

void dilithium_shake256_challenge(uint8_t *dst, const uint8_t *mu, const uint8_t *w1packed) {
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    // mu || w1packed input to keccak
    keccak_absorb((uint32_t*)mu, NULL, CRHBYTES >> 2);
    keccak_absorb((uint32_t*)w1packed, NULL, (K*POLYW1_PACKEDBYTES ) >> 2);
    t = SHAKE_PAD;
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*)dst, NULL, (SEEDBYTES ) >> 2);
}

void dilithium_shake256(uint8_t *dst, size_t dst_len, const uint8_t *src, size_t src_len) {
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*)src, NULL, (src_len  ) >> 2);
    t = SHAKE_PAD;
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*)dst, NULL, (dst_len) >> 2);
}

void dilithium_shake256_nonce(uint8_t *dst, size_t dst_len, const uint8_t *src, size_t src_len, uint16_t nonce) {
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*)src, NULL, (src_len) >> 2);
    t = (SHAKE_PAD << 16) | ((uint32_t) nonce);
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*)dst, NULL, (dst_len) >> 2);
}


void dilithium_shake256_mu_crh(uint8_t *mu, const uint8_t *pk, const uint8_t *m, size_t mlen) {
    uint8_t inner_hash[SEEDBYTES];
    volatile uint32_t t;
    // SHAKE256(pk) = inner_hash
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*)pk, NULL, (CRYPTO_PUBLICKEYBYTES) >> 2);
    t = SHAKE_PAD;
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*)inner_hash, NULL, (SEEDBYTES) >> 2);

    // SHAKE256(inner_hash || m) = mu
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*)inner_hash, NULL, (SEEDBYTES) >> 2);
    keccak_absorb((uint32_t*)m, NULL, (mlen) >> 2);
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*)mu, NULL, (CRHBYTES) >> 2);
}

void dilithium_shake256_absorb_double(uint8_t *dst, size_t dst_len, const uint8_t *src0, size_t src0_len, const uint8_t *src1, size_t src1_len) {
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*)src0, NULL, (src0_len) >> 2);
    keccak_absorb((uint32_t*)src1, NULL, (src1_len) >> 2);
    t = SHAKE_PAD;
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*)dst, NULL, (dst_len) >> 2);
}