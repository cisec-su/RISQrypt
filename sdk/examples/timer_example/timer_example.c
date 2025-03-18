#include <stdint.h>
#include "string.h"
#include "timer.h"
#include "uart.h"
#include "util.h"


int main () {

    const unsigned int limit = 100000;
    unsigned int i;
    char hex_out[sizeof(uint32_t) << 1];
    unsigned int time;

    uart_transmit_string("Timer Example\n\n", 16);

    timer_start();

    time = timer_read();
    uart_transmit_string("Timer Start: ", 13);
    byte_to_hex(hex_out, (const char *)(&time), sizeof(uint32_t), 1);
    uart_transmit_string(hex_out, sizeof(uint32_t) << 1);
    uart_transmit_string("\n", 1);

    for (i = 0; i < limit; i++) {
        if (i % 100 == 0) {
            uart_transmit_string("Count: ", 7);
            time = timer_read();
            byte_to_hex(hex_out, (const char *)(&time), sizeof(uint32_t), 1);
            uart_transmit_string(hex_out, sizeof(uint32_t) << 1);
            uart_transmit_string("\n", 1);
        }
    }

    uart_transmit_string("Timer End: ", 11);
    time = timer_read();
    byte_to_hex(hex_out, (const char *)(&time), sizeof(uint32_t), 1);
    uart_transmit_string(hex_out, sizeof(uint32_t) << 1);
    uart_transmit_string("\n", 1);


    return 0;
}
