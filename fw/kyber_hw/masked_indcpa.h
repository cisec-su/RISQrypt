#ifndef MASKED_INDCPA_H
#define MASKED_INDCPA_H


#include <stdint.h>
#include "params.h"
#include "masked.h"
#include "masked_polyvec.h"


#define masked_indcpa_dec_init KYBER_NAMESPACE(_masked_indcpa_dec_init)
void masked_indcpa_dec_init(masked_polyvec *mskpv,
                            const uint8_t sk[KYBER_INDCPA_SECRETKEYBYTES]);


#define masked_indcpa_dec_core KYBER_NAMESPACE(_masked_indcpa_dec_core)
void masked_indcpa_dec_core(masked_msg mm,
                            const uint8_t c[KYBER_INDCPA_BYTES],
                            const masked_polyvec *mskpv);


#define masked_indcpa_dec_finish KYBER_NAMESPACE(_masked_indcpa_dec_finish)
void masked_indcpa_dec_finish(uint8_t m[KYBER_INDCPA_MSGBYTES],
                              const masked_msg mm);


#define masked_indcpa_dec KYBER_NAMESPACE(_masked_indcpa_dec)
void masked_indcpa_dec(uint8_t m[KYBER_INDCPA_MSGBYTES],
                       const uint8_t c[KYBER_INDCPA_BYTES],
                       const uint8_t sk[KYBER_INDCPA_SECRETKEYBYTES]);


#define masked_indcpa_enc_cmp KYBER_NAMESPACE(_masked_indcpa_enc_cmp)
int masked_indcpa_enc_cmp(uint8_t c[KYBER_INDCPA_BYTES],
                          const masked_msg m,
                          const uint8_t pk[KYBER_INDCPA_PUBLICKEYBYTES],
                          const masked_sym coins);


#endif