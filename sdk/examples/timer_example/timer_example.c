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

    print_string("Timer Example\n");

    timer_start();

    time = timer_read();
    print_string("Timer Start: ");
    print_u32_int(time);
    print_string("\n");

    for (i = 0; i < limit; i++) {
        if (i % 100 == 0) {
            print_string("Count: ");
            time = timer_read();
            print_u32_int(time);
            print_string("\n");
        }
    }

    print_string("Timer End: ");
    time = timer_read();
    print_u32_int(time);
    print_string("\n");


    return 0;
}
