#include <stdint.h>
#include <stdlib.h>
#include "util.h"
#include "cw305.h"
#include "simpleserial_cw305_rq.h"
#include "indcpa.h"
#include "x2x.h"
#include "kem.h"
#include "masked_kem.h"
#include "symmetric.h"
#include "masked_gadgets.h"
#include "timer.h"
#include "victims_commons_kyber.h"
#include "victims_commons_util.h"


#define CIPHERGEN_RETURN_HASH
#define OUTPUT_SIZE 4
#define SLEEP_LOOP 4096


uint8_t pk[KYBER_PUBLICKEYBYTES ];
uint8_t sk[KYBER_SECRETKEYBYTES ];
uint8_t c [KYBER_CIPHERTEXTBYTES];
masked_ss mss;
masked_polyvec mskpv;
uint8_t mhz[MASKING_N][KYBER_SYMBYTES * 2];
masked_polyvec mskpv_dummy;


uint8_t get_key(uint8_t* k, uint8_t len)
{
#ifdef VERBOSE
    uint32_t time;
    print_string("SimpleSerial::get_key command received\n");
    print_string("k: ");
    print_hex(k, len, 0);
    print_string("\n");
#endif
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////// real input masking //////////////////////
    vck_masked_polyvec_from_seed(&mskpv, k);
#ifdef VERBOSE
    vck_print_polyvec_shares(&mskpv, "Masked SKVEC");
#endif
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    //////////////////// dummy input masking ////////////////////////
    for (size_t i = 0; i < len; i++) {
        k[i] = 0;
    }
    vck_masked_polyvec_from_seed(&mskpv_dummy, k);
    (void) mskpv_dummy;
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    ////////////////////// initialize modules ///////////////////////
    poly_init_q();
    masked_gadgets_init_q();
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////////// sleep ///////////////////////////////
    vcu_ntt_lite_reset_state();
    for (int i = 0; i < KYBER_CIPHERTEXTBYTES; i++){
        c[i] = 1;
    }
    for (int i = 0; i < KYBER_PUBLICKEYBYTES; i++){
        pk[i] = 1;
    }
    for (int i = 0; i < MASKING_N; i++){
        for (int j = 0; j < KYBER_SYMBYTES * 2; j++){
            mhz[i][j] = 1;
        }
    }
    vcu_sleep(SLEEP_LOOP);
    cw305_trigger_up();
#ifdef VERBOSE
    timer_reset();
    timer_start();
#endif
    masked_crypto_kem_dec_core(mss, c, pk, &mskpv, mhz);
#ifdef VERBOSE
    time = timer_read();
    print_string("masked_crypto_kem_dec_core time: ");
    print_u32(time);
    print_string("\n");
#endif
    (void) mss;
    for (volatile size_t i = 0; i < SLEEP_LOOP; i++) {
        (void) i;
    }

    simpleserial_put('r', 0, NULL);

#ifdef VERBOSE
    print_string("SimpleSerial::get_key done\n");
#endif

    return 0x00;
}


int main(void)
{
    cw305_trigger_down();
    print_string("Kyber CPAPKE Dec\n");

    simpleserial_init();
    simpleserial_addcmd('l', 0, vcu_prng_on);
    simpleserial_addcmd('g', 0, vcu_prng_off);
    simpleserial_addcmd('p', KYBER_SYMBYTES, get_key);

    while(1)
        simpleserial_cw305_rq_get();
}
