#include "util.h"
#include "cw305.h"


int main () {

    volatile int i;
    char buffer[33];
    buffer[32] = '\0';
    
    print_string("CW305 Communication Example\n\n");

    while (1) {
        cw305_receive_string(buffer, 32);
        print_string("Received from CW305: \n");
        print_string(buffer);
        print_string("\n");
        cw305_trigger_up();
        cw305_transmit_string(buffer, 32);
        print_string("Transmitted back to CW305.\n\n");
        cw305_trigger_down();
    }

    return 0;
}
