#ifndef SYMMETRIC_H
#define SYMMETRIC_H

#include <stddef.h>
#include <stdint.h>
#include "params.h"


#define kyber_shake128_init KYBER_NAMESPACE(_kyber_shake128_init)
void kyber_shake128_init();

#define kyber_shake128_absorb KYBER_NAMESPACE(_kyber_shake128_absorb)
void kyber_shake128_absorb(const uint8_t seed[KYBER_SYMBYTES], uint8_t x, uint8_t y);

#define kyber_shake128_squeezeblocks KYBER_NAMESPACE(_kyber_shake128_squeezeblocks)
void kyber_shake128_squeezeblocks(uint8_t *out,
                                  unsigned int num_blocks);

#define kyber_shake256_prf KYBER_NAMESPACE(_kyber_shake256_prf)
void kyber_shake256_prf(uint8_t *out, size_t outlen, const uint8_t key[KYBER_SYMBYTES], uint8_t nonce);


#define SHAKE128_RATE 168
#define SHAKE256_RATE 136
#define SHAKE_PAD 0x1F
#define SHA3_256_RATE 136
#define SHA3_512_RATE 72
#define SHA3_PAD 0x06
#define SHA3_256_HASH_SIZE 32
#define SHA3_512_HASH_SIZE 64
#define XOF_BLOCKBYTES SHAKE128_RATE


void shake256(uint8_t *dst, size_t dst_len, const uint8_t *src, size_t src_len);
void sha3_256(uint8_t *dst, const uint8_t *src, size_t len);
void sha3_512(uint8_t *dst, const uint8_t *src, size_t len);


#define hash_h(OUT, IN, INBYTES) sha3_256(OUT, IN, INBYTES)
#define hash_g(OUT, IN, INBYTES) sha3_512(OUT, IN, INBYTES)
#define xof_init kyber_shake128_init
#define xof_absorb(SEED, X, Y) kyber_shake128_absorb(SEED, X, Y)
#define xof_squeezeblocks(OUT, OUTBLOCKS) \
        kyber_shake128_squeezeblocks(OUT, OUTBLOCKS)
#define prf(OUT, OUTBYTES, KEY, NONCE) \
        kyber_shake256_prf(OUT, OUTBYTES, KEY, NONCE)
#define kdf(OUT, IN, INBYTES) shake256(OUT, KYBER_SSBYTES, IN, INBYTES)


#endif /* SYMMETRIC_H */
