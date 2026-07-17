#include <string.h>
#include "masked_packing.h"
#include "ntt_lite.h"


void masked_unpack_sk(uint8_t *rho,
                      uint8_t *tr,
                      masked_seed key,
                      polyveck *t0,
                      masked_polyvecl *s1,
                      masked_polyveck *s2,
                      const uint8_t sk[CRYPTO_SECRETKEYBYTES])
{
    unsigned int i;

    memcpy(rho, sk, SEEDBYTES);
    sk += SEEDBYTES;

    mask_seed(key, sk);
    sk += SEEDBYTES;

    memcpy(tr, sk, SEEDBYTES);
    sk += SEEDBYTES;

    for (i = 0; i < L + K; i++) {
        masked_poly_ptr r_ptr;

        if (i < L) {
            r_ptr.share[0] = &s1->share[0].vec[i];
            r_ptr.share[1] = &s1->share[1].vec[i];
        } else {
            r_ptr.share[0] = &s2->share[0].vec[i - L];
            r_ptr.share[1] = &s2->share[1].vec[i - L];
        }

        masked_poly_ptr_unpack(&r_ptr, sk + i * POLYETA_PACKEDBYTES, L, LOG_ETA, ETA);
    }

    sk += (L + K) * POLYETA_PACKEDBYTES;

    if(t0 != NULL) {
        ntt_lite_set_bound(1 << (D - 1));

        for(i = 0; i < K; i++)
            polyt0_unpack(&t0->vec[i], sk + i * POLYT0_PACKEDBYTES);
    }
}

void masked_unpack_sk_t0_row(poly *t0, const uint8_t sk[CRYPTO_SECRETKEYBYTES], unsigned int i)
{
    const uint8_t *packed_t0;

    packed_t0 = sk
              + 3 * SEEDBYTES
              + L * POLYETA_PACKEDBYTES
              + K * POLYETA_PACKEDBYTES;

    ntt_lite_set_bound(1 << (D - 1));

    polyt0_unpack(t0, packed_t0 + i * POLYT0_PACKEDBYTES);
}
