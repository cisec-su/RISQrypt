#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "keccak.h"
#include "masked_symmetric.h"


#if MASKING_N != 2
#error "Masked hash functions do not support MASKING_N > 2"
#endif



void kyber_masked_shake256_prf(masked_ptr dst, size_t dst_len, const masked_sym src, uint8_t nonce)
{  
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_EN);
    keccak_absorb((uint32_t*) src[0], (uint32_t*) src[1], KYBER_SYMBYTES >> 2);
    t = (SHAKE_PAD << 8) | ((uint32_t) nonce);
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*) dst[0], (uint32_t*) dst[1], dst_len >> 2);
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


void masked_sha3_512(masked_ptr dst, const masked_ptr src, size_t len) {
    volatile uint32_t t;
    keccak_init(SHA3_512_RATE >> 3, KECCAK_MASK_EN);
    keccak_absorb((uint32_t*) src[0], (uint32_t*) src[1], len >> 2);
    t = SHA3_PAD;
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*) dst[0], (uint32_t*) dst[1], SHA3_512_HASH_SIZE >> 2);
}