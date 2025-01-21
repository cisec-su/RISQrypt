#include <string.h>
#include "uart.h"



int main() {

    char data[2];

    while(1)
    {
        uart_receive_string(data, 2);
        if (memcmp(data, "go", 2) == 0)
        {
            break;
        }
    }

    while(1)
    {
        uart_transmit_string("Hello world!\n", 13);
        for (int i = 0; i < 4000000; i++){} 
    }
}
