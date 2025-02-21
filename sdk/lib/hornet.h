#ifndef HORNET_H_
#define HORNET_H_

#include <stdint.h>


#define BOOTLOADER_BASE_ADDR      ((uint32_t) 0x00000020)
#define PROGRAM_BASE_ADDR         ((uint32_t) 0x00001000)


#include "uart_regs.h"

#define UART_0_BASE_ADDR          ((uint32_t) 0x10008010)
#define UART_0_REGS               ((uart_regs_t*) UART_0_BASE_ADDR)


#include "timer_regs.h"

#define TIMER_BASE_ADDR          ((uint32_t) 0x10008018)
#define TIMER_REGS               ((timer_regs_t*) TIMER_BASE_ADDR)


#include "ntt_lite_regs.h"

#define NTT_LITE_BASE_ADDR        ((uint32_t) 0x10040000)
#define NTT_LITE_REGS             ((ntt_lite_regs_t*) NTT_LITE_BASE_ADDR)


#include "keccak_regs.h"

#define KECCAK_BASE_ADDR          ((uint32_t) 0x10040020)
#define KECCAK_REGS               ((keccak_regs_t*) KECCAK_BASE_ADDR)


#include "x2x_regs.h"

#define X2X_BASE_ADDR             ((uint32_t) 0x10040050)
#define X2X_REGS                  ((x2x_regs_t*) X2X_BASE_ADDR)

#endif