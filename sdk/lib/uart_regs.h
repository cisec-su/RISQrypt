#ifndef UART_REGS_H_
#define UART_REGS_H_

#include <stdint.h>

typedef volatile       uint8_t IO_U8;
typedef volatile       uint8_t O_U8;
typedef volatile const uint8_t I_U8;

typedef struct {
    O_U8  tx;
    I_U8  rx;
    IO_U8 status;
} uart_t;


#define UART_RX_STATUS_S                ((uint32_t) 0  )
#define UART_RX_STATUS_M                ((uint32_t) 0x1)
#define UART_RX_STATUS_V                (UART_RX_STATUS_M << UART_RX_STATUS_S)

#define UART_TX_STATUS_S                ((uint32_t) 1  )
#define UART_TX_STATUS_M                ((uint32_t) 0x1)
#define UART_TX_STATUS_V                (UART_TX_STATUS_M << UART_TX_STATUS_S)


#endif