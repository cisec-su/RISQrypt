#include <stdint.h>
#include "string.h"
#include "timer.h"
#include "uart.h"
#include "util.h"
#include "indcpa.h"

uint8_t m[KYBER_INDCPA_MSGBYTES];
uint8_t c_dec[KYBER_INDCPA_MSGBYTES];
uint8_t c[KYBER_INDCPA_BYTES];
uint8_t sk[KYBER_INDCPA_SECRETKEYBYTES];


int main () {

    const unsigned int limit = 100000;
    unsigned int i;
    char hex_out[sizeof(uint32_t) << 1];
    unsigned int time;

    for (i = 0; i < KYBER_INDCPA_MSGBYTES; i++) {
        m[i] = i;
    }

    for (i = 0; i < KYBER_INDCPA_BYTES; i++) {
        c[i] = i;
    }

    for (i = 0; i < KYBER_INDCPA_SECRETKEYBYTES; i++) {
        sk[i] = i;
    }


    uart_transmit_string("Timer Example\n\n", 16);

    // timer_start();

    indcpa_dec(c_dec, c, sk);

    // time = timer_read();
    // uart_transmit_string("Time: ", 6);
    // byte_to_hex(hex_out, (const char *)(&time), sizeof(uint32_t), 1);
    // uart_transmit_string(hex_out, sizeof(uint32_t) << 1);
    // uart_transmit_string("\n", 1);

    if (memcmp(c_dec, m, KYBER_INDCPA_MSGBYTES) == 0) {
        uart_transmit_string("PASS\n\n", 6);
    } else {
        uart_transmit_string("FAIL\n\n", 6);
    }

    return 0;
}
