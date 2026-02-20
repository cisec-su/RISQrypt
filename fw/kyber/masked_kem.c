#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "symmetric.h"
#include "masked_gadgets.h"
#include "masked_symmetric.h"
#include "verify.h"
#include "masked_indcpa.h"
#include "masked_kem.h"


void masked_crypto_kem_dec_init(masked_polyvec *mskpv,
                                uint8_t mhz[MASKING_N][KYBER_SYMBYTES * 2],
                                const unsigned char *sk) {
    uint8_t *hz;
    masked_indcpa_dec_init(mskpv, sk);    
    hz = (uint8_t*) (sk + KYBER_SECRETKEYBYTES - 2 * KYBER_SYMBYTES);
    masked_gadgets_init_2k(0xFFFF);
    masked_gadgets_mask_doublesym(mhz, hz);
}


#define FLAT_TO_PTR_LOOP(SRC) for (i = 0; i < MASKING_N; i++) { \
                                  ptr[i] = SRC[i]; } \


int masked_crypto_kem_dec_core(masked_ss mss,
                               const unsigned char *ct,
                               const unsigned char *pk,
                               const masked_polyvec *mskpv,
                               const uint8_t mhz[MASKING_N][KYBER_SYMBYTES * 2]) {

    size_t i;
    int fail;
    masked_msg mm;
    masked_sym mk;
    masked_sym mcoins;
    masked_ptr ptr;
    uint8_t *h_ct = mcoins[0];

    masked_indcpa_dec_core(mm, ct, mskpv);

    ///////// ( K', r') = G(m'||h)
    FLAT_TO_PTR_LOOP(mm)
    masked_hash_g_init();
    masked_hash_g_core(ptr, KYBER_INDCPA_MSGBYTES);
    FLAT_TO_PTR_LOOP((uint8_t*) mhz)
    masked_hash_g_core(ptr, KYBER_INDCPA_MSGBYTES);
    masked_hash_g_finish();
    FLAT_TO_PTR_LOOP(mk)
    masked_hash_g_squeezehalf(ptr);
    FLAT_TO_PTR_LOOP(mcoins)
    masked_hash_g_squeezehalf(ptr);
    ////////////////////////////////

    fail = masked_indcpa_enc_cmp(ct, mm, pk, mcoins);

    hash_h(h_ct, ct, KYBER_CIPHERTEXTBYTES);

    for (i = 0; i < MASKING_N; i++) {
        cmov(mk[i], mhz[i] + KYBER_SYMBYTES, KYBER_SYMBYTES, fail);
    }

    masked_kdf(mss, mk, h_ct);

    return 0;
}


int masked_crypto_kem_dec_finish(unsigned char *ss,
                                 const masked_ss mss) {
    size_t i, j;

    for (i = 0; i < MASKING_N; i++) {
        for (j = 0; j < KYBER_SSBYTES; j++) {
            ss[j] = (i == 0) ? mss[0][j] : mss[i][j] ^ ss[j];
        }
    }

    return 0;
}



int masked_crypto_kem_dec(unsigned char *ss,
                          const unsigned char *ct,
                          const unsigned char *sk) {

    masked_polyvec mskpv;
    uint8_t mhz[MASKING_N][KYBER_SYMBYTES * 2];
    uint8_t *pk = (uint8_t*) (sk + KYBER_INDCPA_SECRETKEYBYTES);
    masked_ss mss;
    masked_crypto_kem_dec_init(&mskpv, mhz, sk);
    masked_crypto_kem_dec_core(mss, ct, pk, &mskpv, mhz);
    masked_crypto_kem_dec_finish(ss, mss);
    return 0;
}
