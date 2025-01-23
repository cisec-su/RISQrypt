#ifndef HORNET_H_
#define HORNET_H_

#include <stdint.h>


#include "uart_regs.h"

#define UART_0_BASE_ADDR          ((uint32_t) 0x10008010)
#define UART_0_REGS               ((uart_regs_t*) UART_0_BASE_ADDR)


#include "keccak_regs.h"

#define KECCAK_BASE_ADDR     ((uint32_t) 0x10040020)
#define KECCAK_REGS          ((keccak_regs_t*) KECCAK_BASE_ADDR)

#endif