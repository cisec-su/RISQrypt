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
#include "indcpa.h"
#include "x2x.h"
#include "masked_indcpa.h"
#include "symmetric.h"


//#define VERBOSE
#define CIPHERGEN_RETURN_HASH
#define OUTPUT_SIZE 16
#define SLEEP_LOOP 1024

uint8_t pk[KYBER_INDCPA_PUBLICKEYBYTES];
uint8_t sk[KYBER_INDCPA_SECRETKEYBYTES];
uint8_t c [KYBER_INDCPA_BYTES         ];
masked_msg mm; 
uint8_t m[MASKING_N]; 
uint8_t rng_buffer[KYBER_SYMBYTES];
masked_polyvec mskpv;
uint8_t pk_dummy[KYBER_INDCPA_PUBLICKEYBYTES];
uint8_t sk_dummy[KYBER_INDCPA_SECRETKEYBYTES];
masked_polyvec mskpv_dummy;


int randombytes(unsigned char *x, unsigned long long xlen) {
    for (int i = 0; i < xlen; i++) {
        x[i] = rng_buffer[i];   
    }
    return 0;
}


uint8_t get_key(uint8_t* k, uint8_t len)
{
#ifdef VERBOSE
    print_string("SimpleSerial::get_key command received\n");
    print_string("k: ");
    print_hex(k, len, 0);
    print_string("\n");
#endif

    for (size_t i = 0; i < KYBER_SYMBYTES; i++) {
        rng_buffer[i] = k[i];
    }

    indcpa_keypair(pk, sk);
    masked_indcpa_dec_init(&mskpv, sk);

    // dummy init to avoid first-run effects
    for (size_t i = 0; i < KYBER_SYMBYTES; i++) {
        rng_buffer[i] = 0;
    }

    indcpa_keypair(pk_dummy, sk_dummy);
    masked_indcpa_dec_init(&mskpv_dummy, sk_dummy);

    for (volatile size_t i = 0; i < SLEEP_LOOP; i++) {
        (void) i;
    }

    cw305_trigger_up();
    masked_indcpa_dec_core(mm, c, &mskpv);

    for (volatile size_t i = 0; i < SLEEP_LOOP; i++) {
        (void) i;
    }

    for (size_t i = 0; i < MASKING_N; i++) {
        for (size_t j = 1; j < KYBER_INDCPA_MSGBYTES; j++) {
            mm[i][0] ^= mm[i][j];
        }
        m[i] = mm[i][0];
    }
    simpleserial_put('r', MASKING_N, (uint8_t*) m);

#ifdef VERBOSE
    print_string("decrypted message: ");
    print_hex(m[0], KYBER_INDCPA_MSGBYTES, 0);
    print_string("\n");
#endif

    return 0x00;
}


uint8_t get_pt(uint8_t* pt, uint8_t len)
{
#ifdef VERBOSE
    print_string("SimpleSerial::get_pt command received\n");
    print_string("pt: ");
    print_hex(pt, len, 0);
    print_string("\n");
#endif

    shake256(c, KYBER_INDCPA_BYTES, pt, len);

#ifdef CIPHERGEN_RETURN_HASH
    simpleserial_put('r', OUTPUT_SIZE, c);
#endif

#ifdef VERBOSE
    print_string("ct set to: ");
    print_hex(c, 16, 0);
    print_string("...");
    print_hex(c + KYBER_INDCPA_BYTES - 16, 16, 0);
    print_string("\n");
#endif

    return 0x00;
}


int main(void)
{
#ifndef PRNG_OFF
    uint32_t seed[] = {0xdeadbeef, 0x21212424};
#endif

    cw305_trigger_down();
    print_string("Kyber CPAPKE Dec\n");

#ifndef PRNG_OFF
    x2x_seed(seed);
#endif

    simpleserial_init();
    simpleserial_addcmd('k', 16, get_pt);
    simpleserial_addcmd('p', KYBER_SYMBYTES, get_key);

    while(1)
        simpleserial_cw305_rq_get();
}
