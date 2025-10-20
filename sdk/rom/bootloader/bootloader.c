#include <stdlib.h>
#include "uart.h"
#include "irq.h"
#include "hornet.h"


#define ST_INST ( 0)
#define ST_DATA ( 1)
#define ST_DONE ( 2)

char *ram_ptr;
char temp;
uint32_t len;
char *limit;
int state;
uint8_t *len_ptr;
int len_ctr;

int main()
{
    ram_ptr = (char*) INST_BASE_ADDR;
    limit   = 0;
    state   = ST_INST;
    len_ptr = (uint8_t*) &len;
    len_ctr = 0;
    SET_MTVEC_VECTOR_MODE();
    uart_transmit_string("Waiting for opcodes...\n",23);

    ENABLE_GLOBAL_IRQ();
    ENABLE_FAST_IRQ(0);

    while(1);
}

void fast_irq0_handler()
{
    uart_receive_byte(&temp);
    if (state == ST_DONE) {
        return;
    }
    else if (len_ctr < 4)
    {
        len_ptr[len_ctr] = (uint8_t) temp;
        len_ctr++;
        if (len_ctr == 4) {
            if (state == ST_INST) {
                limit = (char*) INST_BASE_ADDR + len;
            }
            else
            {
                limit = (char*) DATA_BASE_ADDR + len;
            }
        }
    }
    else {
        *ram_ptr = temp;
        ram_ptr++;
        if (ram_ptr == limit) {
            if (state == ST_INST) {
                ram_ptr = (char*) DATA_BASE_ADDR;
                state = ST_DATA;
                len_ctr = 0;
            }
            else {
                state = ST_DONE;
                uart_transmit_string("Program load complete.\n",23);
            }
        }
    }
}

void mei_handler() {}

