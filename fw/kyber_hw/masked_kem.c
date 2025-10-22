#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "symmetric.h"
#include "masked_symmetric.h"
#include "verify.h"
#include "masked_indcpa.h"
#include "masked_kem.h"


void unmask_and_print(uint8_t m[2][32], const char* label) {
        uint8_t unmasked[32];
        for (size_t i = 0; i < 32; i++) {
                unmasked[i] = m[0][i] ^ m[1][i];
        }
        print_string(label);
        print_string(": ");
        print_hex(unmasked, 32, 0);
        print_string("\n");
}


void just_print(uint8_t m[32], const char* label) {
        print_string(label);
        print_string(": ");
        print_hex(m, 32, 0);
        print_string("\n");
}


int masked_crypto_kem_dec(unsigned char *ss,
                          const unsigned char *ct,
                          const unsigned char *sk)
{
    size_t i, j;
    int fail;
    masked_msg mm;
    masked_msg temp;
    uint8_t mk[MASKING_N][KYBER_SYMBYTES * 2];
    masked_sym mcoins;
    masked_ptr ptr;

    const uint8_t *pk = sk+KYBER_INDCPA_SECRETKEYBYTES;
    masked_indcpa_dec(mm, ct, sk);

    ///////// ( K', r') = G(m'||h)
    for (i = 0; i < MASKING_N; i++) {
        ptr[i] = mm[i];
    }
    masked_hash_g_init();
    masked_hash_g_core(ptr, KYBER_INDCPA_MSGBYTES);
    for(i = 0; i < KYBER_SYMBYTES; i++) {
        temp[0][i] = sk[KYBER_SECRETKEYBYTES-2*KYBER_SYMBYTES+i];
        temp[1][i] = 0;
    }
    for (i = 0; i < MASKING_N; i++) {
        ptr[i] = temp[i];
    }
    masked_hash_g_core(ptr, KYBER_INDCPA_MSGBYTES);
    masked_hash_g_finish();
    for (i = 0; i < MASKING_N; i++) {
        ptr[i] = mk[i];
    }
    masked_hash_g_squeezehalf(ptr);
    for (i = 0; i < MASKING_N; i++) {
        ptr[i] = mcoins[i];
    }
    masked_hash_g_squeezehalf(ptr);
    ////////////////////////////////

    fail = masked_indcpa_enc_cmp(ct, mm, pk, mcoins);

    hash_h(mk[0] + KYBER_SYMBYTES, ct, KYBER_CIPHERTEXTBYTES);

    // masked cmov
    cmov(mk[0], sk + KYBER_SECRETKEYBYTES - KYBER_SYMBYTES, KYBER_SYMBYTES, fail);
    // temp, will change
    for (i = 0; i < KYBER_SYMBYTES; i++) {
        mm[0][i] = 0;
    }
    for (i = 1; i < MASKING_N; i++) {
        cmov(mk[i], mm[0], KYBER_SYMBYTES, fail);
    }

    // unmasking masked_kr
    for (i = 1; i < MASKING_N; i++) {
        for (j = 0; j < KYBER_SYMBYTES; j++) {
            mk[0][j] ^= mk[i][j];
        }
    }

    kdf(ss, mk[0], KYBER_SYMBYTES * 2);

    return 0;
}
