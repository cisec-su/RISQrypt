#include <stdint.h>
#include <stdlib.h>
#include "util.h"
#include "cw305.h"
#include "simpleserial_cw305_rq.h"
#include "masked_gadgets.h"
#include "masked_poly.h"
#include "masked_polyvec.h"
#include "masked_sign.h"
#include "timer.h"
#include "victims_commons_dilithium.h"
#include "victims_commons_util.h"


#define SLEEP_LOOP 1024
#define MSG_LEN 32


uint8_t tr[SEEDBYTES];
polyveck t0;
uint8_t m[MSG_LEN];
size_t siglen;
polyvecl mat[K];


masked_polyvecl s1;
masked_polyveck s2;
masked_seed key;
uint8_t sig[CRYPTO_BYTES];

masked_polyveck s2_dummy;



uint8_t get_poly(uint8_t* p, uint8_t len)
{
    size_t i, j, k;
#ifdef VERBOSE
    uint32_t time;
    print_string("SimpleSerial::get_poly command received\n");
    print_string("p: ");
    print_hex(p, len, 0);
    print_string("\n");
#endif
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////// real input masking //////////////////////
    vcd_masked_polyveck_from_seed(&s2, p);
    vcd_masked_polyvecl_from_seed(&s1, p);
    vcd_masked_msg_from_seed(key, p);
#ifdef VERBOSE
    vcd_print_polyvecl_shares(&s1, "Polyvecl S1");
    vcd_print_polyveck_shares(&s2, "Polyveck S2");
    vcd_print_polyvecl_shares_unmasked(&s1, "Polyvecl S1 unmasked");
    vcd_print_polyveck_shares_unmasked(&s2, "Polyveck S2 unmasked");
    vcd_print_maskedseed_shares_unmasked(key, "Masked Seed Key unmasked");
#endif
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    //////////////////// dummy input masking ////////////////////////
    for (i = 0; i < len; i++) {
        p[i] = 0;
    }
    vcd_masked_polyveck_from_seed(&s2_dummy, p);
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    ////////////////////// initialize modules ///////////////////////
    poly_init_q();
    masked_gadgets_init_q();
    for (i = 0; i < K; i++) {
        for (j = 0; j < N; j++) {
            t0.vec[i].coeffs[j] = 1;
        }
    }
    for (i = 0; i < CRYPTO_BYTES; i++) {
        sig[i] = 0;
    }
    for (size_t i = 0; i < K; i++) {
        for (size_t j = 0; j < L; j++) {
            for (size_t k = 0; k < N; k++) {
                mat[i].vec[j].coeffs[k] = 1;
            }
        }
    }
    for (size_t i = 0; i < SEEDBYTES; i++) {
        tr[i] = 1;
    }
    for (size_t i = 0; i < MSG_LEN; i++) {
        m[i] = 1;
    }
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////////// sleep ///////////////////////////////
    vcu_sleep(SLEEP_LOOP);
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    ////////////////////// trigger and action ///////////////////////
    cw305_trigger_up();
#ifdef VERBOSE
    timer_reset();
    timer_start();
#endif
    masked_crypto_sign_signature_core(sig,
                                      &siglen,
                                      m,
                                      MSG_LEN,
                                      mat,
                                      tr,
                                      key,
                                      &t0,
                                      &s1,
                                      &s2);
    (void) sig;
#ifdef VERBOSE
    time = timer_read();
    print_string("masked_sign time: ");
    print_u32(time);
    print_string("\n");
#endif
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////////// sleep ///////////////////////////////
    vcu_sleep(SLEEP_LOOP);
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////////// masked output ///////////////////////
    simpleserial_put('r', 0, NULL);
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
#ifdef VERBOSE
    print_string("SimpleSerial::get_key done \n");
#endif

    return 0x00;
}


int main(void)
{
    cw305_trigger_down();
    print_string("Dilithium Sign\n");

    for (size_t i = 0; i < K; i++) {
        for (size_t j = 0; j < L; j++) {
            for (size_t k = 0; k < N; k++) {
                mat[i].vec[j].coeffs[k] = 1;
            }
        }
    }
    for (size_t i = 0; i < SEEDBYTES; i++) {
        tr[i] = 1;
    }
    for (size_t i = 0; i < MSG_LEN; i++) {
        m[i] = 1;
    }

    simpleserial_init();
    simpleserial_addcmd('l', 8, vcu_prng_on);
    simpleserial_addcmd('g', 0, vcu_prng_off);
    simpleserial_addcmd('p', VCD_SEED_LEN, get_poly);

    while(1)
        simpleserial_cw305_rq_get();
    // uint8_t temp[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    // get_poly(temp, 32);
}
