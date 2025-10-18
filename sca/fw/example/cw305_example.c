#include "util.h"
#include "cw305.h"


#define USE_DONE


int main () {

#ifdef USE_DONE
    volatile int i;
#endif
    const int buff_len = 33;
    char buffer[buff_len + 1];
    buffer[buff_len] = '\0';

    print_string("CW305 Communication Example\n\n");
    cw305_trigger_down();

    while (1) {
        cw305_receive_string(buffer, buff_len);
        print_string("Received from CW305: \n");
        print_string(buffer);
        print_string("\n");
        cw305_transmit_string(buffer, buff_len);
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
