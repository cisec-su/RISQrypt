#include <stdint.h>
#include <stdlib.h>
#include "util.h"
#include "cw305.h"
#include "simpleserial_cw305_rq.h"
#include "masked_gadgets.h"
#include "masked_poly.h"
#include "timer.h"
#include "victims_commons_dilithium.h"
#include "victims_commons_util.h"


#define SLEEP_LOOP 1024

#define ZERO_STACK_SIZE 8192


masked_poly a;
masked_poly r;
poly b;


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
    vcd_masked_poly_from_seed(&a, p);
#ifdef VERBOSE
    vcd_print_poly_shares(&a, "Poly");
#endif
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    //////////////////// dummy input masking ////////////////////////
    for (size_t i = 0; i < len; i++) {
        p[i] = 0;
    }
    // vcd_masked_poly_gamma2_from_seed(&a_dummy, p);
    // poly_init_q();
    // masked_gadgets_init_q();
    // memset(&a_dummy, 0, sizeof(a_dummy));
    // masked_poly_decompose(&b_dummy, &r_dummy, &a_dummy);
    // (void) r_dummy;
    // (void) b_dummy;
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    ////////////////////// initialize modules ///////////////////////
    poly_init_q();
    masked_gadgets_init_q();
    zero_stack(ZERO_STACK_SIZE);
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
    masked_poly_decompose(&b, &r, &a);
    (void) a;
#ifdef VERBOSE
    time = timer_read();
    print_string("masked_poly_decompose time: ");
    print_u32(time);
    print_string("\n");
#endif
    (void) r;
    (void) b;
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
    print_u32_arr((uint32_t*) r.share[0].coeffs, 8);
    print_u32_arr((uint32_t*) r.share[0].coeffs + N - 8, 8);
    print_string("\n");
    print_string("Poly r Share 1 Coeffs: ");
    print_u32_arr((uint32_t*) r.share[1].coeffs, 8);
    print_u32_arr((uint32_t*) r.share[1].coeffs + N - 8, 8);
    print_string("\n");
#endif

    return 0x00;
}


int main(void)
{
    cw305_trigger_down();
    print_string("Dilithium PolyDecompose Dec\n");


    simpleserial_init();
    simpleserial_addcmd('l', 8, vcu_prng_on);
    simpleserial_addcmd('g', 0, vcu_prng_off);
    simpleserial_addcmd('p', VCD_SEED_LEN, get_poly);

    while(1)
        simpleserial_cw305_rq_get();
}
