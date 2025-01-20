#ifndef UART_H_
#define UART_H_

#include <stdlib.h>


void uart_transmit_byte(const char data);
void uart_transmit_string(char const *data, size_t len);



#endif