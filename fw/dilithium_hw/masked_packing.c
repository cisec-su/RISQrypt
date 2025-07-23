#include "masked_packing.h"

void masked_unpack_sk(uint8_t *rho,
                      uint8_t *tr,
                      masked_seed key,
                      masked_polyveck *t0,
                      masked_polyvecl *s1,
                      masked_polyveck *s2,
                      const uint8_t sk[CRYPTO_SECRETKEYBYTES])
{
  unsigned int i;  

  for(i = 0; i < SEEDBYTES; ++i)
    rho[i] = sk[i];
  sk += SEEDBYTES;
  
  mask_seed(key, sk);
  sk += SEEDBYTES;

  for(i = 0; i < SEEDBYTES; ++i)
    tr[i] = sk[i];
  sk += SEEDBYTES;

  masked_polyvecl_eta_unpack(s1, sk);
  sk += L*POLYETA_PACKEDBYTES;

  masked_polyveck_eta_unpack(s2, sk);
  sk += K*POLYETA_PACKEDBYTES;

  masked_polyveck_t0_unpack(t0, sk);
}
