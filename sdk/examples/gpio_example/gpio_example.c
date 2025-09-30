#include <stdint.h>
#include "string.h"
#include "timer.h"
#include "uart.h"
#include "gpio.h"


int main () {


    uart_transmit_string("GPIO Example\n\n", 15);
    gpio_write(0, 1);
    // gpio_write(1, 0);
    // gpio_write(2, 1);
    // gpio_write(3, 0);


    return 0;
}
