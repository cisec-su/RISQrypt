#include <stdint.h>
#include "victims_commons_util.h"
#include "timer.h"
#include "ntt_lite.h"
#include "x2x.h"
#include "x2x_prng.h"
#include "util.h"

const static uint32_t zero[128] = {0};


void vcu_ntt_lite_reset_state(void) {
    ntt_lite_add(0x0, zero, zero);
}


void vcu_sleep(uint32_t bound) {
    timer_reset();
    timer_start();
    while (timer_read() < bound);
    timer_reset();
}


uint8_t vcu_prng_on(uint8_t* p, uint8_t len) {
#ifdef VERBOSE
    print_string("SimpleSerial::prng_on command received\n");
#endif

    uint32_t seed[] = {0xdeadbeef, 0xfeeddead};
    x2x_seed(seed);

    return 0x00;
}


uint8_t vcu_prng_off(uint8_t* p, uint8_t len) {
#ifdef VERBOSE
    print_string("SimpleSerial::prng_off command received\n");
#endif

    x2x_prng_off();

    return 0x00;
}