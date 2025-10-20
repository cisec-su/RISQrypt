#include "polyvec.h"
#include "masked.h"
#include "pack.h"
#include "masked_poly.h"
#include "masked_polyvec.h"
#include "masked_gadgets.h"
#include "masked_indcpa.h"






int masked_indcpa_enc_cmp(uint8_t c[KYBER_INDCPA_BYTES],
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
    masked_u32 t1, t0;
    uint32_t t;
    uint8_t seed[KYBER_SYMBYTES];

    poly_init_q();

    masked_gadgets_init_q();
    masked_poly_frommsg(&mk, m);

    poly_set_ctrl();

    unpack_pk(&pkpv, seed, pk);
  
    gen_at(at, seed);
    masked_polyvec_getnoise_eta1(&msp, coins, &nonce);
    masked_polyvec_getnoise_eta2(&mep, coins, &nonce);    
    masked_poly_getnoise_eta2(&mepp, coins, &nonce);
    poly_init_ntt();
    masked_polyvec_ntt(&msp);
    poly_init_zeta();

    for (i = 0; i < KYBER_K; i++) {
        masked_polyvec_pointwise_acc_invntt_i(&mbp, &msp, &at[i], i);
    }

    masked_polyvec_pointwise_acc_invntt(&mv, &msp, &pkpv);

    masked_polyvec_add(&mbp, &mbp, &mep);

    masked_poly_add_chain(&mv, &mv, &mepp, &mk);
    
    masked_polyvec_sub_compress(&mpvu32, &mbp, c);
    masked_poly_sub_compress(&mpu32, &mv, c + KYBER_POLYVECCOMPRESSEDBYTES);

    masked_gadgets_init_q_carrier();
    masked_gadgets_B2A_qm_u32_vec(&mpvu32, &mpvu32);
    masked_gadgets_B2A_qm_u32(&mpu32, &mpu32);

    masked_polyvec_u32_acc(t0, &mpvu32, &mpu32);

    return masked_gadgets_zero_test_mul(t0);
}


void masked_indcpa_dec(masked_msg m,
                      const uint8_t c[KYBER_INDCPA_BYTES],
                      const uint8_t sk[KYBER_INDCPA_SECRETKEYBYTES]) {
    unsigned int i;
    polyvec bp, skpv;
    poly v;
    masked_poly mp;
    masked_polyvec mskpv;
                                            
    poly_init_q();

    unpack_ciphertext(&bp, &v, c);

    unpack_sk(&skpv, sk);

    poly_init_ntt();
    polyvec_ntt(&bp);
    
    masked_gadgets_init_q();
    masked_polyvec_mask(&mskpv, &skpv);
    poly_init_zeta();
    masked_polyvec_pointwise_acc_invntt(&mp, &mskpv, &bp);

    masked_poly_sub_tomsg(m, &v, &mp);
}