#ifndef MASKED_SIGN_H
#define MASKED_SIGN_H

#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "api.h"
#include "polyvec.h"
#include "poly.h"
#include "masked_polyvec.h"


#define masked_crypto_sign_signature_init DILITHIUM_NAMESPACE(masked_crypto_sign_signature_init)
int masked_crypto_sign_signature_init(polyvecl mat[K],
                                      uint8_t *tr,
                                      polyveck *t0,
                                      masked_polyvecl_t *s1,
                                      masked_polyveck_t *s2,
                                      masked_seed key,
                                      const uint8_t *sk);

#define masked_crypto_sign_signature_core DILITHIUM_NAMESPACE(masked_crypto_sign_signature_core)
int masked_crypto_sign_signature_core(uint8_t *sig,
                                      size_t *siglen,
                                      const uint8_t *m,
                                      size_t mlen,
                                      const polyvecl mat[K],
                                      const uint8_t *tr,
                                      const masked_seed key,
                                      polyveck *t0,
                                      masked_polyvecl_t *s1,
                                      masked_polyveck_t *s2);

#define masked_crypto_sign_signature DILITHIUM_NAMESPACE(masked_signature)
int masked_crypto_sign_signature(uint8_t *sig, size_t *siglen,
                                 const uint8_t *m, size_t mlen,
                                 const uint8_t *sk);

#endif
