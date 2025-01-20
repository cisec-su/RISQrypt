#include "hornet.h"
#include "uart.h"


void uart_transmit_byte(const char data)
{
    char uart_status;

    while (1)
    {
        uart_status = (UART_0->status) & UART_STATUS_V;
        if(!uart_status)
            break;
    }
    UART_0->tx = data;
}


void uart_transmit_string(char const *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        uart_transmit_byte(*data++);
    }
}
