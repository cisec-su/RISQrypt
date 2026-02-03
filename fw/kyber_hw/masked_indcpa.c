#include "polyvec.h"
#include "masked.h"
#include "pack.h"
#include "masked_poly.h"
#include "masked_polyvec.h"
#include "masked_gadgets.h"
#include "masked_symmetric.h"
#include "masked_indcpa.h"






int masked_indcpa_enc_cmp(const uint8_t c[KYBER_INDCPA_BYTES],
                          const masked_msg m,
                          const uint8_t pk[KYBER_INDCPA_PUBLICKEYBYTES],
                          const masked_sym coins) {
    unsigned int i;
    polyvec bp, pkpv, at[KYBER_K];
    poly v;
    uint8_t nonce = 0;
    masked_poly mepp, mv, mk;
    masked_polyvec mskpv, msp, mep, mbp;
    masked_poly_u32 mpu32;
    masked_polyvec_u32 mpvu32;
    masked_u32 t0;
    uint32_t t;
    uint8_t seed[KYBER_SYMBYTES];

    poly_init_q();

    masked_gadgets_init_q();
    masked_poly_frommsg(&mk, m);

    poly_set_ctrl();

    unpack_pk(&pkpv, seed, pk);
  
    gen_at(at, seed);
    // masked_prf_absorb(coins, nonce++);
    masked_polyvec_getnoise_eta1_fromhw(&msp, coins, &nonce);
    masked_polyvec_getnoise_eta2_fromhw(&mep, coins, &nonce);    
    masked_poly_getnoise_eta2_fromhw(&mepp, coins);
    poly_init_ntt();
    masked_polyvec_ntt(&msp);
    poly_init_zeta();

    for (i = 0; i < KYBER_K; i++) {
        masked_polyvec_pointwise_acc_invntt_add_i(&mbp, &msp, &at[i], &mep, i);
    }

    masked_polyvec_pointwise_acc_invntt_addchain(&mv, &msp, &pkpv, &mepp, &mk);
    
    masked_polyvec_sub_compress(&mpvu32, &mbp, c);
    masked_poly_sub_compress(&mpu32, &mv, c + KYBER_POLYVECCOMPRESSEDBYTES);

    return masked_gadgets_zero_test_vec(t0, &mpvu32, &mpu32);
}


void masked_indcpa_dec_init(masked_polyvec *mskpv,
                            const uint8_t sk[KYBER_INDCPA_SECRETKEYBYTES]) {
    polyvec skpv;
                                            
    poly_init_q();
    unpack_sk(&skpv, sk);

    masked_gadgets_init_q();
    masked_polyvec_mask(mskpv, &skpv);
}


void masked_indcpa_dec_core(masked_msg mm,
                            const uint8_t c[KYBER_INDCPA_BYTES],
                            const masked_polyvec *mskpv) {
    polyvec bp;
    poly v;
    masked_poly mp;

    unpack_ciphertext(&bp, &v, c);

    poly_init_ntt();
    polyvec_ntt(&bp);

    poly_init_zeta();
    masked_polyvec_pointwise_acc_invntt_sub(&mp, mskpv, &bp, &v);
    masked_poly_tomsg(mm, &mp);
}


void masked_indcpa_dec(masked_msg mm,
                       const uint8_t c[KYBER_INDCPA_BYTES],
                       const uint8_t sk[KYBER_INDCPA_SECRETKEYBYTES]) {
    masked_polyvec mskpv;
    masked_indcpa_dec_init(&mskpv, sk);
    masked_indcpa_dec_core(mm, c, &mskpv);
}