#include <stdint.h>
#include "string.h"
#include "timer.h"
#include "uart.h"
#include "util.h"


int main () {

    const unsigned int limit = 100000;
    unsigned int i;
    char hex_out[sizeof(uint64_t) << 1];
    uint64_t time;

    print_string("Timer Example\n");

    timer_start();

    time = timer_read();
    print_string("Timer Start: ");
    byte_to_hex(hex_out, (const uint8_t *)(&time), sizeof(uint64_t), 1);
    uart_transmit_string(hex_out, sizeof(uint64_t) << 1);
    print_string("\n");

    for (i = 0; i < limit; i++) {
        if (i % 100 == 0) {
            print_string("Count: ");
            time = timer_read();
            byte_to_hex(hex_out, (const uint8_t *)(&time), sizeof(uint64_t), 1);
            uart_transmit_string(hex_out, sizeof(uint64_t) << 1);
            print_string("\n");
        }
    }

    print_string("Timer End: ");
    time = timer_read();
    byte_to_hex(hex_out, (const uint8_t *)(&time), sizeof(uint64_t), 1);
    uart_transmit_string(hex_out, sizeof(uint64_t) << 1);
    print_string("\n");


    return 0;
}
