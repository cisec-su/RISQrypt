/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2017 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <stdlib.h>
#include "util.h"
#include "cw305.h"
#include "simpleserial_cw305_rq.h"
#include "x2x.h"
#include "x2x_prng.h"
#include "masked_gadgets.h"
#include "masked_poly.h"
#include "symmetric.h"
//#define VERBOSE
#ifdef VERBOSE
#include "timer.h"
#endif

#define CIPHERGEN_RETURN_HASH
#define OUTPUT_SIZE 16
#define SLEEP_LOOP 1024



poly poly_a;
masked_poly poly_b;
uint8_t poly_bytes[KYBER_POLYBYTES];

masked_poly poly_b_dummy;
uint8_t poly_bytes_dummy[KYBER_POLYBYTES];


masked_msg mm;

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
    ////////////////////// initialize modules ///////////////////////
    poly_init_q();
    masked_gadgets_init_q();
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////// zero public input ///////////////////////
    for (size_t i = 0; i < KYBER_N; i++) {
        poly_a.coeffs[i] = 0;
    }
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////// real input masking //////////////////////
    shake256(poly_bytes, KYBER_POLYBYTES, p, len);
#ifdef VERBOSE
    print_string("Poly Bytes: ");
    print_hex(poly_bytes, KYBER_POLYBYTES, 0);
    print_string("\n");
#endif    
    poly_frombytes(&poly_b.share[0], poly_bytes);
    for (size_t i = 0; i < KYBER_N; i++) {
        poly_b.share[0].coeffs[i] %= KYBER_Q;
    }
#ifdef VERBOSE
    print_string("Poly Coeffs: ");
    print_u32_arr((uint32_t*) poly_b.share[0].coeffs, 8);
    print_u32_arr((uint32_t*) poly_b.share[0].coeffs + KYBER_N/2 - 8, 8);
    print_string("\n");
#endif
    x2x_a_share((uint32_t*) poly_b.share[1].coeffs, (uint32_t*) poly_b.share[0].coeffs, (uint32_t*) poly_b.share[0].coeffs, KYBER_N >> 1);
#ifdef VERBOSE
    print_string("Poly Share 0 Coeffs: ");
    print_u32_arr((uint32_t*) poly_b.share[0].coeffs, 8);
    print_u32_arr((uint32_t*) poly_b.share[0].coeffs + KYBER_N/2 - 8, 8);
    print_string("\n");
    print_string("Poly Share 1 Coeffs: ");
    print_u32_arr((uint32_t*) poly_b.share[1].coeffs, 8);
    print_u32_arr((uint32_t*) poly_b.share[1].coeffs + KYBER_N/2 - 8, 8);
    print_string("\n");
#endif
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    //////////////////// dummy input masking ////////////////////////
    for (size_t i = 0; i < len; i++) {
        temp_buffer[i] = 0;
    }
    shake256(poly_bytes_dummy, KYBER_POLYBYTES, temp_buffer, len);
    poly_frombytes(&poly_b_dummy.share[0], poly_bytes_dummy);
    x2x_a_share((uint32_t*) poly_b_dummy.share[1].coeffs, (uint32_t*) poly_b_dummy.share[0].coeffs, (uint32_t*) poly_b_dummy.share[0].coeffs, KYBER_N >> 1);
    (void) poly_b_dummy;
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////////// sleep ///////////////////////////////
    for (volatile size_t i = 0; i < SLEEP_LOOP; i++) {
        (void) i;
    }
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    ////////////////////// trigger and action ///////////////////////
    cw305_trigger_up();
#ifdef VERBOSE
    timer_reset();
    timer_start();
#endif
    masked_poly_sub_tomsg(mm, &poly_a, &poly_b);
#ifdef VERBOSE
    time = timer_read();
    print_string("masked_poly_sub_tomsg time: ");
    print_u32(time);
    print_string("\n");
#endif
    (void) mm;
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////////// sleep ///////////////////////////////
    for (volatile size_t i = 0; i < SLEEP_LOOP; i++) {
        (void) i;
    }
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    /////////////////////////// masked output ///////////////////////
    for (size_t i = 0; i < OUTPUT_SIZE/2; i++) {
        temp_buffer[i] = mm[0][i];
        temp_buffer[i + OUTPUT_SIZE/2] = mm[1][i];
    }
    simpleserial_put('r', 16, temp_buffer);
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


uint8_t prng_on(uint8_t* p, uint8_t len) {
#ifdef VERBOSE
    print_string("SimpleSerial::prng_on command received\n");
#endif

    uint32_t seed[] = {0xdeadbeef, 0x21212424};
    x2x_seed(seed);

    return 0x00;
}


uint8_t prng_off(uint8_t* p, uint8_t len) {
#ifdef VERBOSE
    print_string("SimpleSerial::prng_off command received\n");
#endif

    x2x_prng_off();

    return 0x00;
}


int main(void)
{
    cw305_trigger_down();
    print_string("Kyber PolyToMsg Dec\n");


    simpleserial_init();
    simpleserial_addcmd('l', 0, prng_on);
    simpleserial_addcmd('g', 0, prng_off);
    simpleserial_addcmd('p', KYBER_SYMBYTES, get_poly);

    while(1)
        simpleserial_cw305_rq_get();
}
