#ifndef masked_SPX_SIGN_H
#define masked_SPX_SIGN_H

#include <stddef.h>
#include <stdint.h>
#include "params.h"

int masked_crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk,
                                      const unsigned char *seed);

int masked_crypto_sign_keypair(unsigned char *pk, unsigned char *sk);

int masked_crypto_sign_signature(uint8_t *sig, size_t *siglen,
                                   const uint8_t *m, size_t mlen, const uint8_t *sk);

int masked_crypto_sign_verify(const uint8_t *sig, size_t siglen,
                                const uint8_t *m, size_t mlen, const uint8_t *pk);

int masked_crypto_sign(unsigned char *sm, unsigned long long *smlen,
                         const unsigned char *m, unsigned long long mlen,
                         const unsigned char *sk);

int masked_crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                              const unsigned char *sm, unsigned long long smlen,
                              const unsigned char *pk);

#endif
