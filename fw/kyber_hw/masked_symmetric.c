#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "keccak.h"
#include "masked_symmetric.h"


#if MASKING_N != 2
#error "Masked hash functions do not support MASKING_N > 2"
#endif


void kyber_masked_shake256_prf_absorb(const masked_sym src, uint8_t nonce)
{
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_EN);
    keccak_absorb((uint32_t*) src[0], (uint32_t*) src[1], KYBER_SYMBYTES >> 2);
    volatile uint32_t t;
    t = (SHAKE_PAD << 8) | ((uint32_t) nonce);
    keccak_finish((uint32_t*) &t);
}


void kyber_masked_shake256_prf_squeeze(masked_ptr dst, size_t dst_len)
{
    keccak_squeeze((uint32_t*) dst[0], (uint32_t*) dst[1], dst_len >> 2);
}


void kyber_masked_shake256_prf(masked_ptr dst, size_t dst_len, const masked_sym src, uint8_t nonce)
{  
    kyber_masked_shake256_prf_absorb(src, nonce);
    kyber_masked_shake256_prf_squeeze(dst, dst_len);
}


void masked_shake256(masked_ptr dst, size_t dst_len, const masked_ptr src, size_t src_len)
{  
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_EN);
    keccak_absorb((uint32_t*) src[0], (uint32_t*) src[1], src_len >> 2);
    t = SHAKE_PAD;
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*) dst[0], (uint32_t*) dst[1], dst_len >> 2);
}


void masked_sha3_512_init() {
    keccak_init(SHA3_512_RATE >> 3, KECCAK_MASK_EN);
}


void masked_keccak_core(const masked_ptr src, size_t len) {
    keccak_absorb((uint32_t*) src[0], (uint32_t*) src[1], len >> 2);
}


void masked_sha3_512_finish() {
    volatile uint32_t t;
    t = SHA3_PAD;
    keccak_finish((uint32_t*) &t);
}


void masked_keccak_squeeze(masked_ptr dst, size_t dst_len) {
    keccak_squeeze((uint32_t*) dst[0], (uint32_t*) dst[1], dst_len >> 2);
}


void masked_sha3_512(masked_ptr dst, const masked_ptr src, size_t len) {
    masked_sha3_512_init();
    masked_keccak_core(src, len);
    masked_sha3_512_finish();
    masked_keccak_squeeze(dst, SHA3_512_HASH_SIZE);
}


void masked_kdf(masked_ss dst, const masked_sym key_material, const uint8_t h_ct[KYBER_SYMBYTES]) {
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_EN);
    keccak_absorb((uint32_t*) key_material[0], (uint32_t*) key_material[1], KYBER_SYMBYTES >> 2);
    keccak_absorb_public((uint32_t*) h_ct, KYBER_SYMBYTES >> 2);
    t = SHAKE_PAD;
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*) dst[0], (uint32_t*) dst[1], KYBER_SSBYTES >> 2);
}
