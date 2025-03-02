#ifndef TIMER_REGS_H_
#define TIMER_REGS_H_

#include <stdint.h>


typedef volatile uint32_t IO;

typedef volatile uint32_t I;

typedef const volatile uint32_t O;


typedef struct {
    IO  ctrl;
    O   data;
} timer_regs_t;


#define TIMER_START_S                ((uint32_t) 0  )
#define TIMER_START_M                ((uint32_t) 0x1)
#define TIMER_START_V                (TIMER_START_M << TIMER_START_S)

#define TIMER_RESET_S                ((uint32_t) 1  )
#define TIMER_RESET_M                ((uint32_t) 0x1)
#define TIMER_RESET_V                (TIMER_RESET_M << TIMER_RESET_S)


#endif