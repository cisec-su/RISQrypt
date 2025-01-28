#include <stdlib.h>
#include "uart.h"
#include "irq.h"
#include "hornet.h"

char *ram_ptr;

int main()
{
    ram_ptr = (char*) PROGRAM_BASE_ADDR;
    SET_MTVEC_VECTOR_MODE();
    uart_transmit_string("Waiting for opcodes...\n",23);

    ENABLE_GLOBAL_IRQ();
    ENABLE_FAST_IRQ(0);

    while(1);
}

void fast_irq0_handler()
{
    uart_receive_byte(ram_ptr);
    ram_ptr++;
}

void mei_handler() {}

