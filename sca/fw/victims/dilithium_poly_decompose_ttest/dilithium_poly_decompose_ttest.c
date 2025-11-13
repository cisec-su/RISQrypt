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


#define CIPHERGEN_RETURN_HASH
#define SLEEP_LOOP 1024


masked_poly a;
masked_poly r;
poly b;
masked_poly a_dummy;
masked_poly r_dummy;
poly b_dummy;

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
    vcd_masked_poly_gamma2_from_seed(&a, p);
#ifdef VERBOSE
    vcd_print_poly_shares(&a, "Poly");
#endif
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    //////////////////// dummy input masking ////////////////////////
    for (size_t i = 0; i < len; i++) {
        p[i] = 0;
    }
    vcd_masked_poly_gamma2_from_seed(&a_dummy, p);
    memset(&a_dummy, 0, sizeof(a_dummy));
    masked_poly_decompose(&b_dummy, &r_dummy, &a_dummy);
    (void) r_dummy;
    (void) b_dummy;
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
    masked_poly_decompose(&b, &r, &a);
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
    // cw305_trigger_down();
    // print_string("Dilithium PolyDecompose Dec\n");


    // simpleserial_init();
    // simpleserial_addcmd('l', 0, vcu_prng_on);
    // simpleserial_addcmd('g', 0, vcu_prng_off);
    // simpleserial_addcmd('p', VCD_SEED_LEN, get_poly);

    // while(1)
    //     simpleserial_cw305_rq_get();

    vcu_prng_on(NULL, 0);
    uint8_t seed[VCD_SEED_LEN] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    //uint8_t seed[VCD_SEED_LEN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    //uint8_t seed[VCD_SEED_LEN] = {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
    for (int i = 0; i < 1; i++) {
        print_string("Iteration ");
        print_u32_int(i);
        print_string("\n");
        get_poly(seed, VCD_SEED_LEN);
        seed[i & 0x1f]++;
    }

}
