#ifndef MASKED_SIGN_H
#define MASKED_SIGN_H

#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "api.h"
#include "polyvec.h"
#include "poly.h"

#define masked_crypto_sign_signature DILITHIUM_NAMESPACE(masked_signature)
int masked_crypto_sign_signature(uint8_t *sig, size_t *siglen,
                                 const uint8_t *m, size_t mlen,
                                 const uint8_t *sk);

#endif
