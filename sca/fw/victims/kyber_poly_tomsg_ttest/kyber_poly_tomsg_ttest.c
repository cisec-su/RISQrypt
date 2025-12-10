#include <stdint.h>
#include <stdlib.h>
#include "util.h"
#include "cw305.h"
#include "simpleserial_cw305_rq.h"
#include "x2x.h"
#include "ntt_lite.h"
#include "x2x_prng.h"
#include "masked_gadgets.h"
#include "masked_poly.h"
#include "symmetric.h"
#include "timer.h"
#include "victims_commons_kyber.h"
#include "victims_commons_util.h"


#define CIPHERGEN_RETURN_HASH
#define OUTPUT_SIZE 0
#define SLEEP_LOOP 1024


poly poly_a;
masked_poly poly_b;
masked_poly poly_b_dummy;
masked_msg mm;
masked_msg mm_dummy;
uint8_t temp_buffer[KYBER_SYMBYTES];



uint8_t get_poly(uint8_t* p, uint8_t len)
{
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
    vck_masked_poly_from_seed(&poly_b, p);
#ifdef VERBOSE
    vck_print_poly_shares(&poly_b, "Poly");
#endif
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    //////////////////// dummy input masking ////////////////////////
    for (size_t i = 0; i < len; i++) {
        p[i] = 0;
    }
    vck_masked_poly_from_seed(&poly_b_dummy, p);
    masked_poly_tomsg(mm_dummy, &poly_b_dummy);
    (void) mm_dummy;
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    ////////////////////// initialize modules ///////////////////////
    poly_init_q();
    masked_gadgets_init_q();
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
    masked_poly_tomsg(mm, &poly_b);
#ifdef VERBOSE
    time = timer_read();
    print_string("masked_poly_tomsg time: ");
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
#if OUTPUT_SIZE > 0
    for (size_t i = 0; i < OUTPUT_SIZE/2; i++) {
        temp_buffer[i] = mm[0][i];
        temp_buffer[i + OUTPUT_SIZE/2] = mm[1][i];
    }
    simpleserial_put('r', OUTPUT_SIZE, temp_buffer);
#else
    simpleserial_put('r', 0, NULL);
#endif
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
#ifdef VERBOSE
    print_string("SimpleSerial::get_key done\n Message shares: \n");
    print_hex(mm[0], KYBER_INDCPA_MSGBYTES, 0);
    print_string("\n");
    print_hex(mm[1], KYBER_INDCPA_MSGBYTES, 0);
    print_string("\n");
#endif

    return 0x00;
}


int main(void)
{
    cw305_trigger_down();
    print_string("Kyber PolyToMsg Dec\n");


    simpleserial_init();
    simpleserial_addcmd('l', 0, vcu_prng_on);
    simpleserial_addcmd('g', 0, vcu_prng_off);
    simpleserial_addcmd('p', KYBER_SYMBYTES, get_poly);

    while(1)
        simpleserial_cw305_rq_get();
}
