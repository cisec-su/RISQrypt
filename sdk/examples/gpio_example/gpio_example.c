#include "util.h"
#include "gpio.h"


int main () {

    volatile int i;
    
    print_string("GPIO Example\n\n");

    while (1) {

        print_string("Heartbeat!\n\n");

        gpio_write(0, 1);
        gpio_write(1, 0);
        gpio_write(7, 1);

        for (i = 0; i < 1000000; i++);

        gpio_write(0, 0);
        gpio_write(1, 1);
        gpio_write(7, 0);

        for (i = 0; i < 1000000; i++);

    }

    return 0;
}
