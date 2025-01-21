#ifndef HORNET_H_
#define HORNET_H_

#include <stdint.h>


#include "uart_regs.h"

#define UART_0_BASE_ADDR ((uint32_t) 0x10008010)
#define UART_0           ((uart_t*) UART_0_BASE_ADDR)


#endif