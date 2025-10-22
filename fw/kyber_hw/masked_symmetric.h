#ifndef MASKED_SYMMETRIC_H
#define MASKED_SYMMETRIC_H

#include "symmetric.h"
#include "masked.h"


#define kyber_masked_shake256_prf KYBER_NAMESPACE(_kyber_masked_shake256_prf)
void kyber_masked_shake256_prf(masked_ptr dst, size_t dst_len, const masked_sym src, uint8_t nonce);


void masked_shake256(masked_ptr dst, size_t dst_len, const masked_ptr src, size_t src_len);

void masked_sha3_512_init();

void masked_keccak_core(const masked_ptr src, size_t len);

void masked_sha3_512_finish();

void masked_keccak_squeeze(masked_ptr dst, size_t dst_len);

void masked_sha3_512(masked_ptr dst, const masked_ptr src, size_t src_len);

void masked_kdf(masked_ss dst, const masked_sym key_material, const uint8_t h_ct[KYBER_SYMBYTES]);


#define masked_hash_g_init() masked_sha3_512_init()
#define masked_hash_g_core(IN, INBYTES) masked_keccak_core(IN, INBYTES)
#define masked_hash_g_finish(OUT) masked_sha3_512_finish()
#define masked_hash_g_squeezehalf(OUT) masked_keccak_squeeze(OUT, SHA3_512_HASH_SIZE >> 1)

#define masked_prf(OUT, OUTBYTES, KEY, NONCE) kyber_masked_shake256_prf(OUT, OUTBYTES, KEY, NONCE)


#endif /* MASKED_SYMMETRIC_H */
