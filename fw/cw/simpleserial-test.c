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

#include "simpleserial.h"
#include "gpio.h"
#include <stdint.h>
#include <stdlib.h>

uint8_t key[16];
volatile uint8_t ct[16];


uint8_t get_key(uint8_t* k, uint8_t len)
{

    // print_string("Setting Key\n");

	for (uint8_t i = 0; i < 16; i++) {
		key[i] = k[i];
	}
	return 0x00;
}

uint8_t get_pt(uint8_t* pt, uint8_t len)
{
    // print_string("Setting Plaintext\n");


    gpio_write(7, 1);

	for (uint8_t i = 0; i < 16; i++) {
		ct[i] = 0;
	}

	for (uint8_t i = 0; i < 16; i++) {
		ct[i] = pt[i] * key[i];
	}

	for (uint8_t i = 0; i < 16; i++) {
		ct[i] = 0;
	}

    gpio_write(7, 0);

    simpleserial_put('r', 16, ct);
	return 0x00;
}


int main(void)
{
    gpio_write(7, 0);
    print_string("SimpleSerial Test\n");

    // platform_init();
    // init_uart();
    // trigger_setup();


    /* Uncomment this to get a HELLO message for debug */

    // putch('h');
    // putch('e');
    // putch('l');
    // putch('l');
    // putch('o');
    // putch('\n');

	simpleserial_init();
    simpleserial_addcmd('p', 16, get_pt);
    simpleserial_addcmd('k', 16, get_key);

    while(1)
        simpleserial_get();
}
