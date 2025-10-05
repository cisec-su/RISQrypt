#include "masked_symmetric.h"
#include "keccak.h"




void dilithium_masked_shake256_absorb_double(masked_flat_ptr dst, unsigned int dst_len, const masked_flat_ptr src_priv, unsigned int src_priv_len, const uint8_t *src_pub, unsigned int src_pub_len) {
    volatile uint32_t t;
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_EN);
    keccak_absorb((uint32_t*) src_priv, (uint32_t*) (src_priv + src_priv_len), (src_priv_len) >> 2);
    keccak_absorb_public((uint32_t*) src_pub, (src_pub_len) >> 2);
    t = SHAKE_PAD;
    keccak_finish((uint32_t*) &t);
    keccak_squeeze((uint32_t*) dst, (uint32_t*) (dst + dst_len), (dst_len) >> 2);
}


void dilithium_masked_shake256_absorb_nonce(masked_flat_ptr dst, unsigned int dst_len, const masked_flat_ptr src, unsigned int src_len, const uint16_t nonce) {
    volatile uint32_t t;
    print_string("\n");
    print_u32(src);
    print_string("\n");
    print_u32(dst);
    print_string("\n");
    keccak_init(SHAKE256_RATE >> 3, KECCAK_MASK_EN);
    print_string("flag\n");
    keccak_absorb((uint32_t*) src, (uint32_t*) (src + src_len), (src_len) >> 2);
    print_string("flag\n");
    t = (SHAKE_PAD << 16) | ((uint32_t) nonce);
    keccak_finish((uint32_t*) &t);
    print_string("flag\n");
    keccak_squeeze((uint32_t*) dst, (uint32_t*) (dst + dst_len), (dst_len) >> 2);
}