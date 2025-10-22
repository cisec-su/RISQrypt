#ifndef MASKED_KEM_H
#define MASKED_KEM_H

#include "params.h"
#include "masked.h"
#include "masked_polyvec.h"


/* WARNING: init function must be called before every core to use a fresh sharing of the secret key.
   alternatively, one can use mask refreshing before every core call */
#define masked_crypto_kem_dec KYBER_NAMESPACE(_masked_dec_init)
void masked_crypto_kem_dec_init(masked_polyvec *mskpv,
                                uint8_t mhz[MASKING_N][KYBER_SYMBYTES * 2],
                                const unsigned char *sk);


#define masked_crypto_kem_dec_core KYBER_NAMESPACE(_masked_dec_core)
int masked_crypto_kem_dec_core(masked_ss mss,
                               const unsigned char *ct,
                               const unsigned char *pk,
                               const masked_polyvec *mskpv,
                               const uint8_t mhz[MASKING_N][KYBER_SYMBYTES * 2]);



#define masked_crypto_kem_dec_finish KYBER_NAMESPACE(_masked_dec_finish)
int masked_crypto_kem_dec_finish(unsigned char *ss,
                                 const masked_ss mss);


#define masked_crypto_kem_dec KYBER_NAMESPACE(_masked_dec)
int masked_crypto_kem_dec(unsigned char *ss,
                          const unsigned char *ct,
                          const unsigned char *sk);

#endif
