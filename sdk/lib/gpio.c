#include "gpio.h"
#include "hornet.h"


int gpio_write(uint8_t pin, uint8_t value) {
    if (pin < 32) {
        if (value) {
            GPIO_REGS->output |=  (1U << pin);
        } else {
            GPIO_REGS->output &= ~(1U << pin);
        }
        return 0;
    } else {
        return -1;
    }
}