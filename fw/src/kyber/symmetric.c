#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "keccak.h"
#include "symmetric.h"




void kyber_shake128_init() {
    keccak_init(SHAKE128_RATE >> 3, KECCAK_MASK_DIS);
}


/*************************************************
* Name:        kyber_shake128_absorb
*
* Description: Absorb step of the SHAKE128 specialized for the Kyber context.
*
* Arguments:   - keccak_state *state: pointer to (uninitialized) output
*                                     Keccak state
*              - const uint8_t *seed: pointer to KYBER_SYMBYTES input
*                                     to be absorbed into state
*              - uint8_t i            additional byte of input
*              - uint8_t j            additional byte of input
**************************************************/
void kyber_shake128_absorb(const uint8_t seed[KYBER_SYMBYTES], uint8_t x, uint8_t y) {
    volatile uint32_t t;
    keccak_init(SHAKE128_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*) seed, NULL, KYBER_SYMBYTES >> 2);
    t = (SHAKE_PAD << 16) | (((uint32_t) y) << 8) | ((uint32_t) x);
    keccak_finish((uint32_t*) &t);
}


void kyber_shake128_squeeze(uint8_t *dst, size_t dst_len)
{
    keccak_squeeze((uint32_t*) dst, NULL, dst_len >> 2);
}


void kyber_shake128_squeezeblocks(uint8_t *dst, unsigned int num_blocks) {
    keccak_squeeze((uint32_t*) dst, NULL, num_blocks*(SHAKE128_RATE >> 2));
}


void kyber_shake256_prf_absorb(const uint8_t key[KYBER_SYMBYTES], uint8_t nonce)
{
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*) key, NULL, KYBER_SYMBYTES >> 2);
    t = (SHAKE_PAD << 8) | ((uint32_t) nonce);
    keccak_finish((uint32_t*) &t);
}


void kyber_shake256_prf_squeeze(uint8_t *dst, size_t dst_len)
{
    keccak_squeeze((uint32_t*) dst, NULL, dst_len >> 2);
}

/*************************************************
* Name:        kyber_shake256_prf
*
* Description: Usage of SHAKE256 as a PRF, concatenates secret and public input
*              and then generates dst_len bytes of SHAKE256 output
*
* Arguments:   - uint8_t *dst:       pointer to output
*              - size_t dst_len:      number of requested output bytes
*              - const uint8_t *key: pointer to the key
*                                    (of length KYBER_SYMBYTES)
*              - uint8_t nonce:      single-byte nonce (public PRF input)
**************************************************/
void kyber_shake256_prf(uint8_t *dst, size_t dst_len, const uint8_t key[KYBER_SYMBYTES], uint8_t nonce)
{
    kyber_shake256_prf_absorb(key, nonce);
    kyber_shake256_prf_squeeze(dst, dst_len);
}


void shake256(uint8_t *dst, size_t dst_len, const uint8_t *src, size_t src_len)
{
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*) src, NULL, src_len >> 2);
    t = SHAKE_PAD;
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*) dst, NULL, dst_len >> 2);
}


void sha3_256(uint8_t *dst, const uint8_t *src, size_t len) {
    volatile uint32_t t;
    keccak_init(SHA3_256_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*) src, NULL, len >> 2);
    t = SHA3_PAD;
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*) dst, NULL, SHA3_256_HASH_SIZE >> 2);
}



void sha3_512(uint8_t *dst, const uint8_t *src, size_t len) {
    volatile uint32_t t;
    keccak_init(SHA3_512_RATE >> 3, KECCAK_MASK_DIS);
    keccak_absorb((uint32_t*) src, NULL, len >> 2);
    t = SHA3_PAD;
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*) dst, NULL, SHA3_512_HASH_SIZE >> 2);
}


void sha3_512_init() {
    keccak_init(SHA3_512_RATE >> 3, KECCAK_MASK_DIS);
}


void keccak_core(const uint8_t *src, size_t len) {
    keccak_absorb((uint32_t*) src, NULL, len >> 2);
}


void sha3_512_finish(uint8_t *dst) {
    volatile uint32_t t;
    t = SHA3_PAD;
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*) dst, NULL, SHA3_512_HASH_SIZE >> 2);
}
