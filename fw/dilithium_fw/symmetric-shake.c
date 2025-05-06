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

void dilithium_shake256_squeezeblocks(uint8_t *dst, unsigned int num_blocks) {
    keccak_squeeze((uint32_t*) dst, NULL, num_blocks*(SHAKE256_RATE >> 2));
}

void dilithium_shake256_challenge(uint8_t *out, const uint8_t *mu, const uint8_t *w1packed) {
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    // mu || w1packed input to keccak
    keccak_absorb((uint32_t*)mu, NULL, CRHBYTES >> 2);
    keccak_absorb((uint32_t*)w1packed, NULL, (K*POLYW1_PACKEDBYTES + 3) >> 2);

    keccak_finish(KECCAK_NULL_PAD_WORD);
    keccak_squeeze((uint32_t*)out, NULL, (SEEDBYTES + 3) >> 2);
}

void dilithium_shake256(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen) {
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*)in, NULL, (inlen + 3) >> 2);
    keccak_finish(KECCAK_NULL_PAD_WORD);
    keccak_squeeze((uint32_t*)out, NULL, (outlen + 3) >> 2);
}

void dilithium_shake256_mu_crh(uint8_t *mu, const uint8_t *pk, const uint8_t *m, size_t mlen) {
    uint8_t inner_hash[SEEDBYTES];

    // SHAKE256(pk) = inner_hash
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*)pk, NULL, (CRYPTO_PUBLICKEYBYTES + 3) >> 2);
    keccak_finish(KECCAK_NULL_PAD_WORD);
    keccak_squeeze((uint32_t*)inner_hash, NULL, (SEEDBYTES + 3) >> 2);

    // SHAKE256(inner_hash || m) = mu
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*)inner_hash, NULL, (SEEDBYTES + 3) >> 2);
    keccak_absorb((uint32_t*)m, NULL, (mlen + 3) >> 2);
    keccak_finish(KECCAK_NULL_PAD_WORD);
    keccak_squeeze((uint32_t*)mu, NULL, (CRHBYTES + 3) >> 2);
}

void dilithium_shake256_dualinput(uint8_t *out, size_t outlen,
    const uint8_t *in1, size_t in1len,
    const uint8_t *in2, size_t in2len) {
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*)in1, NULL, (in1len + 3) >> 2);
    keccak_absorb((uint32_t*)in2, NULL, (in2len + 3) >> 2);
    keccak_finish(KECCAK_NULL_PAD_WORD);
    keccak_squeeze((uint32_t*)out, NULL, (outlen + 3) >> 2);
}
