#include <stdint.h>
#include <stdlib.h>
#include "util.h"
#include "cw305.h"
#include "simpleserial_cw305_rq.h"
#include "indcpa.h"
#include "x2x.h"
#include "x2x_prng.h"
#include "masked_gadgets.h"
#include "masked_indcpa.h"
#include "symmetric.h"
#include "timer.h"
#include "victims_commons_kyber.h"
#include "victims_commons_util.h"


#define CIPHERGEN_RETURN_HASH
#define CIPHERGEN_OUTPUT_SIZE 16
//#define RETURN_OUTPUT
#define SLEEP_LOOP (2048)


uint8_t c [KYBER_INDCPA_BYTES];
masked_msg mm; 
uint8_t m[MASKING_N]; 
uint8_t rng_buffer[KYBER_SYMBYTES];
masked_polyvec mskpv;
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
    vcu_ntt_lite_reset_state();
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    ////////////////////// const public input ///////////////////////
    for (int i = 0; i < KYBER_INDCPA_BYTES; i++){
        c[i] = 1;
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
    masked_indcpa_dec_core(mm, c, &mskpv);
#ifdef VERBOSE
    time = timer_read();
    print_string("masked_indcpa dec time: ");
    print_u32(time);
    print_string("\n");
#endif
    (void) mm;
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////////// sleep ///////////////////////////////
    vcu_sleep(SLEEP_LOOP);
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////////// masked output ///////////////////////
#ifdef RETURN_OUTPUT
    for (size_t i = 0; i < MASKING_N; i++) {
        for (size_t j = 1; j < KYBER_INDCPA_MSGBYTES; j++) {
            mm[i][0] ^= mm[i][j];
        }
        m[i] = mm[i][0];
    }
    simpleserial_put('r', MASKING_N, (uint8_t*) m);
#else
    (void) mm;
    simpleserial_put('r', 0, NULL);
#endif
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
#ifdef VERBOSE
    print_string("decrypted message: ");
    print_hex(m, sizeof(m), 0);
    print_string("\n");
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
