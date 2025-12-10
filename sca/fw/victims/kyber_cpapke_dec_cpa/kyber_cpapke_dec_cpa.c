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
#ifdef MASK_EN
#include "x2x.h"
#include "masked_indcpa.h"
#endif
#include "symmetric.h"


//#define VERBOSE
#define KEYGEN_RETURN_HASH
#define OUTPUT_SIZE 16
#define DEC_RETURN_MSG


uint8_t pk[KYBER_INDCPA_PUBLICKEYBYTES];
uint8_t sk[KYBER_INDCPA_SECRETKEYBYTES];
uint8_t c [KYBER_INDCPA_BYTES         ];
#ifdef MASK_EN
masked_msg m; 
#else
uint8_t m [KYBER_INDCPA_MSGBYTES      ];
#endif

uint8_t rng_buffer[KYBER_SYMBYTES];

uint32_t hash_buffer[SHA3_256_HASH_SIZE >> 2];


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

#ifdef KEYGEN_RETURN_HASH
    sha3_256((uint8_t*) hash_buffer, sk, KYBER_INDCPA_SECRETKEYBYTES);
    simpleserial_put('r', OUTPUT_SIZE, (uint8_t*) hash_buffer);
#endif
#ifdef VERBOSE
    print_string("sk set to: ");
    print_hex(sk, 16, 0);
    print_string("...");
    print_hex(sk + KYBER_INDCPA_SECRETKEYBYTES - 16, 16, 0);
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

#ifdef VERBOSE
    print_string("ct set to: ");
    print_hex(c, 16, 0);
    print_string("...");
    print_hex(c + KYBER_INDCPA_BYTES - 16, 16, 0);
    print_string("\n");
#endif

    cw305_trigger_up();
#ifdef MASK_EN
    masked_indcpa_dec(m, c, sk);
#else
    indcpa_dec(m, c, sk);
#endif

#ifdef DEC_RETURN_MSG
#ifdef MASK_EN
    for (size_t i = 0; i < OUTPUT_SIZE; i++) {
        m[0][i] ^= m[1][i];
    }
    simpleserial_put('r', OUTPUT_SIZE, (uint8_t*) m[0]);
#else
    simpleserial_put('r', OUTPUT_SIZE, m);
#endif

#endif

#ifdef VERBOSE
    print_string("decrypted message: ");
    print_hex(m[0], KYBER_INDCPA_MSGBYTES, 0);
    print_string("\n");
#endif

    return 0x00;
}


int main(void)
{
#if MASK_EN
    uint32_t seed[] = {0xdeadbeef, 0x21212424};
#endif

    cw305_trigger_down();
    print_string("Kyber CPAPKE Dec\n");

#if MASK_EN
#ifndef PRNG_OFF
    x2x_seed(seed);
#endif
#endif

    simpleserial_init();
    simpleserial_addcmd('p', 16, get_pt);
    simpleserial_addcmd('k', KYBER_SYMBYTES, get_key);

    while(1)
        simpleserial_cw305_rq_get();
}
