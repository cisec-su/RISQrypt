#include "hornet.h"
#include "uart.h"


void uart_transmit_byte(const char data)
{
    while (((UART_0->status) & UART_TX_STATUS_V));
    UART_0->tx = data;
}


void uart_transmit_string(char const *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        uart_transmit_byte(*data++);
    }
}


void uart_receive_byte(char *data)
{
    while (!((UART_0->status) & UART_RX_STATUS_V));
    *data = UART_0->rx;
}


void uart_receive_string(char *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        uart_receive_byte(data++);
    }
}