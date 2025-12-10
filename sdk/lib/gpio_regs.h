#ifndef GPIO_REGS_H_
#define GPIO_REGS_H_

#include <stdint.h>


typedef volatile uint32_t IO;

typedef volatile uint32_t I;

typedef const volatile uint32_t O;


typedef struct {
    O   input;
    IO  output;
    IO  direction;
} gpio_regs_t;



#endif