#include <stdint.h>
#include "victims_commons_util.h"
#include "timer.h"
#include "ntt_lite.h"
#include "x2x.h"
#include "x2x_prng.h"
#include "util.h"

const static uint32_t zero[256] = {0};


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

    uint32_t seed[2];
    seed[0] = ((uint32_t) p[0]) | (((uint32_t) p[1]) << 8) | (((uint32_t) p[2]) << 16) | (((uint32_t) p[3]) << 24);
    seed[1] = ((uint32_t) p[4]) | (((uint32_t) p[5]) << 8) | (((uint32_t) p[6]) << 16) | (((uint32_t) p[7]) << 24);
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


__attribute__((noinline))
void zero_stack(uint32_t size)
{
    uintptr_t sp;
    asm volatile("mv %0, sp" : "=r"(sp));

    volatile uint32_t *p = (volatile uint32_t *)(sp - (size << 2));
    for (size_t i = 0; i < size; i++) {
        p[i] = 0;
    }

    asm volatile("" ::: "memory");
}