#include "uart.h"



int main() {
    while(1)
    {
        uart_transmit_string("Hello world!\n",13);
        for (int i = 0; i < 4000000; i++){} 
    }
}
