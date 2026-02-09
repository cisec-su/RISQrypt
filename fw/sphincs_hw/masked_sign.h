#ifndef SPX_MASKED_SIGN_H
#define SPX_MASKED_SIGN_H

#include <stddef.h>
#include <stdint.h>
#include "params.h"

int crypto_sign_signature_masked(uint8_t *sig, size_t *siglen,
                          const uint8_t *m, size_t mlen, const uint8_t *sk);

int crypto_sign_verify_masked(const uint8_t *sig, size_t siglen,
                       const uint8_t *m, size_t mlen, const uint8_t *pk);

int crypto_sign_masked(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk);

int crypto_sign_open_masked(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk);

#endif
