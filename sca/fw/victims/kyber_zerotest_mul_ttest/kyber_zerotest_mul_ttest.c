#include <stdint.h>
#include <stdlib.h>
#include "util.h"
#include "cw305.h"
#include "simpleserial_cw305_rq.h"
#include "ntt_lite.h"
#include "masked_gadgets.h"
#include "timer.h"
#include "masked_gadgets.h"
#include "victims_commons_util.h"


#define SLEEP_LOOP 1024



uint8_t get_poly(uint8_t* p, uint8_t len)
{
    int ret;
    const uint32_t mu[2] = {MU_EXP_L, MU_EXP_H};
    masked_u32 *a = (masked_u32*) p;
#ifdef VERBOSE
    uint32_t time;
    print_string("SimpleSerial::get_poly command received\n");
    print_string("p: ");
    print_hex(p, len, 0);
    print_string("\n");
#endif
    ntt_lite_load_q(Q_EXP, mu, 8, 32, 0, NTT_LITE_MODE_SINGLE);
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////////// sleep ///////////////////////////////
    for (int i = 0; i < MASKING_N; i++) {
        *a[i][1] = 0;
    }
    vcu_sleep(SLEEP_LOOP);
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    ////////////////////// trigger and action ///////////////////////
    cw305_trigger_up();
#ifdef VERBOSE
    timer_reset();
    timer_start();
#endif
    ret = masked_gadgets_zero_test_mul(*a);
    (void) ret;
#ifdef VERBOSE
    time = timer_read();
    print_string("masked_gadgets_zero_test_mul time: ");
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
    print_string("SimpleSerial::get_key done\n");
#endif
    return 0x00;
}


int main(void)
{
    cw305_trigger_down();
    print_string("Kyber ZeroTest Mul\n");


    simpleserial_init();
    simpleserial_addcmd('l', 8, vcu_prng_on);
    simpleserial_addcmd('g', 0, vcu_prng_off);
    simpleserial_addcmd('p', 16, get_poly);

    while(1)
        simpleserial_cw305_rq_get();
}
