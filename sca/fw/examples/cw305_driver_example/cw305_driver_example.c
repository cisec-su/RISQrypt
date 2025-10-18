#include "util.h"
#include "cw305.h"

#ifndef BUFF_LEN
#define BUFF_LEN 32
#endif

int main () {

#ifdef USE_DONE
    volatile int i;
#endif
    char buffer[BUFF_LEN + 1];
    buffer[BUFF_LEN] = '\0';

    print_string("CW305 Communication Example\n\n");
    cw305_trigger_down();

    while (1) {
        cw305_receive_string(buffer, BUFF_LEN);
        print_string("Received from CW305: \n");
        print_string(buffer);
        print_string("\n");
        cw305_transmit_string(buffer, BUFF_LEN);
#ifdef USE_DONE
        cw305_done_set();
#else
        cw305_trigger_up();
#endif
        print_string("Transmitted back to CW305.\n\n");
#ifdef USE_DONE
        while(cw305_is_done() == CW305_DONE_HIGH) {
            print_string("Done not read yet\n");
            for(i = 0; i < 100000; i++);
        }
#else
        cw305_trigger_down();
#endif
    }

    return 0;
}
