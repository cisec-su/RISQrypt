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
#define SLEEP_LOOP 1024


poly poly_a;
masked_poly poly_b;
masked_poly poly_b_dummy;
uint8_t bytes_b[KYBER_POLYVECCOMPRESSEDBYTES / KYBER_K];
poly_u32 *r[MASKING_N];


uint8_t get_poly(uint8_t* p, uint8_t len)
{
    const poly *poly_b_ptr[MASKING_N] = {&poly_b.share[0], &poly_b.share[1]};
#ifdef VERBOSE
    uint32_t time;
    print_string("SimpleSerial::get_poly command received\n");
    print_string("p: ");
    print_hex(p, len, 0);
    print_string("\n");
#endif
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////// zero public input ///////////////////////
    for (size_t i = 0; i < KYBER_N; i++) {
        poly_a.coeffs[i] = 0;
    }
    for (size_t i = 0; i < (KYBER_POLYVECCOMPRESSEDBYTES / KYBER_K); i++) {
        bytes_b[i] = 0;
    }
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
    (void) poly_b_dummy;
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
    masked_poly_sub_compress_du(r, poly_b_ptr, bytes_b, 1);
#ifdef VERBOSE
    time = timer_read();
    print_string("masked_poly_sub_compress time: ");
    print_u32(time);
    print_string("\n");
#endif
    (void) r;
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
    print_string("SimpleSerial::get_key done\n Message shares: \n");
    print_string("Poly r Share 0 Coeffs: ");
    print_u32_arr((uint32_t*) r[0].coeffs, 8);
    print_u32_arr((uint32_t*) r[0].coeffs + KYBER_N - 8, 8);
    print_string("\n");
    print_string("Poly r Share 1 Coeffs: ");
    print_u32_arr((uint32_t*) r[1].coeffs, 8);
    print_u32_arr((uint32_t*) r[1].coeffs + KYBER_N - 8, 8);
    print_string("\n");
#endif

    return 0x00;
}


int main(void)
{
    cw305_trigger_down();
    print_string("Kyber PolyCompress Dec\n");


    simpleserial_init();
    simpleserial_addcmd('l', 0, vcu_prng_on);
    simpleserial_addcmd('g', 0, vcu_prng_off);
    simpleserial_addcmd('p', KYBER_SYMBYTES, get_poly);

    while(1)
        simpleserial_cw305_rq_get();
}
