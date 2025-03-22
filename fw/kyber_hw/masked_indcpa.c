#include "polyvec.h"
#include "masked.h"
#include "pack.h"
#include "masked_poly.h"
#include "masked_polyvec.h"
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
    masked_coeff t;
    uint8_t seed[KYBER_SYMBYTES];

    poly_init_q();

    unpack_ciphertext(&bp, &v, c);

    masked_poly_frommsg(&mk, m);

    poly_init_q();

    unpack_pk(&pkpv, seed, pk);
  
    gen_at(at, seed);
    masked_polyvec_getnoise_eta1(&msp, coins, &nonce);
    print_string("msp\n");
    unmask_and_print_u32_vec(&msp, 0, 4);
    masked_polyvec_getnoise_eta2(&mep, coins, &nonce);    
    masked_poly_getnoise_eta2(&mepp, coins, &nonce);
    print_string("mepp\n");
    unmask_and_print_u32(&mepp, 4);
    poly_init_ntt();
    masked_polyvec_ntt(&msp);

    for(i = 0; i < KYBER_K; i++) {
        masked_polyvec_pointwise_acc_invntt_i(&mbp, &msp, &at[i], i);
    }
    masked_polyvec_pointwise_acc_invntt(&mv, &msp, &pkpv);

    masked_polyvec_add(&mbp, &mbp, &mep);
    masked_poly_add_chain(&mv, &mv, &mepp, &mk);

    masked_polyvec_compress(&mbp, &mbp);
    masked_poly_compress(&mv, &mv);

    // exponentation-based comparsion
    // https://eprint.iacr.org/2021/1615.pdf
    // distinct then the reference, we use decompressed polynomials, and use a second exp. instead of secure ands.
    // after the exponentation, we add all the coefficients, so if any non-zero bit exists, the sum will be non-zero
    // then, we perform a second exponentation.
    poly_init_dual();
    masked_polyvec_sub_exp(&mbp, &mbp, &bp);
    masked_poly_sub_exp(&mv, &mv, &v);

    // 1 - x^{q-1}
    masked_polyvec_sub_one(&mbp, &mbp);
    // array of 1s will be on the HW engine.
    // therefore, we call the subtraction function which subtracts whatever left inside the HW from mv  
    masked_poly_sub_x(&mv, &mv);

    // at this point, any coefficient which is 1 (shared) shows a mismatch in the comparison
    // therefore we sum them up and perform the second exponentation.
    masked_polyvec_acc(&mv, &mbp, &mv);
    masked_poly_sum(&t, &mv);
    masked_poly_coeff_exp(&t, &t);

    masked_poly_unmask_coeff_inp(&t);

    return (int) t[0];
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

    masked_polyvec_mask(&mskpv, &skpv);

    masked_polyvec_pointwise_acc_invntt(&mp, &mskpv, &bp);

    masked_poly_sub_tomsg(m, &v, &mp);
}


void unmask_and_print_u32_vec(masked_polyvec *a, unsigned int i, unsigned int len) {
    poly b;
    unsigned int j, k;
    uint16_t t;
    for (j = 0; j < KYBER_N; j++) {
        t = 0;
        for (k = 0; k < MASKING_N; k++) {
            t += a->share[k].vec[i].coeffs[j];
        }
        b.coeffs[j] = t % KYBER_Q;
    }
    print_u32_arr((uint32_t*) b.coeffs, len);
}


void unmask_and_print_u32(masked_poly *a, unsigned int len) {
    poly b;
    unsigned int j, k;
    uint16_t t;
    for (j = 0; j < KYBER_N; j++) {
        t = 0;
        for (k = 0; k < MASKING_N; k++) {
            t += a->share[k].coeffs[j];
        }
        b.coeffs[j] = t % KYBER_Q;
    }
    print_u32_arr((uint32_t*) b.coeffs, len);
}