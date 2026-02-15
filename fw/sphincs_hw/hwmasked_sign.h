#ifndef SPX_HWMASKED_SIGN_H
#define SPX_HWMASKED_SIGN_H

#include <stddef.h>
#include <stdint.h>
#include "params.h"

int crypto_sign_seed_keypair_hwmasked(unsigned char *pk, unsigned char *sk,
                                      const unsigned char *seed);

int crypto_sign_signature_hwmasked(uint8_t *sig, size_t *siglen,
                                   const uint8_t *m, size_t mlen, const uint8_t *sk);

int crypto_sign_verify_hwmasked(const uint8_t *sig, size_t siglen,
                                const uint8_t *m, size_t mlen, const uint8_t *pk);

int crypto_sign_hwmasked(unsigned char *sm, unsigned long long *smlen,
                         const unsigned char *m, unsigned long long mlen,
                         const unsigned char *sk);

int crypto_sign_open_hwmasked(unsigned char *m, unsigned long long *mlen,
                              const unsigned char *sm, unsigned long long smlen,
                              const unsigned char *pk);

#endif
