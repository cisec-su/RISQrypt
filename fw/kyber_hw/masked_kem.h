#ifndef MASKED_KEM_H
#define MASKED_KEM_H

#include "params.h"

#define masked_crypto_kem_dec KYBER_NAMESPACE(_masked_dec)
int masked_crypto_kem_dec(unsigned char *ss,
                          const unsigned char *ct,
                          const unsigned char *sk);

#endif
